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
    bool intersect(Ray& ray, Intersection& intersection) const {
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
    bool sampleLightDirect(const Vec3f& p, LightSample& sample) const {
        return shape->sampleDirect(p, sample);
    }
    Vec3f evalBsdf(const SurfaceScatterEvent& event) const {
        return material->eval(event);
    }
    float bsdfPdf(const SurfaceScatterEvent& event) const {
        return material->pdf(event);
    }
    bool sampleBsdf(SurfaceScatterEvent& event) const {
        return material->sample(event);
    }
    Vec3f evalEmissionDirect(const Intersection& intersection, const IntersectionData& data) const {
        if (!emissive())
            return Vec3f(0.0f);
        return emitter->radiance;
    }
    float shapePdf(const Intersection& intersection, const IntersectionData& data, const Vec3f& p) const {
        return shape->pdf(intersection, data, p);
    }

    shared_ptr<Shape> shape;
    shared_ptr<Material> material;
    shared_ptr<Emitter> emitter;
};

#endif
