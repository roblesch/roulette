#pragma once

#include "usings.h"

#include "camera.h"
#include "integrator.h"
#include "primitive.h"

class Scene {
public:
    Scene() {};

    Scene(Camera cam,
          Integrator integ,
          map<string, shared_ptr<Material>> mats,
          map<string, shared_ptr<Primitive>> prims,
          map<string, shared_ptr<Primitive>> lights) :
        camera(cam),
        integrator(integ),
        materials(std::move(mats)),
        primitives(std::move(prims)),
        lights(std::move(lights)) {};

    static Scene FromMitsubaXML(const char *filename);
    static Scene FromTungstenJSON(const char* filename);

    Camera camera;
    Integrator integrator;
    map<string, shared_ptr<Material>> materials;
    map<string, shared_ptr<Primitive>> primitives;
    map<string, shared_ptr<Primitive>> lights;
};

class FrameBuffer {
public:
    FrameBuffer() {};
    FrameBuffer(int resx, int resy) :
        resx(resx),
        resy(resy),
        buf(new vec3[resx * resy]) {};

    void set(pixel px, vec3 v) { buf[px.y * resx + px.x] = v; }
    vec3 get(pixel px) { return buf[px.y * resx + px.x]; }

    int resx;
    int resy;
    vec3* buf;
};

class Renderer {
public:
    Renderer() {};

    void loadTungstenJSON(const char* filename) {
        scene = Scene::FromTungstenJSON(filename);
        frame = FrameBuffer(scene.camera.resx,
                            scene.camera.resy);
    }

    void loadMitsubaXML(const char* filename) {
        scene = Scene::FromMitsubaXML(filename);
        frame = FrameBuffer(scene.camera.resx,
                            scene.camera.resy);
    }

    void render() {
        scene.integrator.render(scene, frame);
    }

    Scene scene;
    FrameBuffer frame;
};
