#ifndef USINGS_H
#define USINGS_H

#include <algorithm>
#include <fstream>
#include <unordered_map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "math.h"

using std::unordered_map;
using std::shared_ptr;
using std::string;
using std::unique_ptr;
using std::vector;
using std::make_shared;
using std::make_unique;

inline float clamp(float f) { return f < 0.0f ? 0.0f : (f > 1.0f ? 1.0f : f); }

inline float d_to_r(float d) { return d * (PI / 180.0f); }

inline float r_to_d(float r) { return r * (180.0f / PI); }

#endif
