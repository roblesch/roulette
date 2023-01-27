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

    Ray scatter(const Vec3f& newP, const Vec3f& newD, float newTnear = F_NEAR_ZERO, float newTfar = F_INFTY) const {
        Ray ray(*this);
        ray.pos = newP;
        ray.dir = newD;
        ray.tn = newTnear;
        ray.tf = newTfar;
        return ray;
    }

    [[nodiscard]] Vec3f at(float dt) const {
        return pos + dir * dt;
    }

    [[nodiscard]] const Vec3f& p() const {
        return pos;
    }

    [[nodiscard]] const Vec3f& d() const {
        return dir;
    }
    
    [[nodiscard]] const Vec3f& tip() const {
        return pos + dir * tf;
    }

    float tnear() {
        return tn;
    }

    float tfar() {
        return tf;
    }

    bool isPrimary() {
        return primary;
    }

    void tnear(float tn) {
        this->tn = tn;
    }

    void tfar(float tf) {
        this->tf = tf;
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
    bool primary;
};

#endif
