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

    const Primitive* get() const {
        return this;
    }
    bool emissive() const { return emitter ? true : false; }
    bool intersect(Ray& ray, Intersection& intersection) {
        if (shape->intersect(ray, intersection)) {
            intersection.primitive = this;
            return true;
        }
        return false;
    }
    void setIntersectionData(Intersection &intersection, IntersectionData &data) const {
        shape->setIntersectionData(intersection, data);
        data.primitive = this;
    }

    shared_ptr<Shape> shape;
    shared_ptr<Material> material;
    shared_ptr<Emitter> emitter;
};

#endif
