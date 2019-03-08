#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Engine {
    public:
        Engine();

        void run();
    private:
        GLFWwindow *window;
};