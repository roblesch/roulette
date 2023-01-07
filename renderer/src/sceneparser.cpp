#include "sceneparser.h"

vec3f as_vec3(const xml_node &node) {
    std::istringstream stream(node.attribute("value").value());
    string token;
    float buf[3];
    for (float &i: buf) {
        stream >> token;
        i = stof(token);
    }
    return glm::make_vec3(buf);
}

vec3f as_vec3(const value_type &value) {
    return glm::make_vec3(value.get<vector<float>>().data());
}

mat4f as_mat4(const xml_node &node) {
    std::istringstream stream(node.attribute("value").value());
    string token;
    float buf[16];
    for (float &i: buf) {
        stream >> token;
        i = stof(token);
    }
    return glm::make_mat4(buf);
}

bool is_shape(const xml_node &shape_node, const string &type) {
    return ((string) shape_node.attribute("type").value()) == type;
}

void SceneParser::FromMitsubaXML(Scene &scene, FrameBuffer &frame, unique_ptr<Integrator> &integrator,
                                 const char *filename) {
    xml_document doc;
    xml_parse_result result = doc.load_file(filename);

    if (result.status != pugi::status_ok) {
    }

    Camera camera{};
    unordered_map<string, shared_ptr<Material>> materials;
    unordered_map<string, shared_ptr<Primitive>> primitives;
    unordered_map<string, shared_ptr<Primitive>> lights;

    xml_node data = doc.child("scene");
    xml_node sensor = data.child("sensor");
    xml_object_range<xml_named_node_iterator> bsdfs = data.children("bsdf");
    xml_object_range<xml_named_node_iterator> shapes = data.children("shape");

    if (sensor) {
        int width = data.find_child_by_attribute("default", "name", "resx")
                .attribute("value").as_int();
        int height = data.find_child_by_attribute("default", "name", "resy")
                .attribute("value").as_int();
        float fov = sensor.find_child_by_attribute("float", "name", "fov")
                .attribute("value").as_float();
        mat4f transform = as_mat4(sensor.child("transform").child("matrix"));
        camera = Camera(width, height, fov, transform);
    }

    for (xml_node _bsdf: bsdfs) {
        string matId = _bsdf.attribute("id").value();
        vec3f rgb = as_vec3(_bsdf.child("bsdf").child("rgb"));
        materials[matId] = make_shared<Lambertian>(rgb);
    }

    for (xml_node _shape: shapes) {
        shared_ptr<Shape> shape;
        shared_ptr<Emitter> emitter;

        string shapeId = _shape.attribute("id").value();
        string matId = _shape.child("ref").attribute("id").value();
        mat4f transform = as_mat4(_shape.child("transform").child("matrix"));

        if (is_shape(_shape, "rectangle")) {
            shape = make_shared<Rectangle>(transform);
        } else if (is_shape(_shape, "cube")) {
            shape = make_shared<Cube>(transform);
        }

        primitives[shapeId] = make_shared<Primitive>(
                shape,
                materials[matId],
                emitter);

        if (_shape.child("emitter")) {
            vec3f radiance = as_vec3(_shape.child("emitter").child("rgb"));
            emitter = make_shared<Emitter>(radiance);
            primitives[shapeId]->emitter = emitter;
            lights[shapeId] = primitives[shapeId];
        }
    }

    scene = Scene(camera, materials, primitives, lights);
    frame = FrameBuffer(camera.resx, camera.resy);
    integrator = make_unique<PathIntegrator>();
}

void SceneParser::FromTungstenJSON(Scene &scene, FrameBuffer &frame, unique_ptr<Integrator> &integrator,
                                   const char *filename) {
    std::ifstream f(filename);
    json data = json::parse(f);

    if (data.empty()) {
    }

    Camera camera{};
    unordered_map<string, shared_ptr<Material>> materials;
    unordered_map<string, shared_ptr<Primitive>> primitives;
    unordered_map<string, shared_ptr<Primitive>> lights;

    for (value_type _bsdf: data["bsdfs"]) {
        string name = _bsdf["name"];
        if (_bsdf["type"] == "lambert") {
            vec3f albedo = as_vec3(_bsdf["albedo"]);
            materials[name] = make_shared<Lambertian>(albedo);
        } else if (_bsdf["type"] == "null") {
            float albedo = _bsdf["albedo"];
            materials[name] = make_shared<Lambertian>(vec3f(albedo));
        }
    }

    for (value_type _prim: data["primitives"]) {
        shared_ptr<Shape> shape;
        shared_ptr<Emitter> emit;

        string name = _prim["bsdf"];
        value_type tsf = _prim["transform"];

        vec3f pos = tsf.contains("position") ?
                    as_vec3(tsf["position"]) :
                    vec3f(0);
        vec3f scale = tsf.contains("scale") ?
                      as_vec3(tsf["scale"]) :
                      vec3f(1);
        vec3f rot = tsf.contains("rotation") ?
                    as_vec3(tsf["rotation"]) :
                    vec3f(0);

        if (_prim["type"] == "quad") {
            shape = make_shared<Rectangle>(pos, scale, rot);
        } else if (_prim["type"] == "cube") {
            shape = make_shared<Cube>(pos, scale, rot);
        }

        primitives[name] = make_shared<Primitive>(
                shape,
                materials[name],
                emit);

        if (_prim.contains("emission")) {
            vec3f radiance = as_vec3(_prim["emission"]);
            emit = make_shared<Emitter>(radiance);
            primitives[name]->emitter = emit;
            lights[name] = primitives[name];
        }
    }

    if (data.contains("camera")) {
        int resx = data["camera"]["resolution"][0];
        int resy = data["camera"]["resolution"][1];
        float fovx = data["camera"]["fov"];
        vec3f eye = as_vec3(data["camera"]["transform"]["position"]);
        vec3f cent = as_vec3(data["camera"]["transform"]["look_at"]);
        vec3f up = as_vec3(data["camera"]["transform"]["up"]);
        camera = Camera(resx, resy, fovx, eye, cent, up);
    }

    if (data.contains("integrator")) {
        if (data["integrator"]["type"] == "path_tracer") {
            integrator = make_unique<PathIntegrator>();
        }
    }

    scene = Scene(camera, materials, primitives, lights);
    frame = FrameBuffer(camera.resx, camera.resy);
}
