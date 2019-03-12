#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Fractal.hpp"

#ifndef ENGINE
#define ENGINE

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

        void initWindow();
        void initOpengl();
        void initBuffers();
        void initPipeline();
        void cleanup();

        void mainLoop();
        
        static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
        static void error_callback(int error, const char *description);
};

#endif