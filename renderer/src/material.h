#pragma once

#include "usings.h"

class Material {
public:
    virtual ~Material() = default;
    vec3f albedo{};
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
