#include "shape.h"

bool Rectangle::intersect(const Ray& ray, IntersectionPrimitive& intersection) {
    float nDotW = ray.dir().dot(_normal);
    if (std::abs(nDotW) < 1e-6f)
        return false;

    float t = _normal.dot(_base - ray.pos()) / nDotW;
    if (t < intersection.tnear || t > intersection.tfar)
        return false;

    Vec3f q = ray.pos() + t * ray.dir();
    Vec3f v = q - _base;
    float l0 = v.dot(_edge0) * _invUvSq.x();
    float l1 = v.dot(_edge1) * _invUvSq.y();

    if (l0 < 0.0f || l0 > 1.0f || l1 < 0.0f || l1 > 1.0f)
        return false;

    intersection.tfar = t;
    return true;
}

bool Cube::intersect(const Ray& ray, IntersectionPrimitive& intersection) {
    Vec3f p = invRot * (ray.pos() - pos);
    Vec3f d = invRot * ray.dir();

    Vec3f invD = 1.0f / d;
    Vec3f relMin((-scale - p));
    Vec3f relMax((scale - p));

    float ttMin = intersection.tnear, ttMax = intersection.tfar;
    for (int i = 0; i < 3; ++i) {
        if (invD[i] >= 0.0f) {
            ttMin = max(ttMin, relMin[i] * invD[i]);
            ttMax = min(ttMax, relMax[i] * invD[i]);
        }
        else {
            ttMax = min(ttMax, relMin[i] * invD[i]);
            ttMin = max(ttMin, relMax[i] * invD[i]);
        }
    }

    if (ttMin <= ttMax) {
        if (ttMin > intersection.tnear && ttMin < intersection.tfar) {
            intersection.tfar = ttMin;
            intersection.front = true;
        }
        else if (ttMax > intersection.tnear && ttMax < intersection.tfar) {
            intersection.tfar = ttMax;
            intersection.front = false;
        }
        return true;
    }
    return false;
}
