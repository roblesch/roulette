#pragma once

#include "usings.h"

#include "camera.h"
#include "material.h"
#include "primitive.h"

class Scene {
public:
    Scene() = default;

    Scene(Camera cam,
          map<string, shared_ptr<Material>> mats,
          map<string, shared_ptr<Primitive>> prims,
          map<string, shared_ptr<Primitive>> lights) :
            camera(cam),
            materials(std::move(mats)),
            primitives(std::move(prims)),
            lights(std::move(lights)) {};

    Camera camera{};
    map<string, shared_ptr<Material>> materials;
    map<string, shared_ptr<Primitive>> primitives;
    map<string, shared_ptr<Primitive>> lights;
};
