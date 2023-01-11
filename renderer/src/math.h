/*
Copyright (c) 2014 Benedikt Bitterli <benedikt.bitterli (at) gmail (dot) com>

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from
the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute
it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you
       must not claim that you wrote the original software. If you
       use this software in a product, an acknowledgment in the
       product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and
       must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
       distribution.
*/

#ifndef MATH_H
#define MATH_H

#include <array>
#include <cmath>
#include <cstdint>
#include <ostream>
#include <type_traits>

const float PI = 3.1415926536f;
const float PI_HALF = PI * 0.5f;
const float TWO_PI = PI * 2.0f;
const float FOUR_PI = PI * 4.0f;
const float INV_PI = 1.0f / PI;
const float INV_TWO_PI = 0.5f * INV_PI;
const float INV_FOUR_PI = 0.25f * INV_PI;
const float SQRT_PI = 1.77245385091f;
const float INV_SQRT_PI = 1.0f / SQRT_PI;

typedef std::uint8_t uint8;
typedef std::uint16_t uint16;
typedef std::uint32_t uint32;
typedef std::uint64_t uint64;

typedef std::int8_t int8;
typedef std::int16_t int16;
typedef std::int32_t int32;
typedef std::int64_t int64;

template<typename ElementType, unsigned Size>
class Vec {
protected:
    std::array<ElementType, Size> _v;

public:
    static const unsigned size = Size;

    Vec() = default;

    explicit Vec(const ElementType &a) {
        for (unsigned i = 0; i < Size; ++i)
            _v[i] = a;
    }

    explicit Vec(const ElementType *a) {
        for (unsigned i = 0; i < Size; ++i)
            _v[i] = a[i];
    }

    Vec(const ElementType &a, const ElementType &b) : _v({a, b}) {
    }

    Vec(const ElementType &a, const ElementType &b, const ElementType &c) : _v({a, b, c}) {
    }

    Vec(const ElementType &a, const ElementType &b, const ElementType &c, const ElementType &d) : _v({a, b, c, d}) {
    }

    template<typename OtherType>
    explicit Vec(const Vec<OtherType, Size> &other) {
        for (unsigned i = 0; i < Size; ++i)
            _v[i] = ElementType(other[i]);
    }

    [[nodiscard]] ElementType x() const {
        return _v[0];
    }

    [[nodiscard]] ElementType y() const {
        static_assert(Size > 1, "Vector does not have y coordinate");
        return _v[1];
    }

    [[nodiscard]] ElementType z() const {
        static_assert(Size > 2, "Vector does not have z coordinate");
        return _v[2];
    }

    [[nodiscard]] ElementType w() const {
        static_assert(Size > 3, "Vector does not have w coordinate");
        return _v[3];
    }

    ElementType &x() {
        return _v[0];
    }

    ElementType &y() {
        static_assert(Size > 1, "Vector does not have y coordinate");
        return _v[1];
    }

    ElementType &z() {
        static_assert(Size > 2, "Vector does not have z coordinate");
        return _v[2];
    }

    ElementType &w() {
        static_assert(Size > 3, "Vector does not have w coordinate");
        return _v[3];
    }

    [[nodiscard]] ElementType r() const {
        return _v[0];
    }

    [[nodiscard]] ElementType g() const {
        static_assert(Size > 1, "Vector does not have y coordinate");
        return _v[1];
    }

    [[nodiscard]] ElementType b() const {
        static_assert(Size > 2, "Vector does not have z coordinate");
        return _v[2];
    }

    [[nodiscard]] ElementType a() const {
        static_assert(Size > 3, "Vector does not have w coordinate");
        return _v[3];
    }

    ElementType &r() {
        return _v[0];
    }

    ElementType &g() {
        static_assert(Size > 1, "Vector does not have y coordinate");
        return _v[1];
    }

    ElementType &b() {
        static_assert(Size > 2, "Vector does not have z coordinate");
        return _v[2];
    }

    ElementType &a() {
        static_assert(Size > 3, "Vector does not have w coordinate");
        return _v[3];
    }

    [[nodiscard]] Vec<ElementType, 2> xy() const {
        return Vec<ElementType, 2>(x(), y());
    }

