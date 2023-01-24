#ifndef CAMERA_H
#define CAMERA_H

#include <cmath>

#include "usings.h"

struct PositionSample {
    Vec3f p;
    Vec3f weight;
    float pdf;
    Vec3f Ng;
};

struct DirectionSample {
    Vec3f d;
    Vec3f weight;
    float pdf;
};

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

    void samplePosition(PositionSample& sample) const {
        sample.p = eye;
        sample.weight = Vec3f(1.0f);
        sample.pdf = 1.0f;
        sample.Ng = transform.fwd();
    }

    [[nodiscard]] Vec3f sampleDirection(Vec2i px) const {
        Vec3f camDir = Vec3f(
                -1.0f + 2.0f * (float(px.x()) / float(resx)),
                aspect - 2.0f * aspect * (float(px.y()) / float(resy)),
                toPlane).normalized();
        return transform.transformVector(camDir);
    }

    void sampleDirection(Vec2i px, DirectionSample& sample) const {
        sample.d = sampleDirection(px);
        sample.weight = Vec3f(1.0f);
        sample.pdf = 1.0f;
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
