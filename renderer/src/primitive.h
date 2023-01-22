#ifndef PRIMITIVE_H
#define PRIMITIVE_H

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

    bool intersect(Ray& ray, IntersectionData& intersection);
    void setIntersectionData(Ray& ray, IntersectionData& intersection) {
        shape->setIntersectionData(ray, intersection);
    }

    shared_ptr<Shape> shape;
    shared_ptr<Material> material;
    shared_ptr<Emitter> emitter;
};

#endif
