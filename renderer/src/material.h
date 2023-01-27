#ifndef MATERIAL_H
#define MATERIAL_H

#include "usings.h"

#include "intersection.h"

class Material {
public:
    virtual ~Material() = default;

    Vec3f albedo{};
    Vec3f debug{randf(), randf(), randf()};
};

class Lambertian : public Material {
public:
    explicit Lambertian(const Vec3f &albedo) {
        this->albedo = albedo;
    };
};

class Emitter {
public:
    explicit Emitter(const Vec3f &radiance) : radiance(radiance) {};
    Vec3f radiance;
};

#endif
