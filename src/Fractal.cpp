#include "Fractal.hpp"
#include <fstream>

Fractal::Fractal() {
    this->name = "Mandelbrot";
    this->vertShader = this->compileFragShaderFromSrc("../shaders/triangle.vert", GL_VERTEX_SHADER);
    this->fragShader = this->compileFragShaderFromSrc("../shaders/mandelbrot.frag", GL_FRAGMENT_SHADER);
}

unsigned int Fractal::compileFragShaderFromSrc(const char *fname, GLenum shaderType) {
    std::ifstream shaderSrcFile;// TODO fix the shit out of this
    std::string name = (shaderType ==  GL_VERTEX_SHADER) ? "../shaders/triangle.vert" : "../shaders/mandelbrot.frag";

    shaderSrcFile.open(name, std::ifstream::ate); 

    size_t size = shaderSrcFile.tellg();
    shaderSrcFile.seekg(0, shaderSrcFile.beg);
    GLchar *shaderSrc = static_cast<GLchar *>(malloc(size));
    shaderSrcFile.read(shaderSrc, size);
    shaderSrcFile.close();

    unsigned int shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSrc, NULL);
    glCompileShader(shader);
    delete[] shaderSrc;

    return shader;

    
    return -1;
}