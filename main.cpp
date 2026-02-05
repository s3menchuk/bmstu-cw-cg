#include "Camera.hpp"
#include "Settings.hpp"
#include "Vec.hpp"

// clang-format off
#include <glad/glad.h> // glad must be included before glfw
#include <GLFW/glfw3.h>
// clang-format on
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct VertexGPU {
    uint32_t coord;
    uint32_t normal;
    uint32_t texture;
    uint32_t pad;
};

struct TriangleGPU {
    VertexGPU a;
    VertexGPU b;
    VertexGPU c;
};

struct Vertex {
    glm::vec4 coord;
    glm::vec4 normal;
    glm::vec4 texture;
};

struct Triangle {
    Vertex a;
    Vertex b;
    Vertex c;
};

void create_triangles_from_faces(const std::vector<glm::vec4> &coords, const std::vector<TriangleGPU> &faces, const std::vector<glm::vec4> &normals,
                                 const std::vector<glm::vec4> &textures, std::vector<Triangle> &triangles) {
    for (auto face : faces) {
        Triangle triangle;
        triangle.a.coord = coords[face.a.coord];
        triangle.a.normal = normals[face.a.normal];
        triangle.a.texture = textures[face.a.texture];
        triangle.b.coord = coords[face.b.coord];
        triangle.b.normal = normals[face.b.normal];
        triangle.b.texture = textures[face.b.texture];
        triangle.c.coord = coords[face.c.coord];
        triangle.c.normal = normals[face.c.normal];
        triangle.c.texture = textures[face.c.texture];
        triangles.push_back(triangle);
    }
}

bool load(const std::string &path, std::vector<glm::vec4> &coords, std::vector<TriangleGPU> &faces, std::vector<glm::vec4> &normals,
          std::vector<glm::vec4> &textures) {
    std::ifstream file(path);

    if (!file)
        return false;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            glm::vec4 coord;
            iss >> coord.x >> coord.y >> coord.z;
            coords.push_back(coord);
        } else if (prefix == "vn") {
            glm::vec4 normal;
            iss >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        } else if (prefix == "vt") {
            glm::vec4 texture;
            iss >> texture.x >> texture.y;
            textures.push_back(texture);
        } else if (prefix == "f") {
            std::string vertex_token;
            TriangleGPU face;
            VertexGPU *vertex = &face.a;
            while (iss >> vertex_token) {
                std::replace(vertex_token.begin(), vertex_token.end(), '/', ' ');
                std::istringstream viss(vertex_token);
                viss >> vertex->coord >> vertex->texture >> vertex->normal;
                vertex->coord--;
                vertex->texture--;
                vertex->normal--;
                vertex++;
            }
            faces.push_back(face);
        }
    }

    return true;
}

static Camera camera(DefaultSettings::CAMERA_POS, DefaultSettings::CAMERA_DIR, DefaultSettings::WORLD_UP, DefaultSettings::FOV_Y,
                     DefaultSettings::ASPECT, DefaultSettings::CAMERA_NEAR, DefaultSettings::CAMERA_FAR);

static int frame_num = 0;
static bool camera_mode = false;

std::string loadShaderSource(const std::string &filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filepath << "\n";
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

unsigned int compileShader(const std::string &source, GLenum type) {
    unsigned int shader = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed:\n" << infoLog << "\n";
    }
    return shader;
}

