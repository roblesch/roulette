#ifndef RAY_H
#define RAY_H

#include "usings.h"

class Ray {
public:
    Ray() :
        pos(Vec3f(0.0f, 0.0f, 0.0f)),
        dir(Vec3f(0.0f, 0.0f, -1.0f)),
        t(0) {};
    Ray(const Vec3f &origin, const Vec3f &direction) :
            pos(origin),
            dir(direction),
            t(0) {};

    static Ray scatter(const Vec3f& newPos, const Vec3f& newDir, float newNearT, float newFarT = F_INFTY) {
        Ray ray;
        ray.pos = newPos;
        ray.dir = newDir;
        ray.tn = newNearT;
        ray.tf = newFarT;
        return ray;
    }

    [[nodiscard]] Vec3f at(float dt) const {
        return pos + dir * dt;
    }

    [[nodiscard]] const Vec3f& p() const {
        return pos;
    }

    void setP(const Vec3f& newP) {
        pos = newP;
    }

    [[nodiscard]] const Vec3f& d() const {
        return dir;
    }
    
    [[nodiscard]] const Vec3f& tip() const {
        static const Vec3f tip = pos + dir * tf;
        return tip;
    }

    [[nodiscard]] float tnear() const {
        return tn;
    }

    [[nodiscard]] float tfar() const {
        return tf;
    }

    [[nodiscard]] bool isPrimary() const {
        return primary;
    }

    void tnear(float tn_) {
        this->tn = tn_;
    }

    void tfar(float tf_) {
        this->tf = tf_;
    }

    void setPrimary(bool isP) {
        this->primary = isP;
    }

private:
    Vec3f pos;
    Vec3f dir;
    float t;
    float tn = F_NEAR_ZERO;
    float tf = F_INFTY;
    bool primary{};
};

#endif
