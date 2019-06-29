#include "Fractal.hpp"
#include <iostream>
#include <fstream>
#include <string>

Fractal::Fractal() {
    this->name = "Mandelbrot";
    this->vertShader = this->compileFragShaderFromSrc("../shaders/triangle.vert", GL_VERTEX_SHADER);
    // this->fragShader = this->compileFragShaderFromSrc("../shaders/mandelbrot.frag", GL_FRAGMENT_SHADER);
    // this->fragShader = this->compileFragShaderFromSrc("../shaders/sphere.frag", GL_FRAGMENT_SHADER);
    this->fragShader = this->compileFragShaderFromSrc("../shaders/sierpinskiTri.frag", GL_FRAGMENT_SHADER);
}

unsigned int Fractal::compileFragShaderFromSrc(const char *fname, GLenum shaderType) {
    std::fstream shaderSrcFile;
    shaderSrcFile.open(fname, std::fstream::in | std::ifstream::ate);// TODO make the file names not be hard coded
    if (!shaderSrcFile.is_open()) {
        throw std::runtime_error("Failed to open shader source file");
    }

    int size = shaderSrcFile.tellg();
    shaderSrcFile.seekg(0, shaderSrcFile.beg);
    std::string shaderSrc(size, ' ');
    shaderSrcFile.read(&shaderSrc[0], size);

    const GLchar *srcCStr = (const GLchar *)shaderSrc.c_str();

    unsigned int shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &srcCStr, NULL);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        int logSize;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
        char *log = new char[logSize];
        glGetShaderInfoLog(shader, logSize, NULL, log);
        std::cerr << "Failed to compile shader." << log;
        throw;
    }

    shaderSrcFile.close();
    // delete[] shaderSrc;

    return shader;
}