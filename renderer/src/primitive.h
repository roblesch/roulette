#pragma once

#include "usings.h"
#include "scene.h"

class Ray {
public:
    Ray(const vec3 &o, const vec3 &d) :
        o(o),
        d(normalize(d)),
        t(0) {};
    vec3 at(float dt) const { return o + d * dt; }

    vec3 o;
    vec3 d;
    float t;
};

class Material {
public:
    virtual ~Material() {};
    vec3 albedo;
};

class Lambertian : public Material {
public:
    explicit Lambertian(const vec3 &albedo) {
        this->albedo = albedo;
    };
};

class Emitter {
public:
    explicit Emitter(const vec3 &radiance) : radiance(radiance) {};
    vec3 radiance;
};

class Shape {
public:
    explicit Shape(const mat4 &to_world) :
        pos(vec3()),
        scale(vec3()),
        rot3(vec3()),
        rot4(mat4()),
        invRot(mat4()),
        to_world(to_world),
        to_obj(inverse(to_world)) {};
    Shape(const vec3 &pos,
          const vec3 &scale,
          const vec3 &rot3) :
        pos(pos),
        scale(scale),
        rot3(rot3) {
        
        mat4 T(1), R(1);
        T = glm::translate(T, pos);
        T = glm::scale(T, scale);
        R = glm::rotate(R, rot3.x, vec3(1, 0, 0));
        R = glm::rotate(R, rot3.y, vec3(0, 1, 0));
        R = glm::rotate(R, rot3.z, vec3(0, 0, 1));
        T = T * R;

        rot4 = R;
        invRot = glm::inverse(rot4);
        to_world = T;
        to_obj = glm::inverse(T);
    };

    vec3 pos;
    vec3 scale;
    vec3 rot3;
    mat4 rot4;
    mat4 invRot;
    mat4 to_world;
    mat4 to_obj;
};

class Rectangle : public Shape {
public:
    explicit Rectangle(const mat4 &transform) : Shape(transform) {};
    Rectangle(const vec3 &pos,
              const vec3 &scale,
              const vec3 &rot3) :
        Shape(pos, scale, rot3) {};

    vec3 a{-1, -1, 0};
    vec3 b{1, 1, 0};
    vec3 normal{0, 0, 1};
};

class Cube : public Shape {
public:
    explicit Cube(const mat4 &transform) : Shape(transform) {};
    Cube(const vec3 &pos,
         const vec3 &scale,
         const vec3 &rot3) :
        Shape(pos, scale, rot3) {};

    vec3 a{-1, -1, -1};
    vec3 b{1, 1, 1};
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
