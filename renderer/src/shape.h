#ifndef SHAPE_H
#define SHAPE_H

#include "usings.h"

#include "ray.h"
#include "intersection.h"

class Shape {
public:
    explicit Shape(const Mat4f &to_world) :
            pos(Vec3f(0.0f, 0.0f, 0.0f)),
            scale(Vec3f(1.0f, 1.0f, 1.0f)),
            rot3(Vec3f(0.0f, 0.0f, 0.0f)),
            rot4(Mat4f()),
            invRot(Mat4f()),
            to_world(to_world),
            to_obj(to_world.invert()) {};

    Shape(const Vec3f &pos,
          const Vec3f &scale,
          const Vec3f &rot3) :
            pos(pos),
            scale(scale),
            rot3(rot3) {

        Vec3f x(1.0f, 0.0f, 0.0f);
        Vec3f y(0.0f, 1.0f, 0.0f);
        Vec3f z(0.0f, 0.0f, 1.0f);

        x *= scale.x();
        y *= scale.y();
        z *= scale.z();
        Mat4f tform = Mat4f::rotYXZ(rot3);
        x = tform * x;
        y = tform * y;
        z = tform * z;

        rot4 = tform;
        invRot = tform.invert();
        to_world = Mat4f(
            x[0], y[0], z[0], pos[0],
            x[1], y[1], z[1], pos[1],
            x[2], y[2], z[2], pos[2],
            0.0f, 0.0f, 0.0f, 1.0f
        );
        to_obj = to_world.invert();
    };

    virtual bool intersect(const Ray& ray) = 0;

    Vec3f pos;
    Vec3f scale;
    Vec3f rot3;
    Mat4f rot4{};
    Mat4f invRot{};
    Mat4f to_world{};
    Mat4f to_obj{};
};

class Rectangle : public Shape {
public:
    explicit Rectangle(const Mat4f &transform) : Shape(transform) {};

    Rectangle(const Vec3f &pos,
              const Vec3f &scale,
              const Vec3f &rot3) :
            Shape(pos, scale, rot3) {};

    bool intersect(const Ray& ray) override;

    Vec3f a{-1, -1, 0};
    Vec3f b{1, 1, 0};
    Vec3f normal{0, 0, 1};
};

class Cube : public Shape {
public:
    explicit Cube(const Mat4f &transform) : Shape(transform) {};

    Cube(const Vec3f &pos,
         const Vec3f &scale,
         const Vec3f &rot3) :
            Shape(pos, scale*0.5f, rot3) {};

    bool intersect(const Ray& ray) override;

    Vec3f a{-1, -1, -1};
    Vec3f b{1, 1, 1};
};

#endif
