#include "Camera.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace glm;

Camera::Camera() {
    this->fov = 90;
    // initialize R to identity matrix (i.e. world space is parallel to camera space)
    this->R = mat4(1.0f);
    this->T = translate(mat4(1.0f), vec3(0.0, -7., -8.0));
    rotUp(45);
    // this->R = lookAt(vec3(-getCamPos()), vec3(0), vec3(0, 1, 0));
    this->vel = vec4(0);
}

vec4 Camera::getCamPos() {
    return camToWorldMat() * vec4(0, 0, 0, 1);
}

mat4 Camera::camToWorldMat() {
    return inverse(worldToCamMat());
}

mat4 Camera::worldToCamMat() {
    return this->R * this->T;
}

vec4 Camera::getCamRight() {
    return normalize(camToWorldMat() * vec4(1, 0, 0, 0));
}

vec4 Camera::getCamUp() {
    return normalize(camToWorldMat() * vec4(0, 1, 0, 0));
}

vec4 Camera::getCamForward() {
    return normalize(camToWorldMat() * vec4(0, 0, -1, 0));
}

/*
If you do the existing rotation, then the new rotation, then the new rotation is happening
in camera space. If you do the new rotation *and then* do the existing rotation, then the
new rotation is happening in *world space*. So flipping the sequence of the matrix
multiplication here changes if you are rotating along axes in world space or in camera space.

... fuck ...
*/
void Camera::rotRight(float degrees) { // L/R rotation should be along Y in world space
    this->R = this->R * rotate(radians(degrees), vec3(0, 1, 0));
    if (degrees) {
        vec3 camForward = getCamForward();
        printf("CamForward: (%.2f, %.2f, %.2f)\n", camForward.x, camForward.y, camForward.z);
    }
}

void Camera::rotUp(float degrees) { // U/D rotation should be along X in camera space
    this->R = rotate(radians(degrees), vec3(1, 0, 0)) * this->R;
    if (degrees) {
        vec3 camForward = getCamForward();
        printf("CamForward: (%.2f, %.2f, %.2f)\n", camForward.x, camForward.y, camForward.z);
    }
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
    vec4 wSpcVel = camToWorldMat() * vel;
    this->T = translate(this->T, -vec3(wSpcVel));

    if (length(wSpcVel)) {
        vec3 camPos = getCamPos();
        printf("CamPos: (%.2f, %.2f, %.2f)\n", camPos.x, camPos.y, camPos.z);
    }
    dvec2 vel_2d = dvec2(vel.x, -vel.z);
    if (length(vel_2d) > std::numeric_limits<double>::epsilon()) {
        this->loc2d += glm::normalize(glm::dvec2(vel.x, -vel.z)) * max(abs((double)vel.x), abs((double)vel.z));
    }
    //this->loc2d.x += vel.x;
    //this->loc2d.y -= vel.z; 
}