    [[nodiscard]] Vec<ElementType, 3> xyz() const {
        return Vec<ElementType, 3>(x(), y(), z());
    }

    [[nodiscard]] Vec<ElementType, 2> xz() const {
        return Vec<ElementType, 2>(x(), z());
    }

    [[nodiscard]] Vec<ElementType, 2> yz() const {
        return Vec<ElementType, 2>(y(), z());
    }

    ElementType *data() {
        return &_v[0];
    }

    [[nodiscard]] const ElementType *data() const {
        return &_v[0];
    }

    [[nodiscard]] ElementType lengthSq() const {
        auto res = ElementType(0);
        for (unsigned i = 0; i < Size; ++i)
            res += _v[i] * _v[i];
        return res;
    }

    [[nodiscard]] ElementType length() const {
        return std::sqrt(lengthSq());
    }

    [[nodiscard]] ElementType sum() const {
        ElementType result(_v[0]);
        for (unsigned i = 1; i < Size; ++i)
            result += _v[i];
        return result;
    }

    [[nodiscard]] ElementType avg() const {
        return sum() * (ElementType(1) / ElementType(Size));
    }

    [[nodiscard]] ElementType product() const {
        ElementType result(_v[0]);
        for (unsigned i = 1; i < Size; ++i)
            result *= _v[i];
        return result;
    }

    [[nodiscard]] Vec abs() const {
        Vec other(*this);
        for (unsigned i = 0; i < Size; ++i)
            other._v[i] = std::abs(_v[i]);
        return other;
    }

    void normalize() {
        ElementType invLen = ElementType(1) / length();
        for (unsigned i = 0; i < Size; ++i)
            _v[i] *= invLen;
    }

    [[nodiscard]] Vec normalized() const {
        ElementType invLen = ElementType(1) / length();
        Vec other(*this);
        for (unsigned i = 0; i < Size; ++i)
            other._v[i] *= invLen;
        return other;
    }

    [[nodiscard]] ElementType dot(const Vec &other) const {
        ElementType sum = _v[0] * other._v[0];
        for (unsigned i = 1; i < Size; ++i)
            sum += _v[i] * other._v[i];
        return sum;
    }

    [[nodiscard]] Vec cross(const Vec &other) const {
        static_assert(Size == 3, "Cross product only defined in three dimensions!");
        return Vec(
                y() * other.z() - z() * other.y(),
                z() * other.x() - x() * other.z(),
                x() * other.y() - y() * other.x()
        );
    }

    [[nodiscard]] ElementType luminance() const {
        static_assert(Size == 3, "Luminance only supported in three dimensions!");
        return x() * ElementType(0.2126) + y() * ElementType(0.7152) + z() * ElementType(0.0722);
    }

    ElementType &operator[](unsigned i) {
        return _v[i];
    }

    const ElementType &operator[](unsigned i) const {
        return _v[i];
    }

    Vec operator-() const {
        Vec result;
        for (unsigned i = 0; i < Size; ++i)
            result._v[i] = -_v[i];
        return result;
    }

    Vec operator+(const Vec &other) const {
        Vec result;
        for (unsigned i = 0; i < Size; ++i)
            result._v[i] = _v[i] + other._v[i];
        return result;
    }

    Vec operator-(const Vec &other) const {
        Vec result;
        for (unsigned i = 0; i < Size; ++i)
            result._v[i] = _v[i] - other._v[i];
        return result;
    }

    Vec operator*(const Vec &other) const {
        Vec result;
        for (unsigned i = 0; i < Size; ++i)
            result._v[i] = _v[i] * other._v[i];
        return result;
    }

    Vec operator/(const Vec &other) const {
        Vec result;
        for (unsigned i = 0; i < Size; ++i)
            result._v[i] = _v[i] / other._v[i];
        return result;
    }

    Vec operator+(const ElementType &a) const {
        Vec result;
        for (unsigned i = 0; i < Size; ++i)
            result._v[i] = _v[i] + a;
        return result;
    }

    Vec operator-(const ElementType &a) const {
        Vec result;
        for (unsigned i = 0; i < Size; ++i)
            result._v[i] = _v[i] - a;
        return result;
    }