unsigned int createShaderProgram(const std::string &vertexPath, const std::string &fragmentPath) {
    std::string vertexCode = loadShaderSource(vertexPath);
    std::string fragmentCode = loadShaderSource(fragmentPath);

    unsigned int vertexShader = compileShader(vertexCode, GL_VERTEX_SHADER);
    unsigned int fragmentShader = compileShader(fragmentCode, GL_FRAGMENT_SHADER);

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Shader linking failed:\n" << infoLog << "\n";
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

void handle_keystrokes(GLFWwindow *window, Camera &camera) {
    bool is_key_pressed = false;

    // Movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.pos += camera.dir * DefaultSettings::CAMERA_MOVEMENT_SPEED;
        is_key_pressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.pos -= camera.dir * DefaultSettings::CAMERA_MOVEMENT_SPEED;
        is_key_pressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.pos += camera.right * DefaultSettings::CAMERA_MOVEMENT_SPEED;
        is_key_pressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.pos -= camera.right * DefaultSettings::CAMERA_MOVEMENT_SPEED;
        is_key_pressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
        camera.pos += DefaultSettings::WORLD_UP * DefaultSettings::CAMERA_MOVEMENT_SPEED;
        is_key_pressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
        camera.pos -= DefaultSettings::WORLD_UP * DefaultSettings::CAMERA_MOVEMENT_SPEED;
        is_key_pressed = true;
    }

    // Rotation
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && camera.get_zenith() <= DefaultSettings::MAX_ZENITH_RADIANS) {
        camera.rotate_vertically(DefaultSettings::CAMERA_ROTATION_SPEED);
        is_key_pressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && camera.get_zenith() >= -DefaultSettings::MAX_ZENITH_RADIANS) {
        camera.rotate_vertically(-DefaultSettings::CAMERA_ROTATION_SPEED);
        is_key_pressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        camera.rotate_by(DefaultSettings::WORLD_UP, DefaultSettings::CAMERA_ROTATION_SPEED);
        is_key_pressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        camera.rotate_by(DefaultSettings::WORLD_UP, -DefaultSettings::CAMERA_ROTATION_SPEED);
        is_key_pressed = true;
    }

    if (is_key_pressed) {
        frame_num = 0;
    }
}

void SetCursorPos_callback(GLFWwindow *window, double xpos, double ypos) {
    if (!camera_mode)
        return;

    static double lastX, lastY;
    static bool firstMouse = true;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos;  // инвертируем Y: вверх = положительно
    lastX = xpos;
    lastY = ypos;

    xoffset *= DefaultSettings::MOUSE_SENSITIVITY;
    yoffset *= DefaultSettings::MOUSE_SENSITIVITY;

    if ((yoffset > 0 && camera.get_zenith() < DefaultSettings::MAX_ZENITH_RADIANS) ||
        (yoffset < 0 && camera.get_zenith() > -DefaultSettings::MAX_ZENITH_RADIANS)) {
        camera.rotate_vertically(yoffset * DefaultSettings::CAMERA_ROTATION_SPEED);
    }

    camera.rotate_by(DefaultSettings::WORLD_UP, -xoffset * DefaultSettings::CAMERA_ROTATION_SPEED);
    frame_num = 0;
}

template <typename T>
GLuint ssbo_load(const std::vector<T> &data, GLuint binding) {
    GLuint ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(T) * data.size(), data.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    return ssbo;
}

// coords vec3
// indexes vec3i
// normals vec3
// colors vec4

// textures

// objects (offset, count, color)

struct TBO {
    GLuint buf = 0;
    GLuint tex = 0;
};

template <typename T>
TBO tbo_load(const std::vector<T> &data, GLenum internal_format, GLuint shader) {
    static_assert(sizeof(T) % 4 == 0, "TBO element must be 4-byte aligned");

    TBO tbo;

    glGenBuffers(1, &tbo.buf);
    glBindBuffer(GL_TEXTURE_BUFFER, tbo.buf);
    glBufferData(GL_TEXTURE_BUFFER, sizeof(T) * data.size(), data.data(), GL_STATIC_DRAW);

    glGenTextures(1, &tbo.tex);
    glBindTexture(GL_TEXTURE_BUFFER, tbo.tex);
    glTexBuffer(GL_TEXTURE_BUFFER, internal_format, tbo.buf);

    glBindBuffer(GL_TEXTURE_BUFFER, 0);
    glBindTexture(GL_TEXTURE_BUFFER, 0);

    return tbo;
}

