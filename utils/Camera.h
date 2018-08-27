#pragma once

#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

class Camera {
public:
    glm::fvec3 eye, center, up; float fovy;
    int width, height;

    Camera(glm::fvec3 eye_, glm::fvec3 center_, glm::fvec3 up_, float fovy_, int width_, int height_) :
        eye(eye_), center(center_), up(up_), fovy(fovy_),
        width(width_), height(height_)
    {}

    glm::fmat4 getProjMat(void) {
        float dist = getDistance();
        return glm::perspective(fovy, (float) width / height, 5e-2f * dist, 2e2f * dist)
            * glm::lookAt(eye, center, up);
    }
    glm::fvec3 getEye() { return eye; }

    float getDistance() { return glm::length(eye - center); }

    void setResolution(int w, int h) {
        width = w; height = h;
    }
    void rotate(float r, float u, bool lockUp = true) {
        glm::fvec3 dr = eye - center;
        glm::fvec3 axis = glm::cross(glm::normalize(up), glm::normalize(dr));
        const double c = glm::dot(glm::normalize(up), glm::normalize(dr));
        const double s = sqrt(1 - c * c);
        dr = glm::rotate(dr, -r, up);
        if (c < 0 && u > s) u = s - 5e-2;
        if (c >= 0 && u < -s) u = -s + 5e-2;
        dr = glm::rotate(dr, u, axis);
        center = eye - dr;
        if (!lockUp)
            up = glm::normalize(glm::cross(dr, axis));
    }

    void translate(glm::fvec3 d) { eye += d; center += d; }
    void walk(float forward, float right, float upward) {
        glm::fvec3 f = center - eye;
        f -= dot(f, up) * up;
        f = normalize(f);
        glm::fvec3 r = normalize(cross(f, up));
        translate((f * forward + r * right + up * upward) * getDistance());
    }
};