    Vec operator*(const ElementType &a) const {
        Vec result{};
        for (unsigned i = 0; i < Size; ++i)
            result._v[i] = _v[i] * a;
        return result;
    }

    Vec operator/(const ElementType &a) const {
        Vec result{};
        for (unsigned i = 0; i < Size; ++i)
            result._v[i] = _v[i] / a;
        return result;
    }

    Vec operator>>(const ElementType &a) const {
        Vec result;
        for (unsigned i = 0; i < Size; ++i)
            result._v[i] = _v[i] >> a;
        return result;
    }

    Vec operator<<(const ElementType &a) const {
        Vec result;
        for (unsigned i = 0; i < Size; ++i)
            result._v[i] = _v[i] << a;
        return result;
    }

    Vec operator+=(const Vec &other) {
        for (unsigned i = 0; i < Size; ++i)
            _v[i] += other._v[i];
        return *this;
    }

    Vec operator-=(const Vec &other) {
        for (unsigned i = 0; i < Size; ++i)
            _v[i] -= other._v[i];
        return *this;
    }

    Vec operator*=(const Vec &other) {
        for (unsigned i = 0; i < Size; ++i)
            _v[i] *= other._v[i];
        return *this;
    }

    Vec operator/=(const Vec &other) {
        for (unsigned i = 0; i < Size; ++i)
            _v[i] /= other._v[i];
        return *this;
    }

    Vec operator+=(const ElementType &a) {
        for (unsigned i = 0; i < Size; ++i)
            _v[i] += a;
        return *this;
    }

    Vec operator-=(const ElementType &a) {
        for (unsigned i = 0; i < Size; ++i)
            _v[i] -= a;
        return *this;
    }

    Vec operator*=(const ElementType &a) {
        for (unsigned i = 0; i < Size; ++i)
            _v[i] *= a;
        return *this;
    }

    Vec operator/=(const ElementType &a) {
        for (unsigned i = 0; i < Size; ++i)
            _v[i] /= a;
        return *this;
    }

    Vec operator>>=(const ElementType &a) const {
        for (unsigned i = 0; i < Size; ++i)
            _v[i] >>= a;
        return *this;
    }

    Vec operator<<=(const ElementType &a) const {
        for (unsigned i = 0; i < Size; ++i)
            _v[i] <<= a;
        return *this;
    }

    [[nodiscard]] uint32 maxDim() const {
        ElementType m = _v[0];
        uint32 idx = 0;
        for (unsigned i = 1; i < Size; ++i) {
            if (_v[i] > m) {
                m = _v[i];
                idx = i;
            }
        }
        return idx;
    }

    [[nodiscard]] uint32 minDim() const {
        ElementType m = _v[0];
        uint32 idx = 0;
        for (unsigned i = 1; i < Size; ++i) {
            if (_v[i] < m) {
                m = _v[i];
                idx = i;
            }
        }
        return idx;
    }

    [[nodiscard]] ElementType max() const {
        ElementType m = _v[0];
        for (unsigned i = 1; i < Size; ++i)
            if (_v[i] > m)
                m = _v[i];
        return m;
    }

    [[nodiscard]] ElementType min() const {
        ElementType m = _v[0];
        for (unsigned i = 1; i < Size; ++i)
            if (_v[i] < m)
                m = _v[i];
        return m;
    }

    bool operator==(const Vec &o) const {
        for (unsigned i = 0; i < Size; ++i)
            if (_v[i] != o[i])
                return false;
        return true;
    }

    bool operator!=(const Vec &o) const {
        for (unsigned i = 0; i < Size; ++i)
            if (_v[i] != o[i])
                return true;
        return false;
    }

    bool operator==(const ElementType &a) const {
        for (unsigned i = 0; i < Size; ++i)
            if (_v[i] != a)
                return false;
        return true;
    }

    bool operator!=(const ElementType &a) const {
        for (unsigned i = 0; i < Size; ++i)
            if (_v[i] != a)
                return true;
        return false;
    }

    friend std::ostream &operator<<(std::ostream &stream, const Vec &v) {
        stream << '(';
        for (uint32 i = 0; i < Size; ++i)
            stream << v[i] << (i == Size - 1 ? ')' : ',');
        return stream;
    }
};

