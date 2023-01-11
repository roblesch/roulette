#include "sceneparser.h"

Vec3f as_vec3(const xml_node &node) {
    std::istringstream stream(node.attribute("value").value());
    string token;
    float buf[3];
    for (float &i: buf) {
        stream >> token;
        i = stof(token);
    }
    return Vec3f(buf);
}

Vec3f as_vec3(const value_type &value) {
    return Vec3f(value.get<vector<float>>().data());
}

Mat4f as_mat4(const xml_node &node) {
    std::istringstream stream(node.attribute("value").value());
    string token;
    float buf[16];
    for (float &i: buf) {
        stream >> token;
        i = stof(token);
    }
    return {
         buf[0],  buf[1],  buf[2],  buf[3],
         buf[4],  buf[5],  buf[6],  buf[7],
         buf[8],  buf[9], buf[10], buf[11],
        buf[12], buf[13], buf[14], buf[15]
    };
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
        Mat4f transform = as_mat4(sensor.child("transform").child("matrix"));
        camera = Camera(width, height, fov, transform);
    }

    for (xml_node _bsdf: bsdfs) {
        string matId = _bsdf.attribute("id").value();
        Vec3f rgb = as_vec3(_bsdf.child("bsdf").child("rgb"));
        materials[matId] = make_shared<Lambertian>(rgb);
    }

    for (xml_node _shape: shapes) {
        shared_ptr<Shape> shape;
        shared_ptr<Emitter> emitter;

        string shapeId = _shape.attribute("id").value();
        string matId = _shape.child("ref").attribute("id").value();
        Mat4f transform = as_mat4(_shape.child("transform").child("matrix"));

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
            Vec3f radiance = as_vec3(_shape.child("emitter").child("rgb"));
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
            Vec3f albedo = as_vec3(_bsdf["albedo"]);
            materials[name] = make_shared<Lambertian>(albedo);
        } else if (_bsdf["type"] == "null") {
            float albedo = _bsdf["albedo"];
            materials[name] = make_shared<Lambertian>(Vec3f(albedo, albedo, albedo));
        }
    }

    for (value_type _prim: data["primitives"]) {
        shared_ptr<Shape> shape;
        shared_ptr<Emitter> emit;

        string name = _prim["bsdf"];
        value_type tsf = _prim["transform"];

        Vec3f pos = tsf.contains("position") ?
                    as_vec3(tsf["position"]) :
                    Vec3f(0.0f, 0.0f, 0.0f);
        Vec3f scale = tsf.contains("scale") ?
                      as_vec3(tsf["scale"]) :
                      Vec3f(1.0f, 1.0f, 1.0f);
        Vec3f rot = tsf.contains("rotation") ?
                    as_vec3(tsf["rotation"]) :
                    Vec3f(0.0f, 0.0f, 0.0f);

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
            Vec3f radiance = as_vec3(_prim["emission"]);
            emit = make_shared<Emitter>(radiance);
            primitives[name]->emitter = emit;
            lights[name] = primitives[name];
        }
    }

    if (data.contains("camera")) {
        int resx = data["camera"]["resolution"][0];
        int resy = data["camera"]["resolution"][1];
        float fovx = data["camera"]["fov"];
        Vec3f eye = as_vec3(data["camera"]["transform"]["position"]);
        Vec3f cent = as_vec3(data["camera"]["transform"]["look_at"]);
        Vec3f up = as_vec3(data["camera"]["transform"]["up"]);
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
