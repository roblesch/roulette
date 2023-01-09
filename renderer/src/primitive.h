#pragma once

#include "usings.h"

#include "intersection.h"
#include "material.h"
#include "ray.h"
#include "shape.h"

class Primitive {
public:
    Primitive(shared_ptr<Shape> shape,
              shared_ptr<Material> material,
              shared_ptr<Emitter> emitter) :
            shape(std::move(shape)),
            material(std::move(material)),
            emitter(std::move(emitter)) {};

    bool intersect(const Ray& ray, IntersectionPrimitive& intersection);

    shared_ptr<Shape> shape;
    shared_ptr<Material> material;
    shared_ptr<Emitter> emitter;
};
