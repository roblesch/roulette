#ifndef SHAPE_H
#define SHAPE_H

#include "usings.h"

#include "aabb.h"
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

    virtual void setbb(AABB& aabb) const = 0;
    virtual bool intersect(Ray& ray, Intersection& intersection) const = 0;
    virtual void setIntersectionData(Intersection& intersection, IntersectionData& data) const = 0;
    virtual bool sampleDirect(const Vec3f& p, PathSampleGenerator& sampler, LightSample& sample) const = 0;
    virtual float pdf(const Intersection& intersection, const IntersectionData& data, const Vec3f& p) const = 0;

public:
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
            Shape(pos, scale, rot3) {
        base = to_world * Vec3f(0.0f);
        edge0 = to_world.transformVector(Vec3f(1.0f, 0.0f, 0.0f));
        edge1 = to_world.transformVector(Vec3f(0.0f, 0.0f, 1.0f));
        base -= edge0 * 0.5f;
        base -= edge1 * 0.5f;
        Vec3f n = edge1.cross(edge0);
        area = n.length();
        invArea = 1.0f / area;
        n /= area;
        invUvSq = 1.0f / Vec2f(edge0.lengthSq(), edge1.lengthSq());
        normal = n;
        frame = TangentFrame(n, edge0.normalized(), edge1.normalized());
    };

    void setbb(AABB& aabb) const override {
        aabb.grow(base);
        aabb.grow(base + edge0);
        aabb.grow(base + edge1);
        aabb.grow(base + edge0 + edge1);
    }

    bool intersect(Ray& ray, Intersection& intersection) const override;
    void setIntersectionData(Intersection& intersection, IntersectionData& data) const override;

    bool sampleDirect(const Vec3f& p, PathSampleGenerator &sampler, LightSample& sample) const override {
        if (frame.normal.dot(p - base) <= 0.0f)
            return false;

        Vec2f xi = sampler.next2D(EmitterSample);
        Vec3f q = base + xi.x() * edge0 + xi.y() * edge1;
        sample.d = q - p;
        float rSq = sample.d.lengthSq();
        sample.dist = std::sqrt(rSq);
        sample.d /= sample.dist;
        float cosTheta = -frame.normal.dot(sample.d);
        sample.pdf = rSq / (cosTheta * area);

        return true;
    }

    float pdf(const Intersection& intersection, const IntersectionData& data, const Vec3f& p) const override {
        float cosTheta = std::abs(frame.normal.dot(data.w));
        float t = frame.normal.dot(base - p) / frame.normal.dot(data.w);
        return t * t / (cosTheta * area);
    }

public:
    Vec3f base;
    Vec3f edge0, edge1;
    //TangentFrame _frame;
    Vec2f invUvSq;
    float area;
    float invArea;
    Vec3f normal;
    TangentFrame frame;
};

class Cube : public Shape {
public:
    explicit Cube(const Mat4f &transform) : Shape(transform) {};

    Cube(const Vec3f &pos,
         const Vec3f &scale,
         const Vec3f &rot3) :
            Shape(pos, scale*0.5f, rot3) {
        Vec3f faceCdf = 4.0f * Vec3f(
            scale.y() * scale.z(),
            scale.z() * scale.x(),
            scale.x() * scale.y()
        );
        area = 2.0f * faceCdf.z();
    };

    void setbb(AABB& aabb) const override {
        for (int i = 0; i < 8; i++) {
            aabb.grow(pos + rot4 * Vec3f(
                (i & 1 ? scale.x() : -scale.x()),
                (i & 2 ? scale.y() : -scale.y()),
                (i & 4 ? scale.z() : -scale.z())
            ));
        }
    }

    bool intersect(Ray& ray, Intersection& intersection) const override;
    void setIntersectionData(Intersection &intersection, IntersectionData &data) const override;

    bool sampleDirect(const Vec3f& p, PathSampleGenerator& sampler, LightSample& sample) const override {
        return false;
    }

    float pdf(const Intersection& intersection, const IntersectionData& data, const Vec3f& p) const override {
        return (p - data.p).lengthSq() / (-data.w.dot(data.Ng) * area);
    }

public:
    float area;
};

#endif