bool save_to_ppm_binary(const std::vector<uint8_t> &image, const std::string &filename) {
    std::ofstream fout(filename, std::ios::binary);
    if (!fout.is_open())
        return false;
    fout << "P6" << '\n';
    fout << DefaultSettings::WIDTH << ' ' << DefaultSettings::HEIGHT << '\n';
    fout << 255 << '\n';
    for (size_t row = 0; row < DefaultSettings::HEIGHT; ++row) {
        for (size_t col = 0; col < DefaultSettings::WIDTH; ++col) {
            auto color = image[(row * col + col) * 3];
            fout.write(reinterpret_cast<char *>(&color), 3);
        }
    }
    fout.close();
    return true;
}

void SaveScreenshot(GLFWwindow *window) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    std::vector<unsigned char> pixels(width * height * 3);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // flip vertically
    for (int y = 0; y < height / 2; y++)
        for (int x = 0; x < width * 3; x++)
            std::swap(pixels[y * width * 3 + x], pixels[(height - 1 - y) * width * 3 + x]);

    save_to_ppm_binary(pixels, "image.ppm");
    // stbi_write_png("output.png", width, height, 3, pixels.data(), width * 3);
}

void SetMouseButton_callback(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        camera_mode = !camera_mode;
    }

    if (camera_mode) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

// void WindowSize_callback(GLFWwindow *window, int width, int height) {
//     glViewport(0, 0, width, height);
// }

