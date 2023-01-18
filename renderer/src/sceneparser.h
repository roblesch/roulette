#ifndef SCENEPARSER_H
#define SCENEPARSER_H

#include "usings.h"

#include "framebuffer.h"
#include "integrator.h"
#include "material.h"
#include "primitive.h"
#include "shape.h"
#include "scene.h"

#include <iostream>

#include <nlohmann/json.hpp>
#include "pugixml.hpp"

using nlohmann::json;
using value_type = json::value_type;

using pugi::xml_document;
using pugi::xml_named_node_iterator;
using pugi::xml_node;
using pugi::xml_object_range;
using pugi::xml_parse_result;

class SceneParser {
public:
    static void
    FromMitsubaXML(Scene &scene, FrameBuffer &frame, unique_ptr<Integrator> &integrator, const char *filename);

    static void
    FromTungstenJSON(Scene &scene, FrameBuffer &frame, unique_ptr<Integrator> &integrator, const char *filename);
};

#endif
