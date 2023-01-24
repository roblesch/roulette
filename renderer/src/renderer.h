#ifndef RENDERER_H
#define RENDERER_H

#include "usings.h"

#include "framebuffer.h"
#include "integrator.h"
#include "scene.h"
#include "sceneparser.h"

class Renderer {
public:
    Renderer() = default;

    void loadTungstenJSON(const char *filename) {
        SceneParser::FromTungstenJSON(scene, frame, integrator, filename);
    }

    void loadMitsubaXML(const char *filename) {
        SceneParser::FromMitsubaXML(scene, frame, integrator, filename);
    }

    void render() {
        integrator->render(scene, frame);
        frame.toPng("out.png");
    }

    Scene scene;
    FrameBuffer frame;
    unique_ptr<Integrator> integrator;
};

#endif
