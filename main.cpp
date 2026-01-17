#include "Camera.hpp"
#include "Settings.hpp"
#include "Vec.hpp"

// clang-format off
#include <glad/glad.h> // glad must be included before glfw
#include <GLFW/glfw3.h>
// clang-format on

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

static Camera camera(Settings::CAMERA_POS, Settings::CAMERA_DIR, Settings::WORLD_UP, Settings::FOV_Y, Settings::ASPECT, Settings::CAMERA_NEAR,
                     Settings::CAMERA_FAR);

static int frame_num = 0;
static int last_frame = frame_num;

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
        camera.pos += camera.dir * Settings::CAMERA_MOVEMENT_SPEED;
        is_key_pressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.pos -= camera.dir * Settings::CAMERA_MOVEMENT_SPEED;
        is_key_pressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.pos += camera.right * Settings::CAMERA_MOVEMENT_SPEED;
        is_key_pressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.pos -= camera.right * Settings::CAMERA_MOVEMENT_SPEED;
        is_key_pressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
        camera.pos += camera.up * Settings::CAMERA_MOVEMENT_SPEED;
        is_key_pressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
        camera.pos -= camera.up * Settings::CAMERA_MOVEMENT_SPEED;
        is_key_pressed = true;
    }

    // Rotation
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && camera.get_zenith() <= Settings::MAX_ZENITH_RADIANS) {
        camera.rotate_vertically(Settings::CAMERA_ROTATION_SPEED);
        is_key_pressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && camera.get_zenith() >= -Settings::MAX_ZENITH_RADIANS) {
        camera.rotate_vertically(-Settings::CAMERA_ROTATION_SPEED);
        is_key_pressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        camera.rotate_by(Settings::WORLD_UP, Settings::CAMERA_ROTATION_SPEED);
        is_key_pressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        camera.rotate_by(Settings::WORLD_UP, -Settings::CAMERA_ROTATION_SPEED);
        is_key_pressed = true;
    }

    if (is_key_pressed) {
        frame_num = 0;
        last_frame = frame_num;
    }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
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

    xoffset *= Settings::MOUSE_SENSITIVITY;
    yoffset *= Settings::MOUSE_SENSITIVITY;

    if ((yoffset > 0 && camera.get_zenith() < Settings::MAX_ZENITH_RADIANS) || (yoffset < 0 && camera.get_zenith() > -Settings::MAX_ZENITH_RADIANS)) {
        camera.rotate_vertically(yoffset * Settings::CAMERA_ROTATION_SPEED);
    }

    camera.rotate_by(Settings::WORLD_UP, -xoffset * Settings::CAMERA_ROTATION_SPEED);
    frame_num = 0;
    last_frame = frame_num;
}

int main() {
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(Settings::WIDTH, Settings::HEIGHT, Settings::APP_NAME.c_str(), nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // glfwSetKeyCallback(window, handle_keystrokes);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    // отключить V-Sync
    // glfwSwapInterval(0);
    // 0 = без синхронизации, 1 = синхронизация с монитором

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    unsigned int tracer_shader = createShaderProgram("shaders/tracer.vert", "shaders/tracer.frag");
    unsigned int screen_shader = createShaderProgram("shaders/screen.vert", "shaders/screen.frag");

    // clang-format off
    GLfloat vertices[] = {
        -1.0f, -1.0f,
         3.0f, -1.0f,
        -1.0f,  3.0f,
    };
    // clang-format on

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    GLuint accumFBO[2];
    GLuint accumTex[2];

    glGenFramebuffers(2, accumFBO);
    glGenTextures(2, accumTex);

    for (int i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, accumFBO[i]);
        glBindTexture(GL_TEXTURE_2D, accumTex[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, Settings::WIDTH, Settings::HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, accumTex[i], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "FBO " << i << " not complete!" << std::endl;
    }

    double last_time = glfwGetTime();

    int ping_pong_buffer = 0;

    while (!glfwWindowShouldClose(window)) {
        handle_keystrokes(window, camera);
        frame_num++;

        ping_pong_buffer = frame_num % 2;  // ping-pong

        double curr_time = glfwGetTime();
        if (curr_time - last_time >= 1.0) {
            double fps = double(frame_num - last_frame) / (curr_time - last_time);
            std::string title = "Graphics Engine | FPS: " + std::to_string(int(fps));
            glfwSetWindowTitle(window, title.c_str());
            last_time = curr_time;
            last_frame = frame_num;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, accumFBO[ping_pong_buffer]);
        glViewport(0, 0, Settings::WIDTH, Settings::HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(tracer_shader);
        glUniform1i(glGetUniformLocation(tracer_shader, "FrameNum"), frame_num);
        glUniform1f(glGetUniformLocation(tracer_shader, "Time"), glfwGetTime());
        glUniform3f(glGetUniformLocation(tracer_shader, "CameraPos"), camera.pos.x, camera.pos.y, camera.pos.z);
        glUniform3f(glGetUniformLocation(tracer_shader, "CameraDir"), camera.dir.x, camera.dir.y, camera.dir.z);

        // передаём предыдущую текстуру
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, accumTex[1 - ping_pong_buffer]);  // читаем старый кадр
        glUniform1i(glGetUniformLocation(tracer_shader, "AccumTex"), 0);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // теперь выводим на экран
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, Settings::WIDTH, Settings::HEIGHT);

        glUseProgram(screen_shader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, accumTex[ping_pong_buffer]);
        glUniform1i(glGetUniformLocation(screen_shader, "ScreenTex"), 0);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(tracer_shader);

    glfwTerminate();
    return 0;
}

// Init, CreateWindow, MakeContext, Main loop (not is_close + poll events + swap buffers), Terminate