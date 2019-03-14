#include <glm/glm.hpp>

#pragma once
/*
The Camera class holds the location and orientation of the camera,
and also has functions for retriving the orthonormal basis vectors for the camera's
coordinate space with respect to the coords that are used for computing the actual
fractal, as well as a function to update the camera location according to the camera
velocity.

I'm also pretty sure I didn't do this in a way that follows conventions. I kinda made
up the linear algebra as I went so it's a little bit of a struggle. Maybe I'll fix it
eventually
*/

class Camera {
    public:
        Camera();
        float fov;

        glm::vec4 getCamForward();
        glm::vec4 getCamRight();
        glm::vec4 getCamUp();

        // void setXPos(float newX);
        // void setYPos(float newY);
        // void setZPos(float newZ);

        void setXVel(float newXVel); // in camera space
        void setYVel(float newYVel); // in camera space
        void setZVel(float newZVel); // in camera space

        void rotRight(float degrees); // + angle corresponds to moving the mouse to the right
        void rotUp(float degrees); // + corresponds to moving the mouse backward (like... away from ur monitor)

        glm::mat4 camToWorldMat();
        glm::mat4 worldToCamMat();
        void stepTime();
    private:
        // glm::vec4 pos; // in world space
        glm::vec4 vel; // in world space

        // not actually sure that the world space <-> camera space direction is correct below...
        glm::mat4 R; // world space -> camera space rotation matrix
        glm::mat4 T; // world space -> camera space translation matrix

        glm::vec3 camRight;
};