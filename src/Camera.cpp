#include "Camera.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>

Camera::Camera() {
    this->fov = 90;
    // initialize R to identity matrix (i.e. world space is parallel to camera space)
    this->R = glm::mat4(1.0f); 
    // initialize T such that the camera is at  (x, y, z) = (0, -2, 0)
    this->T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 2.0, 0.0));
    std::cout << glm::to_string(this->T) << std::endl;
    this->vel = glm::vec4(0);
}

glm::mat4 Camera::camToWorldMat() {
    return glm::inverse(worldToCamMat());
}

glm::mat4 Camera::worldToCamMat() {
    // return this->R * this->T;
    return this->T * this->R;
}


glm::vec4 Camera::getCamRight() {
    return this->R * glm::vec4(1, 0, 0, 0);
}

glm::vec4 Camera::getCamForward() {
    return this->R * glm::vec4(0, 1, 0, 0);
}

glm::vec4 Camera::getCamUp() {
    return this->R * glm::vec4(0, 0, 1, 0);
}


void Camera::rotRight(float degrees) {
    this->R = glm::rotate(degrees, glm::vec3(getCamUp())) * this->R;
}

void Camera::rotUp(float degrees) {
    this->R = glm::rotate(degrees, glm::vec3(getCamRight())) * this->R;
}

void Camera::setXVel(float newXVel) {
    // translate world space velocity to camera space, set the x value, then translate
    // back to world space and set it.
    glm::vec4 camVel = worldToCamMat() * this->vel;
    camVel.x = newXVel;
    this->vel = camToWorldMat() * camVel;
}

void Camera::setYVel(float newYVel) {
    glm::vec4 camVel = worldToCamMat() * this->vel;
    camVel.y = newYVel;
    this->vel = camToWorldMat() * camVel;
}

void Camera::setZVel(float newZVel) {
    glm::vec4 camVel = worldToCamMat() * this->vel;
    camVel.z = newZVel;
    this->vel = camToWorldMat() * camVel;
}

void Camera::stepTime() {
    this->T = glm::translate(this->T, glm::vec3(this->vel));
}