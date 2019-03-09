#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

#ifndef FRACTAL
#define FRACTAL

class Fractal {
    public:
        Fractal();
        std::string name;
        unsigned int vertShader;
        unsigned int fragShader;
    private:
        unsigned int compileFragShaderFromSrc(const char *fname, GLenum shaderType);
};

#endif