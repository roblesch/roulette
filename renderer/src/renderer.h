#pragma once

#include "usings.h"

#include "framebuffer.h"
#include "scene.h"

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
        frame.toPng("out.png");
    }

    Scene scene;
    FrameBuffer frame;
};
