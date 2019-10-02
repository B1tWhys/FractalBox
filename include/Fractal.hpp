#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

#pragma once

/*
A fractal object encapsulates all of the information specific to rendering one fractal as opposed to another. Mostly
this consists of loading and compiling the vertex & fragment shaders.

All ray marching is done in the fragment shader, and it's mostly what changes between one fractal and another.
*/

class Fractal {
    public:
        Fractal();
        std::string name;
        unsigned int vertShader;
        unsigned int fragShader;
    private:
        unsigned int compileFragShaderFromSrc(const char *fname, GLenum shaderType);
        std::string *includeFiles(std::string deShader);
};
