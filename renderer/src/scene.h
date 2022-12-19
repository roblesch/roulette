#pragma once

#include "usings.h"
#include "primitive.h"

class Camera {
public:
    Camera(int resx, int resy, float fovx, mat4 transform) {};

    int resx{};
    int resy{};
    float fovx{};
    float aspect{};
    vec3 eye{};
    vec3 center{};
    vec3 up{};
    mat4 to_world{};
};

class Scene {
public:
    Scene() = default;

    Scene(shared_ptr<Camera> cam,
          map<string, shared_ptr<Material>> mats,
          map<string, shared_ptr<Primitive>> prims) :
        camera(std::move(cam)),
        materials(std::move(mats)),
        primitives(std::move(prims)) {};

    static Scene FromMitsubaXML(const char *filename);
    static Scene FromTungstenJSON(const char* filename);

    shared_ptr<Camera> camera;
    map<string, shared_ptr<Material>> materials;
    map<string, shared_ptr<Primitive>> primitives;
};