template<typename ElementType, unsigned Size>
Vec<ElementType, Size> operator+(const ElementType &a, const Vec<ElementType, Size> &b) {
    Vec<ElementType, Size> result;
    for (unsigned i = 0; i < Size; ++i)
        result[i] = a + b[i];
    return result;
}

template<typename ElementType, unsigned Size>
Vec<ElementType, Size> operator-(const ElementType &a, const Vec<ElementType, Size> &b) {
    Vec<ElementType, Size> result;
    for (unsigned i = 0; i < Size; ++i)
        result[i] = a - b[i];
    return result;
}

template<typename ElementType, unsigned Size>
Vec<ElementType, Size> operator*(const ElementType &a, const Vec<ElementType, Size> &b) {
    Vec<ElementType, Size> result;
    for (unsigned i = 0; i < Size; ++i)
        result[i] = a * b[i];
    return result;
}

template<typename ElementType, unsigned Size>
Vec<ElementType, Size> operator/(const ElementType &a, const Vec<ElementType, Size> &b) {
    Vec<ElementType, Size> result;
    for (unsigned i = 0; i < Size; ++i)
        result[i] = a / b[i];
    return result;
}

typedef Vec<double, 4> Vec4d;
typedef Vec<double, 3> Vec3d;
typedef Vec<double, 2> Vec2d;

typedef Vec<float, 4> Vec4f;
typedef Vec<float, 3> Vec3f;
typedef Vec<float, 2> Vec2f;

typedef Vec<uint32, 4> Vec4u;
typedef Vec<uint32, 3> Vec3u;
typedef Vec<uint32, 2> Vec2u;

typedef Vec<int32, 4> Vec4i;
typedef Vec<int32, 3> Vec3i;
typedef Vec<int32, 2> Vec2i;

typedef Vec<uint8, 4> Vec4c;
typedef Vec<uint8, 3> Vec3c;
typedef Vec<uint8, 2> Vec2c;

class Mat4f;

static inline Mat4f operator*(const Mat4f &a, const Mat4f &b);

static inline Vec4f operator*(const Mat4f &a, const Vec4f &b);

static inline Vec3f operator*(const Mat4f &a, const Vec3f &b);

class Mat4f {
    union {
        struct {
            float a11, a12, a13, a14;
            float a21, a22, a23, a24;
            float a31, a32, a33, a34;
            float a41, a42, a43, a44;
        };
        float a[16]{};
    };

public:
    Mat4f() {
        a12 = a13 = a14 = 0.0f;
        a21 = a23 = a24 = 0.0f;
        a31 = a32 = a34 = 0.0f;
        a41 = a42 = a43 = 0.0f;
        a11 = a22 = a33 = a44 = 1.0f;
    }

    Mat4f(const Vec3f &right, const Vec3f &up, const Vec3f &fwd)
            : a11(right.x()), a12(up.x()), a13(fwd.x()), a14(0.0f),
              a21(right.y()), a22(up.y()), a23(fwd.y()), a24(0.0f),
              a31(right.z()), a32(up.z()), a33(fwd.z()), a34(0.0f),
              a41(0.0f), a42(0.0f), a43(0.0f), a44(1.0f) {
    }

    Mat4f(
            float _a11, float _a12, float _a13, float _a14,
            float _a21, float _a22, float _a23, float _a24,
            float _a31, float _a32, float _a33, float _a34,
            float _a41, float _a42, float _a43, float _a44)
            : a11(_a11), a12(_a12), a13(_a13), a14(_a14),
              a21(_a21), a22(_a22), a23(_a23), a24(_a24),
              a31(_a31), a32(_a32), a33(_a33), a34(_a34),
              a41(_a41), a42(_a42), a43(_a43), a44(_a44) {
    }

    [[nodiscard]] Mat4f transpose() const {
        return {
                a11, a21, a31, a41,
                a12, a22, a32, a42,
                a13, a23, a33, a43,
                a14, a24, a34, a44
        };
    }

