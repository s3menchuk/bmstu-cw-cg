// clang-format off
#include <glad/glad.h> // glad must be included before glfw
#include <GLFW/glfw3.h>
// clang-format on

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

// Функция для загрузки шейдера из файла
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

// Компиляция шейдера
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

// Создание шейдерной программы из файлов
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

int main() {
    int WIDTH = 1920;
    int HEIGHT = 1080;

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Graphics Engine", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // отключить V-Sync
    // glfwSwapInterval(0);
    // 0 = без синхронизации, 1 = синхронизация с монитором

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    // Шейдерная программа
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, accumTex[i], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "FBO " << i << " not complete!" << std::endl;
    }

    int frame_num = 0;

    double last_time = glfwGetTime();
    int last_frame = frame_num;

    int ping = 0; // индекс для ping-pong

    while (!glfwWindowShouldClose(window)) {
        frame_num++;

        ping = frame_num % 2; // чередуем текстуры

        double curr_time = glfwGetTime();
        if (curr_time - last_time >= 1.0) {
            double fps = double(frame_num - last_frame) / (curr_time - last_time);
            std::string title = "Graphics Engine | FPS: " + std::to_string(int(fps));
            glfwSetWindowTitle(window, title.c_str());
            last_time = curr_time;
            last_frame = frame_num;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, accumFBO[ping]);
        glViewport(0, 0, WIDTH, HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(tracer_shader);

        // uniform frameIndex / time
        glUniform1i(glGetUniformLocation(tracer_shader, "FrameNum"), frame_num);
        glUniform1f(glGetUniformLocation(tracer_shader, "Time"), glfwGetTime());

        // передаём предыдущую текстуру
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, accumTex[1 - ping]); // читаем старый кадр
        glUniform1i(glGetUniformLocation(tracer_shader, "AccumTex"), 0);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // теперь выводим на экран
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, WIDTH, HEIGHT);

        glUseProgram(screen_shader); // простой shader для отображения текстуры
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, accumTex[ping]);
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