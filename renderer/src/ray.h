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

    [[nodiscard]] Vec3f at(float dt) const {
        return pos + dir * dt;
    }

    [[nodiscard]] const Vec3f& p() const {
        return pos;
    }

    [[nodiscard]] const Vec3f& d() const {
        return dir;
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

    void setPrimary() {
        primary = true;
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
