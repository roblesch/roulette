#pragma once

#include <cmath>

#include "usings.h"

class Camera {
public:
    Camera() = default;

    Camera(int resx,
           int resy,
           float fovd,
           const mat4f &transform) :
            resx(resx),
            resy(resy),
            aspect((float) resy / (float) resx),
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
           const vec3f &eye,
           const vec3f &center,
           const vec3f &up) :
            resx(resx),
            resy(resy),
            aspect((float) resy / (float) resx),
            fovd(fovd),
            fovr(d_to_r(fovd)),
            toPlane(1.0f / std::tan(fovr * 0.5f)),
            planeArea((2.0f / toPlane) * (2.0f * aspect / toPlane)),
            invPlaneArea(1.0f / planeArea),
            eye(eye),
            center(center),
            up(up),
            transform(glm::lookAt(eye, center, up)) {};

    [[nodiscard]] vec3f sampleDirection(vec2i px) const {
        vec3f camDir = vec3f(
            -1.0f + 2.0f * (float(px.x) / float(resx)),
            aspect - 2.0f * aspect * (float(px.y) / float(resy)),
            -toPlane);
        vec3f worldDir = transformVec(transform, camDir);
        return glm::normalize(worldDir);
    }

    //static mat4f lookAt(const vec3f &pos, const vec3f &fwd, const vec3f &up) {
    //    vec3f f = glm::normalize(fwd);
    //    vec3f r = glm::normalize(glm::cross(f, up));
    //    vec3f u = glm::normalize(glm::cross(r, f));
    //    return {
    //        r.x, u.x, f.x, pos.x,
    //        r.y, u.y, f.y, pos.y,
    //        r.z, u.z, f.z, pos.z,
    //        0.f, 0.f, 0.f, 1.f
    //    };
    //}

    int resx;
    int resy;
    float aspect;
    float fovd;
    float fovr;
    float toPlane;
    float planeArea;
    float invPlaneArea;
    vec3f eye;
    vec3f center;
    vec3f up;
    mat4f transform;
};
