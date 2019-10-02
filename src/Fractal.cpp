#include "Fractal.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <regex>

Fractal::Fractal() {
    this->name = "Mandelbrot";
    this->vertShader = this->compileFragShaderFromSrc("../shaders/triangle.vert", GL_VERTEX_SHADER);
    this->fragShader = this->compileFragShaderFromSrc("../shaders/sierpinskiTri.de", GL_FRAGMENT_SHADER);
}

std::string readFile(const char *fName) {
    std::fstream file;
    file.open(fName, std::fstream::in | std::ifstream::ate);
    if (!file.is_open()) {
        char *err = (char *)malloc(100 + strlen(fName));
        strcpy(err, "Failed to open file: ");
        strcat(err, fName);
        throw std::runtime_error(err);
    }
    int size = file.tellg();
    file.seekg(0, file.beg);
    std::string ret(size, ' ');
    file.read(&ret[0], size);
    file.close();
    return ret;
}

std::string *Fractal::includeFiles(std::string deShader) {
    std::regex e("#include \"(.+)\"");
    std::regex_iterator<std::string::iterator> rit(deShader.begin(), deShader.end(), e);
    std::regex_iterator<std::string::iterator> rend;
    if (rit == rend) {
        return new std::string(deShader);
    }
    
    std::string *ret = new std::string();
    int srcPos = 0;
    for (;rit != rend; rit++) {
        std::string fNameStr = rit->str(1);
        const char *incFName = fNameStr.c_str();
        std::string newSrc = readFile(incFName);
        ret->append(deShader, srcPos, rit->position() - srcPos);
        if (srcPos) ret->append("\n");
        ret->append("//// ");
        ret->append(incFName);
        ret->append("\n");
        ret->append(newSrc);
        srcPos = rit->position() + rit->length();
    }

    ret->append(deShader, srcPos, deShader.length() - srcPos);

    return ret;
}

unsigned int Fractal::compileFragShaderFromSrc(const char *fname, GLenum shaderType) {
    std::string rawShaderSrc = readFile(fname);
    std::string *srcStr = includeFiles(rawShaderSrc);
    const GLchar *srcCStr = srcStr->c_str();

    unsigned int shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &srcCStr, NULL);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        int logSize;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
        std::cerr << *srcStr<< std::endl;
        char *log = new char[logSize];
        glGetShaderInfoLog(shader, logSize, NULL, log);
        std::cerr << "Failed to compile shader." << log;
        throw;
    }

    delete srcStr;
    return shader;
}

