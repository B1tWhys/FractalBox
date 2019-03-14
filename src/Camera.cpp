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
    this->T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -2.0));
    this->vel = glm::vec4(0);
}

glm::mat4 Camera::camToWorldMat() {
    return glm::inverse(worldToCamMat());
}

glm::mat4 Camera::worldToCamMat() {
    return this->R * this->T;
    // return this->T * this->R;
}


glm::vec4 Camera::getCamRight() {
    return camToWorldMat() * glm::vec4(1, 0, 0, 0);
}

glm::vec4 Camera::getCamForward() {
    return camToWorldMat() * glm::vec4(0, 0, -1, 0);
}

glm::vec4 Camera::getCamUp() {
    return camToWorldMat() * glm::vec4(0, 1, 0, 0);
}

/*
If you do the existing rotation, then the new rotation, then the new rotation is happening
in camera space. If you do the new rotation *and then* do the existing rotation, then the
new rotation is happening in *world space*. So flipping the order of the matrix
multiplication here changes if you are rotating along axis in world space or in camera space.

... fuck ...
*/
void Camera::rotRight(float degrees) { // L/R rotation should be along Y in world space
    this->R = this->R * glm::rotate(glm::radians(degrees), glm::vec3(0, 1, 0)) ;
}

void Camera::rotUp(float degrees) { // U/D rotation should be along X in camera space
    this->R = glm::rotate(glm::radians(degrees), glm::vec3(1, 0, 0)) * this->R;
}

void Camera::setXVel(float newXVel) {
    // translate world space velocity to camera space, set the x value, then translate
    // back to world space and set it.
    // glm::vec4 camVel = worldToCamMat() * this->vel;
    // camVel.x = newXVel;
    // this->vel = camToWorldMat() * camVel;
    this->vel.x = newXVel;
}

void Camera::setYVel(float newYVel) {
    // glm::vec4 camVel = worldToCamMat() * this->vel;
    // camVel.y = newYVel;
    // this->vel = camToWorldMat() * camVel;
    this->vel.y = newYVel;
}

void Camera::setZVel(float newZVel) {
    // glm::vec4 camVel = worldToCamMat() * this->vel;
    // camVel.z = newZVel;
    // this->vel = camToWorldMat() * camVel;
    this->vel.z = newZVel;
}

void Camera::stepTime() {
    glm::vec4 wSpcVel = camToWorldMat() * this->vel;
    this->T = glm::translate(this->T, -glm::vec3(wSpcVel));
}