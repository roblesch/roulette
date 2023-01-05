#pragma once

#include <algorithm>
#include <fstream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <nlohmann/json.hpp>
#include "pugixml.hpp"

using std::map;
using std::shared_ptr;
using std::string;
using std::unique_ptr;
using std::vector;
using std::make_shared;
using std::make_unique;

using vec2i = glm::u16vec2;
using vec3c = glm::u8vec3;
using vec3f = glm::f32vec3;
using vec4f = glm::f32vec4;
using vec4c = glm::u8vec4;
using mat4f = glm::f32mat4;

using nlohmann::json;
using value_type = json::value_type;

using pugi::xml_document;
using pugi::xml_named_node_iterator;
using pugi::xml_node;
using pugi::xml_object_range;
using pugi::xml_parse_result;

inline void __NO_OP() {}
#define NO_OP __NO_OP ()

const float PI = 3.1415926536f;
const float PI_HALF = PI * 0.5f;
const float TWO_PI = PI * 2.0f;
const float FOUR_PI = PI * 4.0f;
const float INV_PI = 1.0f / PI;
const float INV_TWO_PI = 0.5f * INV_PI;
const float INV_FOUR_PI = 0.25f * INV_PI;
const float SQRT_PI = 1.77245385091f;
const float INV_SQRT_PI = 1.0f / SQRT_PI;

inline float clamp(float f) { return f < 0.0f ? 0.0f : (f > 1.0f ? 1.0f : f); }
inline float d_to_r(float d) { return d * (PI / 180.0f); }
inline float r_to_d(float r) { return r * (180.0f / PI); }

inline vec3f transformVec(const mat4f& m, const vec3f& v) {
    return vec3f(
        m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z,
        m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z,
        m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z
    );
}
