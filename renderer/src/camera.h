#pragma once

#include "usings.h"

class Camera {
public:
    Camera() {};
    Camera(int resx,
        int resy,
        float fovd,
        const mat4& transform) :
        resx(resx),
        resy(resy),
        aspect((float)resy / (float)resx),
        fovd(fovd),
        fovr(d_to_r(fovd)),
        toPlane(0),
        planeArea(0),
        invPlaneArea(0),
        eye(0),
        center(0),
        up(0),
        transform(transform) {};
    Camera(int resx,
        int resy,
        float fovd,
        const vec3& eye,
        const vec3& center,
        const vec3& up) :
        resx(resx),
        resy(resy),
        aspect((float)resy / (float)resx),
        fovd(fovd),
        fovr(d_to_r(fovd)),
        toPlane(1.0f / tan(fovr * 0.5f)),
        planeArea((2.0f / toPlane)* (2.0f * aspect / toPlane)),
        invPlaneArea(1.0f / planeArea),
        eye(eye),
        center(center),
        up(up),
        transform(lookAt(eye, center - eye, up)) {};

    vec3 sampleDirection(pixel px) {
        return transformVec(transform, vec3(
            -1.0f + 2.0f * (float(px.x) / float(resx)),
            aspect - 2.0f * aspect * (float(px.y) / float(resy)),
            toPlane));
    }

    static mat4 lookAt(const vec3& pos, const vec3& fwd, const vec3& up) {
        vec3 f = glm::normalize(fwd);
        vec3 r = glm::normalize(glm::cross(f, up));
        vec3 u = glm::normalize(glm::cross(r, f));
        return mat4(
            r.x, r.y, r.z, 0.f,
            u.x, u.y, u.z, 0.f,
            f.x, f.y, f.z, 0.f,
            pos.x, pos.y, pos.z, 1.f
        );
    }

    int resx;
    int resy;
    float aspect;
    float fovd;
    float fovr;
    float toPlane;
    float planeArea;
    float invPlaneArea;
    vec3 eye;
    vec3 center;
    vec3 up;
    mat4 transform;
};