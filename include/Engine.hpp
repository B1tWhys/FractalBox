#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Fractal.hpp"
#include "Camera.hpp"

#pragma once

/*
The Engine class does all the rendering related stuff, as well as responding to key presses.
When it is initialized by the App singleton, it gets everything ready but doesn't actually
enter the main game loop. Rather, the game loop is run in a seperate thread which is managed
by the App singleton.
*/

class Engine {
    public:

        Fractal *fractal;

        Engine();
        void run();
    private:
        GLFWwindow *window;
        unsigned int shaderProgram;
        unsigned int VBO; // Vertex buffer object
        unsigned int VAO; // Vertex array object
        Camera cam;
        bool readMouse;

        void initWindow();
        void initOpengl();
        void initBuffers();
        void initPipeline();
        void cleanup();
        
        void mainLoop();
        void updateUniforms();
        void updateRot();
        
        static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
        static void error_callback(int error, const char *description);
};