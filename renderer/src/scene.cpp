#include "scene.h"

float as_float(const pugi::xml_node fl) {
    return stof((std::string) fl.attribute("value").value());
}

glm::vec3 as_vec3(const pugi::xml_node rgb) {
    std::istringstream stream(rgb.attribute("value").value());
    std::string token;
    float buf[3];
    for (float &i : buf) {
        stream >> token;
        i = stof(token);
    }
    return glm::make_vec3(buf);
}

bool is_shape(const pugi::xml_node shape, const std::string &type) {
    return ((std::string) shape.attribute("type").value()) == type;
}

glm::mat4 as_mat4(const pugi::xml_node matrix) {
    std::istringstream stream(matrix.attribute("value").value());
    std::string token;
    float buf[16];
    for (float &i : buf) {
        stream >> token;
        i = stof(token);
    }
    return glm::make_mat4(buf);
}

Scene Scene::FromMitsubaXML(const char *filename) {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filename);

    std::shared_ptr<Camera> cam;
    std::map<std::string, std::shared_ptr<Material>> mats;
    std::map<std::string, std::shared_ptr<Primitive>> prims;

    if (result.status == pugi::status_ok) {
        pugi::xml_node scene = doc.child("scene");
        pugi::xml_node camera = scene.child("sensor");
        pugi::xml_object_range<pugi::xml_named_node_iterator> bsdfs = scene.children("bsdf");
        pugi::xml_object_range<pugi::xml_named_node_iterator> shapes = scene.children("shape");

        if (camera) {
            float fov = as_float(camera.child("float"));
            glm::mat4 transform = as_mat4(camera.child("transform").child("matrix"));
            cam = std::make_shared<Camera>(fov, transform);
        }

        for (pugi::xml_node _bsdf : bsdfs) {
            std::string matId = _bsdf.attribute("id").value();
            glm::vec3 rgb = as_vec3(_bsdf.child("bsdf").child("rgb"));
            mats[matId] = std::make_shared<Diffuse>(rgb);
        }

        for (pugi::xml_node _shape : shapes) {
            std::shared_ptr<Shape> shape;
            std::shared_ptr<Emissive> emitter;

            std::string shapeId = _shape.attribute("id").value();
            std::string matId = _shape.child("ref").attribute("id").value();
            glm::mat4 transform = as_mat4(_shape.child("transform").child("matrix"));

            if (is_shape(_shape, "rectangle")) {
                shape = std::make_shared<Rectangle>(transform);
            } else if (is_shape(_shape, "cube")) {
                shape = std::make_shared<Cube>(transform);
            }

            if (_shape.child("emitter")) {
                glm::vec3 radiance = as_vec3(_shape.child("emitter").child("rgb"));
                emitter = std::make_shared<Emissive>(radiance);
            }

            prims[shapeId] = std::make_shared<Primitive>(
                shape,
                mats[matId],
                emitter);
        }
    }

    return {cam, mats, prims};
}