    [[nodiscard]] Mat4f pseudoInvert() const {
        Mat4f trans = translate(Vec3f(-a14, -a24, -a34));
        Mat4f rot = transpose();
        rot.a41 = rot.a42 = rot.a43 = 0.0f;

        return rot * trans;
    }

    [[nodiscard]] Mat4f invert() const {
        Mat4f inv;
        inv[0] = a[5] * a[10] * a[15] - a[5] * a[11] * a[14] - a[9] * a[6] * a[15] + a[9] * a[7] * a[14] +
                 a[13] * a[6] * a[11] - a[13] * a[7] * a[10];
        inv[1] = -a[1] * a[10] * a[15] + a[1] * a[11] * a[14] + a[9] * a[2] * a[15] - a[9] * a[3] * a[14] -
                 a[13] * a[2] * a[11] + a[13] * a[3] * a[10];
        inv[2] = a[1] * a[6] * a[15] - a[1] * a[7] * a[14] - a[5] * a[2] * a[15] + a[5] * a[3] * a[14] +
                 a[13] * a[2] * a[7] - a[13] * a[3] * a[6];
        inv[3] = -a[1] * a[6] * a[11] + a[1] * a[7] * a[10] + a[5] * a[2] * a[11] - a[5] * a[3] * a[10] -
                 a[9] * a[2] * a[7] + a[9] * a[3] * a[6];
        inv[4] = -a[4] * a[10] * a[15] + a[4] * a[11] * a[14] + a[8] * a[6] * a[15] - a[8] * a[7] * a[14] -
                 a[12] * a[6] * a[11] + a[12] * a[7] * a[10];
        inv[5] = a[0] * a[10] * a[15] - a[0] * a[11] * a[14] - a[8] * a[2] * a[15] + a[8] * a[3] * a[14] +
                 a[12] * a[2] * a[11] - a[12] * a[3] * a[10];
        inv[6] = -a[0] * a[6] * a[15] + a[0] * a[7] * a[14] + a[4] * a[2] * a[15] - a[4] * a[3] * a[14] -
                 a[12] * a[2] * a[7] + a[12] * a[3] * a[6];
        inv[8] = a[4] * a[9] * a[15] - a[4] * a[11] * a[13] - a[8] * a[5] * a[15] + a[8] * a[7] * a[13] +
                 a[12] * a[5] * a[11] - a[12] * a[7] * a[9];
        inv[7] = a[0] * a[6] * a[11] - a[0] * a[7] * a[10] - a[4] * a[2] * a[11] + a[4] * a[3] * a[10] +
                 a[8] * a[2] * a[7] - a[8] * a[3] * a[6];
        inv[9] = -a[0] * a[9] * a[15] + a[0] * a[11] * a[13] + a[8] * a[1] * a[15] - a[8] * a[3] * a[13] -
                 a[12] * a[1] * a[11] + a[12] * a[3] * a[9];
        inv[10] = a[0] * a[5] * a[15] - a[0] * a[7] * a[13] - a[4] * a[1] * a[15] + a[4] * a[3] * a[13] +
                  a[12] * a[1] * a[7] - a[12] * a[3] * a[5];
        inv[11] = -a[0] * a[5] * a[11] + a[0] * a[7] * a[9] + a[4] * a[1] * a[11] - a[4] * a[3] * a[9] -
                  a[8] * a[1] * a[7] + a[8] * a[3] * a[5];
        inv[12] = -a[4] * a[9] * a[14] + a[4] * a[10] * a[13] + a[8] * a[5] * a[14] - a[8] * a[6] * a[13] -
                  a[12] * a[5] * a[10] + a[12] * a[6] * a[9];
        inv[13] = a[0] * a[9] * a[14] - a[0] * a[10] * a[13] - a[8] * a[1] * a[14] + a[8] * a[2] * a[13] +
                  a[12] * a[1] * a[10] - a[12] * a[2] * a[9];
        inv[14] = -a[0] * a[5] * a[14] + a[0] * a[6] * a[13] + a[4] * a[1] * a[14] - a[4] * a[2] * a[13] -
                  a[12] * a[1] * a[6] + a[12] * a[2] * a[5];
        inv[15] = a[0] * a[5] * a[10] - a[0] * a[6] * a[9] - a[4] * a[1] * a[10] + a[4] * a[2] * a[9] +
                  a[8] * a[1] * a[6] - a[8] * a[2] * a[5];

        float det = a[0] * inv[0] + a[1] * inv[4] + a[2] * inv[8] + a[3] * inv[12];
        if (det == 0.0f)
            return {};

        float invDet = 1.0f / det;
        return inv * invDet;
    }

