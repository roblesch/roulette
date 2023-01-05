#pragma once

#include "usings.h"

#include "camera.h"
#include "integrator.h"
#include "primitive.h"

class Integrator;

class Scene {
public:
    Scene() {};
    Scene(Camera cam,
          unique_ptr<Integrator> &integ,
          map<string, shared_ptr<Material>> mats,
          map<string, shared_ptr<Primitive>> prims,
          map<string, shared_ptr<Primitive>> lights) :
        camera(cam),
        integrator(std::move(integ)),
        materials(std::move(mats)),
        primitives(std::move(prims)),
        lights(std::move(lights)) {};

    static Scene FromMitsubaXML(const char *filename);
    static Scene FromTungstenJSON(const char* filename);

    Camera camera;
    unique_ptr<Integrator> integrator;
    map<string, shared_ptr<Material>> materials;
    map<string, shared_ptr<Primitive>> primitives;
    map<string, shared_ptr<Primitive>> lights;
};
