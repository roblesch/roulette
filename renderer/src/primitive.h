#pragma once

#include "usings.h"
#include "scene.h"

class Ray {
public:
    Ray(vec3 o, vec3 d) : o(o), d(normalize(d)), t(0) {};
    vec3 at(float dt) const { return o + d * dt; }

    vec3 o;
    vec3 d;
    float t;
};

class Material {
public:
    explicit Material(vec3 rgb) : rgb(rgb) {};
    vec3 rgb;
};

class Diffuse : public Material {
public:
    explicit Diffuse(vec3 rgb) : Material(rgb) {};
};

class Emissive {
public:
    explicit Emissive(vec3 radiance) : radiance(radiance) {};
    vec3 radiance;
};

class Shape {
public:
    explicit Shape(mat4 transform) : to_world(transform), to_obj(inverse(transform)) {};
    mat4 to_world;
    mat4 to_obj;
};

class Rectangle : public Shape {
public:
    explicit Rectangle(mat4 transform) : Shape(transform) {};

    vec3 a{-1, -1, 0};
    vec3 b{1, 1, 0};
    vec3 normal{0, 0, 1};
};

class Cube : public Shape {
public:
    explicit Cube(mat4 transform) : Shape(transform) {};

    vec3 a{-1, -1, -1};
    vec3 b{1, 1, 1};
};

class Primitive {
public:
    Primitive(shared_ptr<Shape> shape,
              shared_ptr<Material> material,
              shared_ptr<Emissive> emitter) :
        shape(std::move(shape)),
        material(std::move(material)),
        emitter(std::move(emitter)) {};

    shared_ptr<Shape> shape;
    shared_ptr<Material> material;
    shared_ptr<Emissive> emitter;
};