    [[nodiscard]] Vec3f right() const {
        return {a11, a21, a31};
    }

    [[nodiscard]] Vec3f up() const {
        return {a12, a22, a32};
    }

    [[nodiscard]] Vec3f fwd() const {
        return {a13, a23, a33};
    }

    void setRight(const Vec3f &x) {
        a11 = x.x();
        a21 = x.y();
        a31 = x.z();
    }

    void setUp(const Vec3f &y) {
        a12 = y.x();
        a22 = y.y();
        a32 = y.z();
    }

    void setFwd(const Vec3f &z) {
        a13 = z.x();
        a23 = z.y();
        a33 = z.z();
    }

    float operator()(int i, int j) const {
        return a[i * 4 + j];
    }

    float operator[](int i) const {
        return a[i];
    }

    float &operator[](int i) {
        return a[i];
    }

    [[nodiscard]] const float *data() const {
        return a;
    }

    [[nodiscard]] Vec3f transformVector(const Vec3f &b) const {
        return {
                a11 * b.x() + a12 * b.y() + a13 * b.z(),
                a21 * b.x() + a22 * b.y() + a23 * b.z(),
                a31 * b.x() + a32 * b.y() + a33 * b.z()
        };
    }

    Mat4f operator-(const Mat4f &o) const {
        Mat4f tmp(*this);
        for (int i = 0; i < 16; ++i)
            tmp[i] -= o[i];
        return tmp;
    }

    Mat4f operator+(const Mat4f &o) const {
        Mat4f tmp(*this);
        for (int i = 0; i < 16; ++i)
            tmp[i] += o[i];
        return tmp;
    }

    Mat4f operator-(float o) const {
        Mat4f tmp(*this);
        for (int i = 0; i < 16; ++i)
            tmp[i] -= o;
        return tmp;
    }

    Mat4f operator+(float o) const {
        Mat4f tmp(*this);
        for (int i = 0; i < 16; ++i)
            tmp[i] += o;
        return tmp;
    }

    Mat4f &operator-=(const Mat4f &o) {
        for (int i = 0; i < 16; ++i)
            a[i] -= o[i];
        return *this;
    }

    Mat4f &operator+=(const Mat4f &o) {
        for (int i = 0; i < 16; ++i)
            a[i] += o[i];
        return *this;
    }

    Mat4f &operator-=(float o) {
        for (float &i: a)
            i -= o;
        return *this;
    }

    Mat4f &operator+=(float o) {
        for (float &i: a)
            i += o;
        return *this;
    }

    Mat4f operator*(float o) const {
        Mat4f tmp(*this);
        for (int i = 0; i < 16; ++i)
            tmp[i] *= o;
        return tmp;
    }

    Mat4f operator/(float o) const {
        Mat4f tmp(*this);
        for (int i = 0; i < 16; ++i)
            tmp[i] /= o;
        return tmp;
    }

    Mat4f &operator*=(float o) {
        for (float &i: a)
            i *= o;
        return *this;
    }

    Mat4f &operator/=(float o) {
        for (float &i: a)
            i /= o;
        return *this;
    }

    Mat4f toNormalMatrix() const;

    Vec3f extractRotationVec() const;

    Mat4f extractRotation() const;

    Vec3f extractTranslationVec() const;

    Mat4f extractTranslation() const;

    Vec3f extractScaleVec() const;

    Mat4f extractScale() const;

    Mat4f stripRotation() const;

    Mat4f stripTranslation() const;

    Mat4f stripScale() const;

    static Mat4f translate(const Vec3f &v);

    static Mat4f scale(const Vec3f &s);

