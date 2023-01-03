#pragma once

#include "usings.h"

class Ray {
public:
    Ray(const vec3f &o, const vec3f &d) :
        o(o),
        d(normalize(d)),
        t(0) {};
    vec3f at(float dt) const { return o + d * dt; }

    vec3f o;
    vec3f d;
    float t;
};

class Material {
public:
    virtual ~Material() {};
    vec3f albedo;
};

class Lambertian : public Material {
public:
    explicit Lambertian(const vec3f &albedo) {
        this->albedo = albedo;
    };
};

class Emitter {
public:
    explicit Emitter(const vec3f &radiance) : radiance(radiance) {};
    vec3f radiance;
};

class Shape {
public:
    explicit Shape(const mat4f &to_world) :
        pos(vec3f()),
        scale(vec3f()),
        rot3(vec3f()),
        rot4(mat4f()),
        invRot(mat4f()),
        to_world(to_world),
        to_obj(inverse(to_world)) {};
    Shape(const vec3f &pos,
          const vec3f &scale,
          const vec3f &rot3) :
        pos(pos),
        scale(scale),
        rot3(rot3) {
        
        mat4f T(1), R(1);
        T = glm::translate(T, pos);
        T = glm::scale(T, scale);
        R = glm::rotate(R, rot3.x, vec3f(1, 0, 0));
        R = glm::rotate(R, rot3.y, vec3f(0, 1, 0));
        R = glm::rotate(R, rot3.z, vec3f(0, 0, 1));
        T = T * R;

        rot4 = R;
        invRot = glm::inverse(rot4);
        to_world = T;
        to_obj = glm::inverse(T);
    };

    vec3f pos;
    vec3f scale;
    vec3f rot3;
    mat4f rot4;
    mat4f invRot;
    mat4f to_world;
    mat4f to_obj;
};

class Rectangle : public Shape {
public:
    explicit Rectangle(const mat4f &transform) : Shape(transform) {};
    Rectangle(const vec3f &pos,
              const vec3f &scale,
              const vec3f &rot3) :
        Shape(pos, scale, rot3) {};

    vec3f a{-1, -1, 0};
    vec3f b{1, 1, 0};
    vec3f normal{0, 0, 1};
};

class Cube : public Shape {
public:
    explicit Cube(const mat4f &transform) : Shape(transform) {};
    Cube(const vec3f &pos,
         const vec3f &scale,
         const vec3f &rot3) :
        Shape(pos, scale, rot3) {};

    vec3f a{-1, -1, -1};
    vec3f b{1, 1, 1};
};

class Primitive {
public:
    Primitive(shared_ptr<Shape> shape,
              shared_ptr<Material> material,
              shared_ptr<Emitter> emitter) :
        shape(std::move(shape)),
        material(std::move(material)),
        emitter(std::move(emitter)) {};

    shared_ptr<Shape> shape;
    shared_ptr<Material> material;
    shared_ptr<Emitter> emitter;
};
