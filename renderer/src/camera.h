#ifndef CAMERA_H
#define CAMERA_H

#include <cmath>

#include "usings.h"

#include "sampler.h"

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
            pixelSize(1.0f/resx, 1.0f/resy),
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
            pixelSize(1.0f / resx, 1.0f / resy),
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

    Vec2i resolution() const {
        return { resx, resy };
    }

    void samplePosition(PositionSample& sample, PathSampleGenerator &sampler) const {
        sample.p = eye;
        sample.weight = Vec3f(1.0f);
        sample.pdf = 1.0f;
        sample.Ng = transform.fwd();
    }

    [[nodiscard]] Vec3f sampleDirection(Vec2i px, PathSampleGenerator &sampler) const {
        Vec2f uv = sampler.next2D(CameraSample);
        uv -= 0.5f;
        Vec3f camDir = Vec3f(
                //-1.0f + 2.0f * (float(px.x()) / float(resx)),
                //aspect - 2.0f * aspect * (float(px.y()) / float(resy)),
                -1.0f + (float(px.x()) + 0.5f + uv.x())*2.0f*pixelSize.x(),
                aspect - (float(px.y()) + 0.5f + uv.y())*2.0f*pixelSize.y(),
                toPlane).normalized();
        return transform.transformVector(camDir);
    }

    void sampleDirection(Vec2i px, DirectionSample& sample, PathSampleGenerator &sampler) const {
        sample.d = sampleDirection(px, sampler);
        sample.weight = Vec3f(1.0f);
        sample.pdf = 1.0f;
    }

    int resx{};
    int resy{};
    Vec2f pixelSize{};
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
