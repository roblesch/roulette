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

class FrameBuffer {
public:
    FrameBuffer() {};
    FrameBuffer(int resx, int resy) :
        resx(resx),
        resy(resy),
        buf(vector<vec3f>(resx*resy)) {};

    void set(pixel px, vec3f v) { buf[px.y * resx + px.x] = v; }
    vec3f get(pixel px) { return buf[px.y * resx + px.x]; }

    void toFile(const char* filename) {
        std::ofstream out(filename);
        out << "P3\n" << resx << ' ' << resy << "\n255\n";
        for (int j = 0; j < resy; j++) {
            for (int i = 0; i < resx; i++) {
                vec3f px = get(pixel(i,j));
                out << static_cast<int>(256 * clamp(px.r)) << ' '
                    << static_cast<int>(256 * clamp(px.g)) << ' '
                    << static_cast<int>(256 * clamp(px.b)) << '\n';
            }
        }
    }

    int resx;
    int resy;
    vector<vec3f> buf;
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
        scene.integrator->render(scene, frame);
        frame.toFile("out.ppm");
    }

    Scene scene;
    FrameBuffer frame;
};
