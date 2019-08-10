#include "Engine.hpp"
#include "App.hpp"
#include <iostream>

// #define GLM_GTX_dual_quaternion
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

float vertices[] = {
    -1.0f, 1.0f, 0.0f,
    3.0f, 1.0f, 0.0f,
    -1.0f, -3.0, 0.0f
};

Engine::Engine(Settings *settings) {
    this->settings = settings;
    this->readMouse = true;
    initWindow();
    initOpengl();
    locateUniforms();
    updateUniforms();
    glfwMakeContextCurrent(NULL); // Remove context from main thread. The context is made current again in run()
}

void Engine::locateUniforms() {
    uniformLocations.lock.lock();
    uniformLocations.screenSize = glGetUniformLocation(shaderProgram, "screenSize");
    uniformLocations.fov = glGetUniformLocation(shaderProgram, "FOV");
    uniformLocations.camWorldMat = glGetUniformLocation(shaderProgram, "camToWorldMat");
    uniformLocations.worldCamMat = glGetUniformLocation(shaderProgram, "worldToCamMat");
    uniformLocations.maxSteps = glGetUniformLocation(shaderProgram, "maxSteps");
    uniformLocations.minDist = glGetUniformLocation(shaderProgram, "minDist");
    uniformLocations.lock.unlock();
}

void Engine::run() {
    glfwMakeContextCurrent(window);
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

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // neccessary for OS X support
#endif

    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE); // maybe change this to disable FPS cap?

    this->window = glfwCreateWindow(this->settings->width, this->settings->height, "Fractal Box", NULL, NULL);
    if (this->window == NULL) throw std::runtime_error("Failed to create window");
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) throw std::runtime_error("Failed to init GLAD");

    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//    glfwSetScrollCallback(window, scroll_callback); // TODO
}

void Engine::initOpengl() {
    glViewport(0, 0, this->settings->width, this->settings->height); // TODO: update these coordinates to make fractal math easier
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
        // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT);

        if (readMouse) updateRot();
        this->cam.stepTime();
        updateUniforms();
        glUseProgram(this->shaderProgram);
        glBindVertexArray(this->VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        // glFinish();
        glfwPollEvents();
    }
}

void Engine::updateUniforms() {
    this->settings->lock.lock();
    this->uniformLocations.lock.lock();
    glUniform1f(uniformLocations.fov, this->cam.fov);
    glUniform2f(uniformLocations.screenSize, this->settings->width, this->settings->height);
    glUniformMatrix4fv(uniformLocations.camWorldMat, 1, GL_FALSE, glm::value_ptr(this->cam.camToWorldMat()));
    glUniformMatrix4fv(uniformLocations.worldCamMat, 1, GL_FALSE, glm::value_ptr(this->cam.worldToCamMat()));
    glUniform1i(uniformLocations.maxSteps, this->settings->maxSteps);
    glUniform1f(uniformLocations.minDist, this->settings->minDist);
    this->uniformLocations.lock.unlock();
    this->settings->lock.unlock();
}

void Engine::cleanup() {
    glfwDestroyWindow(this->window);
    glfwTerminate();
}

void Engine::updateRot() {
    double xpos, ypos;
    glfwGetCursorPos(this->window, &xpos, &ypos);
    glfwSetCursorPos(this->window, this->settings->width/2, this->settings->height/2);

    float delta_x, delta_y;
    delta_x = xpos - this->settings->width/2;
    delta_y = ypos - this->settings->height/2;

    this->cam.rotRight(delta_x * this->settings->mouse_sensitivity);
    this->cam.rotUp(delta_y * this->settings->mouse_sensitivity);
}

void Engine::key_callback(GLFWwindow *keyWindow, int key, int scancode, int action, int mods) {
    float speed = 0.1;
    Engine *engineP = &App::getInstance().engine;
    if (action == GLFW_PRESS) {
        // TODO: In retrospect it would have been better if this had been a switch from the start
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
            if (engineP->readMouse) {
                glfwSetInputMode(engineP->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                glfwSetCursorPos(engineP->window, engineP->settings->width/2, engineP->settings->height/2);
            } else {
                glfwSetInputMode(engineP->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
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