void error_callback(int error, const char *description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

int main() {
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(DefaultSettings::WIDTH, DefaultSettings::HEIGHT, DefaultSettings::APP_NAME.c_str(), nullptr, nullptr);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetCursorPosCallback(window, SetCursorPos_callback);
    glfwSetMouseButtonCallback(window, SetMouseButton_callback);
    // glfwSetWindowSizeCallback(window, WindowSize_callback);

    // отключить V-Sync; 0 = без синхронизации, 1 = синхронизация с монитором
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    int fbw, fbh;
    glfwGetFramebufferSize(window, &fbw, &fbh);

    // OpenGL and GLSL versions
    // std::cout << "OpenGL: " << glGetString(GL_VERSION) << std::endl;
    // std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    unsigned int tracer_shader = createShaderProgram("shaders/tracer.vert", "shaders/tracer.frag");
    unsigned int screen_shader = createShaderProgram("shaders/screen.vert", "shaders/screen.frag");

    // clang-format off
    const GLfloat SCREEN_TRIANGLE_VERTICES[] = {
        -1.0f, -1.0f,
         3.0f, -1.0f,
        -1.0f,  3.0f,
    };
    // clang-format on

    // clang-format off
    const GLfloat SCREEN_QUAD_VERTICES[] = {
        // positions   // texCoords
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f
    };
    // clang-format on

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SCREEN_TRIANGLE_VERTICES), SCREEN_TRIANGLE_VERTICES, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    GLuint accumFBO[2];
    GLuint accumTex[2];

    glGenFramebuffers(2, accumFBO);
    glGenTextures(2, accumTex);

    float scale = 1.0f;

    for (int i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, accumFBO[i]);
        glBindTexture(GL_TEXTURE_2D, accumTex[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, DefaultSettings::WIDTH * scale, DefaultSettings::HEIGHT * scale, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, accumTex[i], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "FBO " << i << " not complete!" << std::endl;
    }

    std::vector<glm::vec4> coords;
    std::vector<TriangleGPU> faces;
    std::vector<glm::vec4> normals;
    std::vector<glm::vec4> textures;

    load("assets/models/utah_teapot-res2_unit.obj", coords, faces, normals, textures);
    // faces.clear();

    glUseProgram(tracer_shader);
    TBO coordsTBO = tbo_load(coords, GL_RGBA32F, tracer_shader);
    TBO facesTBO = tbo_load(faces, GL_RGBA32UI, tracer_shader);
    TBO normalsTBO = tbo_load(normals, GL_RGBA32F, tracer_shader);
    TBO texturesTBO = tbo_load(textures, GL_RGBA32F, tracer_shader);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_BUFFER, coordsTBO.tex);
    glUniform1i(glGetUniformLocation(tracer_shader, "CoordsBuffer"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_BUFFER, facesTBO.tex);
    glUniform1i(glGetUniformLocation(tracer_shader, "TrianglesBuffer"), 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_BUFFER, normalsTBO.tex);
    glUniform1i(glGetUniformLocation(tracer_shader, "NormalsBuffer"), 3);

    // glActiveTexture(GL_TEXTURE4);
    // glBindTexture(GL_TEXTURE_BUFFER, texturesTBO.tex);
    // glUniform1i(glGetUniformLocation(tracer_shader, "TexturesBuffer"), 4);

    // glUseProgram(tracer_shader);
    // std::vector<glm::vec4> colors(5);
    // colors[0] = {1.0f, 0.0f, 0.0f, 1.0f};
    // TBO colorsTBO = tbo_load(colors, GL_RGBA32F);
    // glActiveTexture(GL_TEXTURE4);
    // glBindTexture(GL_TEXTURE_BUFFER, colorsTBO.tex);
    // glUniform1i(glGetUniformLocation(tracer_shader, "ColorsBuffer"), 4);

    int total_curr_frame = 0;
    int total_last_frame = 0;

    double last_time = glfwGetTime();

    int ping_pong_buffer = 0;

    bool is_save = false;

    while (!glfwWindowShouldClose(window)) {
        handle_keystrokes(window, camera);
        frame_num++;
        total_curr_frame++;

        ping_pong_buffer = frame_num % 2;  // ping-pong

        double curr_time = glfwGetTime();
        if (curr_time - last_time >= 1.0) {
            double fps = double(total_curr_frame - total_last_frame) / (curr_time - last_time);
            std::string title = "Graphics Engine | FPS: " + std::to_string(int(fps));
            glfwSetWindowTitle(window, title.c_str());
            last_time = curr_time;
            total_last_frame = total_curr_frame;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, accumFBO[ping_pong_buffer]);
        glViewport(0, 0, DefaultSettings::WIDTH * scale, DefaultSettings::HEIGHT * scale);

        glUseProgram(tracer_shader);
        glUniform1i(glGetUniformLocation(tracer_shader, "FrameNum"), frame_num);
        glUniform1f(glGetUniformLocation(tracer_shader, "Time"), glfwGetTime());
        glUniform3f(glGetUniformLocation(tracer_shader, "CameraPos"), camera.pos.x, camera.pos.y, camera.pos.z);
        glUniform3f(glGetUniformLocation(tracer_shader, "CameraDir"), camera.dir.x, camera.dir.y, camera.dir.z);
        glUniform1i(glGetUniformLocation(tracer_shader, "MaxRayBounces"), DefaultSettings::max_ray_bounces);
        glUniform1i(glGetUniformLocation(tracer_shader, "SamplesPerPixel"), DefaultSettings::samples_per_pixel);

        glUniform1i(glGetUniformLocation(tracer_shader, "CountTriangles"), faces.size());

        // передаём предыдущую текстуру
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, accumTex[1 - ping_pong_buffer]);  // читаем старый кадр
        glUniform1i(glGetUniformLocation(tracer_shader, "AccumTex"), 0);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // теперь выводим на экран
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, fbw, fbh);

        glUseProgram(screen_shader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, accumTex[ping_pong_buffer]);
        glUniform1i(glGetUniformLocation(screen_shader, "ScreenTex"), 0);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();

        // if (!is_save && total_curr_frame > 100) {
        //     size_t size = Settings::WIDTH * Settings::HEIGHT * 4;
        //     std::vector<uint8_t> image(size);
        //     glGetTextureImage(accumTex[1 - ping_pong_buffer], 0, GL_RGBA, GL_UNSIGNED_BYTE, size, image.data());
        //     // save_to_ppm_binary(image, "image.ppm");
        //     SaveScreenshot(window);
        //     is_save = true;
        // }
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(tracer_shader);
    glDeleteProgram(screen_shader);

    glfwTerminate();
    return 0;
}

// Init, CreateWindow, MakeContext, Main loop (not is_close + poll events + swap buffers), Terminate