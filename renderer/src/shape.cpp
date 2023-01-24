/*
Intersection tests for Rectangle and Quad from Benedikt Bitterli's Tungsten
renderer - https://github.com/tunabrain/tungsten.

See License.txt for tungsten's license.
*/

#include "shape.h"

bool Rectangle::intersect(Ray& ray, IntersectionData& intersection) {
    float nDotW = ray.d().dot(frame.normal);
    if (std::abs(nDotW) < 1e-6f)
        return false;

    float t = frame.normal.dot(base - ray.p()) / nDotW;
    if (t < ray.tnear() || t > ray.tfar())
        return false;

    Vec3f q = ray.p() + t * ray.d();
    Vec3f v = q - base;
    float l0 = v.dot(edge0) * invUvSq.x();
    float l1 = v.dot(edge1) * invUvSq.y();
    if (l0 < 0.0f || l0 > 1.0f || l1 < 0.0f || l1 > 1.0f)
        return false;

    ray.tfar(t);
    return true;
}

bool Cube::intersect(Ray& ray, IntersectionData& intersection) {
    Vec3f p = invRot * (ray.p() - pos);
    Vec3f d = invRot * ray.d();
    Vec3f invD = 1.0f / d;
    Vec3f relMin((-scale - p));
    Vec3f relMax((scale - p));

    float ttMin = ray.tnear(), ttMax = ray.tfar();
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
        if (ttMin == ray.tnear()) {
            ray.tfar(ttMax);
            intersection.front = false;
        }
        else {
            ray.tfar(ttMin);
            intersection.front = true;
        }
        return true;
    }
    return false;
}

void Rectangle::setIntersectionData(IntersectionData& intersection) {
    intersection.Ng = intersection.Ns = frame.normal;
}

void Cube::setIntersectionData(IntersectionData& intersection) {
    Vec3f p = invRot * (intersection.p - pos);
    Vec3f n(0.0f);
    int dim = (abs(p) - scale).maxDim();
    n[dim] = p[dim] < 0.0f ? -1.0f : 1.0f;
    intersection.Ns = intersection.Ng = rot4 * n;
    return;
}

bool Rectangle::sampleDirect(const Vec3f& p, LightSample& sample) {
    if (frame.normal.dot(p - base) <= 0.0f)
        return false;

    Vec2f xi(randf(), randf());
    Vec3f q = base + xi.x() * edge0 + xi.y() * edge1;
    sample.d = q - p;
    float rSq = sample.d.lengthSq();
    sample.dist = std::sqrt(rSq);
    sample.d /= sample.dist;
    float cosTheta = -frame.normal.dot(sample.d);
    sample.pdf = rSq / (cosTheta * area);

    return true;
}
