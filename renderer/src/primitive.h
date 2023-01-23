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

    bool emissive() const { return emitter ? true : false; }
    bool intersect(Ray& ray, IntersectionData& intersection);
    void setIntersectionData(IntersectionData& intersection) {
        shape->setIntersectionData(intersection);
    }
    bool sampleDirect(const Vec3f& p, LightSample& sample) const {
        return shape->sampleDirect(p, sample);
    }
    Vec3f evalDirect(const IntersectionData& data) const {
        if (!emissive())
            return Vec3f(0.0f);
        //if (hitBackside) TODO: hitbackside?
        return emitter->radiance;
    }
    float directPdf(const IntersectionData& data, const Vec3f& p) const {
        return shape->directPdf(data, p);
    }

    shared_ptr<Shape> shape;
    shared_ptr<Material> material;
    shared_ptr<Emitter> emitter;
};

#endif
