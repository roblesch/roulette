#ifndef MATERIAL_H
#define MATERIAL_H

#include "usings.h"

#include "intersection.h"

class Material {
public:
    virtual ~Material() = default;
    virtual Vec3f eval(const SurfaceScatterEvent& event) const = 0;
    virtual float pdf(const SurfaceScatterEvent& event) const = 0;

    Vec3f albedo{};
    Vec3f debug{randf(), randf(), randf()};
};

class Lambertian : public Material {
public:
    explicit Lambertian(const Vec3f &albedo) {
        this->albedo = albedo;
    };
    Vec3f eval(const SurfaceScatterEvent& event) const override {
        if (event.wi.z() <= 0.0f || event.wo.z() <= 0.0f)
            return Vec3f(0.0f);
        return albedo * INV_PI * event.wo.z();
    }
    float pdf(const SurfaceScatterEvent& event) const override {
        if (event.wi.z() <= 0.0f || event.wo.z() <= 0.0f)
            return 0.0f;
        return cosineHemispherePdf(event.wo);
    }
};

class Emitter {
public:
    explicit Emitter(const Vec3f &radiance) : radiance(radiance) {};
    Vec3f radiance;
};

#endif
