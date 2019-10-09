#ifndef __APPLE__
#include <glad/glad.h> // macOS does not need a function loader
#endif

#include <GLFW/glfw3.h>
#include "Fractal.hpp"
#include "Camera.hpp"
#include "Settings.hpp"
#include <mutex>
#include <glm/glm.hpp>

#pragma once

/*
The Engine class does all the rendering related stuff, as well as responding to key presses.
When it is initialized by the App singleton, it gets everything ready but doesn't actually
enter the main game loop. Rather, the game loop is run in a seperate thread which is managed
by the App singleton.
*/

struct UniformLocations {
    std::mutex lock;
    int screenSize;
    int fov;
    int camWorldMat;
    int worldCamMat;
    int maxSteps;
    int minDist;
    int hlUniform;
    int kiUniform;
    int lrUniform; // left/right arrows
    int udUniform; // up/down arrows
    int commaPeriodUniform;
    int scrollUniform;
    int loc2d;
};

typedef struct {
    float hlUniform;
    float kiUniform;
    float lrUniform; // left/right arrows
    float udUniform; // up/down arrows
    float commaPeriodUniform;
    glm::vec2 scrollUniform;
} WildcardUniforms;

class Engine {
    public:
        Settings *settings;
        Fractal *fractal;
        Engine(Settings *);
        WildcardUniforms wildcardUniforms;
        WildcardUniforms wildcardUniformsVelocities;

        void run();
    private:
        GLFWwindow *window;
        unsigned int shaderProgram;
        unsigned int VBO; // Vertex buffer object
        unsigned int VAO; // Vertex array object
        Camera cam;
        bool readMouse;
        struct UniformLocations uniformLocations;

        void initWindow();
        void initOpengl();
        void initBuffers();
        void initPipeline();
        void locateUniforms();
        void cleanup();
        
        void mainLoop();
        void updateUniforms();
        void updateRot();
        
        static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
        static void error_callback(int error, const char *description);
        static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
};
