#include "scene.h"

vec3 as_vec3(const xml_node rgb_node) {
    istringstream stream(rgb_node.attribute("value").value());
    string token;
    float buf[3];
    for (float& i : buf) {
        stream >> token;
        i = stof(token);
    }
    return make_vec3(buf);
}

mat4 as_mat4(const xml_node matrix_node) {
    istringstream stream(matrix_node.attribute("value").value());
    string token;
    float buf[16];
    for (float& i : buf) {
        stream >> token;
        i = stof(token);
    }
    return make_mat4(buf);
}

bool is_shape(const xml_node shape_node, const string& type) {
    return ((string)shape_node.attribute("type").value()) == type;
}

Scene Scene::FromMitsubaXML(const char* filename) {
    xml_document doc;
    xml_parse_result result = doc.load_file(filename);

    shared_ptr<Camera> camera;
    map<string, shared_ptr<Material>> materials;
    map<string, shared_ptr<Primitive>> primitives;

    if (result.status != status_ok) {
        return { camera, materials, primitives };
    }

    xml_node scene = doc.child("scene");
    xml_node sensor = scene.child("sensor");
    xml_object_range<xml_named_node_iterator> bsdfs = scene.children("bsdf");
    xml_object_range<xml_named_node_iterator> shapes = scene.children("shape");

    if (sensor) {
        int width = scene.find_child_by_attribute("default", "name", "resx")
            .attribute("value").as_int();
        int height = scene.find_child_by_attribute("default", "name", "resy")
            .attribute("value").as_int();
        float fov = sensor.find_child_by_attribute("float", "name", "fov")
            .attribute("value").as_float();
        mat4 transform = as_mat4(sensor.child("transform").child("matrix"));
        camera = make_shared<Camera>(width, height, fov, transform);
    }
    
    for (xml_node _bsdf : bsdfs) {
        string matId = _bsdf.attribute("id").value();
        vec3 rgb = as_vec3(_bsdf.child("bsdf").child("rgb"));
        materials[matId] = make_shared<Diffuse>(rgb);
    }
    
    for (xml_node _shape : shapes) {
        shared_ptr<Shape> shape;
        shared_ptr<Emissive> emitter;

        string shapeId = _shape.attribute("id").value();
        string matId = _shape.child("ref").attribute("id").value();
        mat4 transform = as_mat4(_shape.child("transform").child("matrix"));

        if (is_shape(_shape, "rectangle")) {
            shape = make_shared<Rectangle>(transform);
        }
        else if (is_shape(_shape, "cube")) {
            shape = make_shared<Cube>(transform);
        }

        if (_shape.child("emitter")) {
            vec3 radiance = as_vec3(_shape.child("emitter").child("rgb"));
            emitter = make_shared<Emissive>(radiance);
        }

        primitives[shapeId] = make_shared<Primitive>(
            shape,
            materials[matId],
            emitter);
    }

    return { camera, materials, primitives };
}

Scene Scene::FromTungstenJSON(const char* filename) {
    ifstream f(filename);
    json data = json::parse(f);
    return {};
};
