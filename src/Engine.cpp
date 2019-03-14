#include "Engine.hpp"
#include "App.hpp"
#include <iostream>

// #define GLM_GTX_dual_quaternion
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

// TODO: move these into a settings struct
#define WIDTH 1920
#define HEIGHT 1080

float vertices[] = {
    -1.0f, 1.0f, 0.0f,
    3.0f, 1.0f, 0.0f,
    -1.0f, -3.0, 0.0f
};

Engine::Engine() {}

void Engine::run() {
    initWindow();
    initOpengl();
    this->readMouse = true;
    mainLoop();
}

void Engine::initWindow() {
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize glfw");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // neccessary for OS X support
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

//TODO: make these adjustable at runtime
#define MAXSTEPS 100
#define MINDIST 0.0001
void Engine::updateUniforms() {
    static bool firstUpdate = true;
    firstUpdate = false;

    int screenSize_loc = glGetUniformLocation(shaderProgram, "screenSize");
    int fov_loc = glGetUniformLocation(shaderProgram, "FOV");
    int camWorldMat_loc = glGetUniformLocation(shaderProgram, "camToWorldMat");
    int worldCamMat_loc = glGetUniformLocation(shaderProgram, "worldToCamMat");
    int maxSteps_loc = glGetUniformLocation(shaderProgram, "maxSteps");
    int minDist_loc = glGetUniformLocation(shaderProgram, "minDist");

    glUniform2f(screenSize_loc, WIDTH, HEIGHT);
    glUniform1f(fov_loc, this->cam.fov);
    glUniformMatrix4fv(camWorldMat_loc, 1, GL_FALSE, glm::value_ptr(this->cam.camToWorldMat()));
    glUniformMatrix4fv(worldCamMat_loc, 1, GL_FALSE, glm::value_ptr(this->cam.worldToCamMat()));
    glUniform1i(maxSteps_loc, MAXSTEPS); // TODO: max steps should be adjustable at runtime
    glUniform1f(minDist_loc, MINDIST); // TODO: min step distance should be adjustable at runtime
}

void Engine::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        updateUniforms();
        this->cam.stepTime();
        glUseProgram(this->shaderProgram);
        glBindVertexArray(this->VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        // glFinish();
        glfwPollEvents();
        if (readMouse) updateRot();
    }
}

void Engine::cleanup() {
    glfwDestroyWindow(this->window);
    glfwTerminate();
}

void Engine::updateRot() {
    double xpos, ypos;
    glfwGetCursorPos(this->window, &xpos, &ypos);
    glfwSetCursorPos(this->window, WIDTH/2, HEIGHT/2);

    float delta_x, delta_y;
    delta_x = xpos - WIDTH/2;
    delta_y = ypos - HEIGHT/2;

    this->cam.rotRight(delta_x/100);
    this->cam.rotUp(delta_y/100);
}

void Engine::key_callback(GLFWwindow *keyWindow, int key, int scancode, int action, int mods) {
    float speed = 0.02;
    Engine *engineP = &App::getInstance().engine;
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE||
            key == GLFW_KEY_Q) {
            glfwSetWindowShouldClose(keyWindow, GLFW_TRUE);
        } else if (key == GLFW_KEY_R) {
            engineP->initPipeline();
        } else if (key == GLFW_KEY_W) {
            engineP->cam.setZVel(-speed);
        } else if (key == GLFW_KEY_A) {
            engineP->cam.setXVel(-speed);
        } else if (key == GLFW_KEY_S) {
            engineP->cam.setZVel(speed);
        } else if (key == GLFW_KEY_D) {
            engineP->cam.setXVel(speed);
        } else if (key == GLFW_KEY_LEFT_CONTROL) {
            engineP->cam.setYVel(-speed);
        } else if (key == GLFW_KEY_SPACE) {
            engineP->cam.setYVel(speed);
        } else if (key == GLFW_KEY_E) {
            engineP->readMouse = !engineP->readMouse;
        }
    } else if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_W || key == GLFW_KEY_S) {
            engineP->cam.setZVel(0);
        } else if (key == GLFW_KEY_A || key == GLFW_KEY_D) {
            engineP->cam.setXVel(0);
        } else if (key == GLFW_KEY_SPACE || key == GLFW_KEY_LEFT_CONTROL) {
            engineP->cam.setYVel(0);
        }
    }
}

void Engine::error_callback(int error, const char *description) {
    fprintf(stderr, "Error: %s\n", description);
}