#ifndef AABB_H
#define AABB_H

#include "usings.h"

class AABB {
public:
    AABB() :
        min(std::numeric_limits<Vec3f>::lowest()),
        max(std::numeric_limits<Vec3f>::max())
    {};

    void grow(const Vec3f& v) {
        min = v3fmin(min, v);
        max = v3fmax(max, v);
    }

    Vec3f getExtents() const {
        return max - min;
    }

    Vec3f min;
    Vec3f max;
};

#endif
