#include "Engine.hpp"
#include <iostream>

// TODO: move these into a settings struct
#define WIDTH 1920
#define HEIGHT 1080

float vertices[] = {
    -1.0f, 1.0f, 0.0f,
    3.0f, 1.0f, 0.0f,
    -1.0f, -3.0, 0.0f
};

// float vertices[] = {
//     -0.5f, -0.5f, 0.0f,
//      0.5f, -0.5f, 0.0f,
//      0.0f,  0.5f, 0.0f
// };

Engine::Engine() {}

void Engine::run() {
    initWindow();
    initOpengl();
    mainLoop();
}

void Engine::initWindow() {
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize glfw");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // neccessary for OS X support
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE); // maybe change this to disable FPS cap?

    this->window = glfwCreateWindow(WIDTH, HEIGHT, "Fractal Box", NULL, NULL);
    if (this->window == NULL) {
        throw std::runtime_error("Failed to create window");
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to init GLAD");
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    glfwSetKeyCallback(window, key_callback);
//    glfwSetScrollCallback(window, scroll_callback); // TODO: uncomment and implement arrow keys
}

void Engine::initOpengl() {
    glViewport(0, 0, WIDTH, HEIGHT); // TODO: update these coordinates to make fractal math easier
    this->initBuffers();
    this->initPipeline();
}

void Engine::initBuffers() {
    glGenVertexArrays(1, &(this->VAO));
    glBindVertexArray(this->VAO);

    glGenBuffers(1, &this->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
}

void Engine::initPipeline() {
    this->fractal = new Fractal();
    unsigned int vertexShader = this->fractal->vertShader;
    int success;
    char infoLog[1025];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
        printf("Vertex shader compilation error: %s\n", infoLog);
        throw std::runtime_error("Failed to compile vertex shader");
    }

    unsigned int fragmentShader = this->fractal->fragShader;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
        // printf("Fragment shader compilation error: %s\n", infoLog);
        std::cerr << "Fragment shader compilation error: " << infoLog << std::endl;
        // throw std::runtime_error("Failed to compile fragment shader");
    }

    this->shaderProgram = glCreateProgram();
    glAttachShader(this->shaderProgram, vertexShader);
    glAttachShader(this->shaderProgram, fragmentShader);
    glLinkProgram(this->shaderProgram);
    glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(this->shaderProgram, 1024, NULL, infoLog);
        printf("Shader program linking error: %s\n", infoLog);
        throw std::runtime_error("Failed to link shader program");
    } else {
        printf("Pipeline assembled\n");
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
};

void Engine::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(this->shaderProgram);
        glBindVertexArray(this->VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        // glFinish();
        glfwPollEvents();
    }

}
void Engine::cleanup() {
    glfwDestroyWindow(this->window);
    glfwTerminate();
}

void Engine::key_callback(GLFWwindow *keyWindow, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(keyWindow, GLFW_TRUE);
    }
}

void Engine::error_callback(int error, const char *description) {
    fprintf(stderr, "Error: %s\n", description);
}