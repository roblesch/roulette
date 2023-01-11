#ifndef CAMERA_H
#define CAMERA_H

#include <cmath>

#include "usings.h"

class Camera {
public:
    Camera() = default;

    Camera(int resx,
           int resy,
           float fovd,
           const Mat4f &transform) :
            resx(resx),
            resy(resy),
            aspect((float) resy / (float) resx),
            fovd(fovd),
            fovr(d_to_r(fovd)),
            toPlane(0),
            planeArea(0),
            invPlaneArea(0),
            eye(0.0f, 0.0f, 0.0f),
            center(0.0f, 0.0f, -1.0f),
            up(0.0f, 1.0f, 0.0f),
            transform(transform) {};

    Camera(int resx,
           int resy,
           float fovd,
           const Vec3f &eye,
           const Vec3f &center,
           const Vec3f &up) :
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
            transform(Mat4f::lookAt(eye, center - eye, up)) {};

    [[nodiscard]] Vec3f sampleDirection(Vec2i px) const {
        Vec3f camDir = Vec3f(
                -1.0f + 2.0f * (float(px.x()) / float(resx)),
                aspect - 2.0f * aspect * (float(px.y()) / float(resy)),
                -toPlane);
        Vec3f worldDir = transform * camDir;
        return worldDir.normalized();
    }

    int resx{};
    int resy{};
    float aspect{};
    float fovd{};
    float fovr{};
    float toPlane{};
    float planeArea{};
    float invPlaneArea{};
    Vec3f eye{};
    Vec3f center{};
    Vec3f up{};
    Mat4f transform;
};

#endif