    static Mat4f rotXYZ(const Vec3f &rot) {
        Vec3f r = rot * PI / 180.0f;
        float c[] = {std::cos(r.x()), std::cos(r.y()), std::cos(r.z())};
        float s[] = {std::sin(r.x()), std::sin(r.y()), std::sin(r.z())};

        return {
                c[1] * c[2], -c[0] * s[2] + s[0] * s[1] * c[2], s[0] * s[2] + c[0] * s[1] * c[2], 0.0f,
                c[1] * s[2], c[0] * c[2] + s[0] * s[1] * s[2], -s[0] * c[2] + c[0] * s[1] * s[2], 0.0f,
                -s[1], s[0] * c[1], c[0] * c[1], 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
        };
    }

    static Mat4f rotYXZ(const Vec3f &rot) {
        Vec3f r = rot * PI / 180.0f;
        float c[] = {std::cos(r.x()), std::cos(r.y()), std::cos(r.z())};
        float s[] = {std::sin(r.x()), std::sin(r.y()), std::sin(r.z())};

        return {
                c[1] * c[2] - s[1] * s[0] * s[2], -c[1] * s[2] - s[1] * s[0] * c[2], -s[1] * c[0], 0.0f,
                c[0] * s[2], c[0] * c[2], -s[0], 0.0f,
                s[1] * c[2] + c[1] * s[0] * s[2], -s[1] * s[2] + c[1] * s[0] * c[2], c[1] * c[0], 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
        };
    }

    static Mat4f rotAxis(const Vec3f &axis, float angle);

    static Mat4f ortho(float l, float r, float b, float t, float near, float far);

    static Mat4f perspective(float aov, float ratio, float near, float far);

    static Mat4f lookAt(const Vec3f &pos, const Vec3f &fwd, const Vec3f &up) {
        Vec3f f = fwd.normalized();
        Vec3f r = f.cross(up).normalized();
        Vec3f u = r.cross(f).normalized();

        return {
                r.x(), u.x(), f.x(), pos.x(),
                r.y(), u.y(), f.y(), pos.y(),
                r.z(), u.z(), f.z(), pos.z(),
                0.0f, 0.0f, 0.0f, 1.0f
        };
    }

    friend Mat4f operator*(const Mat4f &a, const Mat4f &b);

    friend Vec4f operator*(const Mat4f &a, const Vec4f &b);

    friend Vec3f operator*(const Mat4f &a, const Vec3f &b);

    friend std::ostream &operator<<(std::ostream &stream, const Mat4f &m) {
        for (int i = 0; i < 4; ++i) {
            stream << '[';
            for (uint32 j = 0; j < 4; ++j)
                stream << m[i * 4 + j] << (j == 3 ? ']' : ',');
            if (i < 3)
                stream << std::endl;
        }
        return stream;
    }
};

static inline Mat4f operator*(const Mat4f &a, const Mat4f &b) {
    Mat4f result;
    for (int i = 0; i < 4; i++)
        for (int t = 0; t < 4; t++)
            result[i * 4 + t] =
                    a[i * 4 + 0] * b[0 * 4 + t] +
                    a[i * 4 + 1] * b[1 * 4 + t] +
                    a[i * 4 + 2] * b[2 * 4 + t] +
                    a[i * 4 + 3] * b[3 * 4 + t];

    return result;
}

static inline Vec4f operator*(const Mat4f &a, const Vec4f &b) {
    return {
            a.a11 * b.x() + a.a12 * b.y() + a.a13 * b.z() + a.a14 * b.w(),
            a.a21 * b.x() + a.a22 * b.y() + a.a23 * b.z() + a.a24 * b.w(),
            a.a31 * b.x() + a.a32 * b.y() + a.a33 * b.z() + a.a34 * b.w(),
            a.a41 * b.x() + a.a42 * b.y() + a.a43 * b.z() + a.a44 * b.w()
    };
}

static inline Vec3f operator*(const Mat4f &a, const Vec3f &b) {
    return {
            a.a11 * b.x() + a.a12 * b.y() + a.a13 * b.z() + a.a14,
            a.a21 * b.x() + a.a22 * b.y() + a.a23 * b.z() + a.a24,
            a.a31 * b.x() + a.a32 * b.y() + a.a33 * b.z() + a.a34
    };
}

#endif
