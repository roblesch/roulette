#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "usings.h"

inline std::vector<Vec3c> tonemap(std::vector<Vec3f> hdr) {
    std::vector<Vec3c> ldr(hdr.size());
    for (int i = 0; i < ldr.size(); i++) {
        Vec3f a = v3fmax(hdr[i], Vec3f(0.0f));
        Vec3f x = v3fmax(a - 0.004f, Vec3f(0.0f));
        Vec3f tonemap = (x * (6.2f * x + 0.5f)) / (x * (6.2f * x + 1.7f) + 0.06f);
        ldr[i] = Vec3c(255.0f * clamp(tonemap.x()), 255.0f * clamp(tonemap.y()), 255.0f * clamp(tonemap.z()));
    }
    return ldr;
}

class Film {
public:
    Film() = default;

    Film(int resx, int resy) :
        resx(resx),
        resy(resy),
        buffer(std::vector<Vec3f>(resx * resy))
    {};

    Film(Vec2i size) :
        Film(size.x(), size.y())
    {};

    Vec3f get(int px) {
        return buffer[px];
    }

    Vec3f get(Vec2i px) {
        return get(px.y() * resx + px.x());
    }

    void add(int px, Vec3f v) {
        buffer[px] = v;
    }

    void add(Vec2i px, Vec3f v) {
        add(px.y() * resx + px.x(), v);
    }

    void clear() {
        buffer = std::vector<Vec3f>(resx * resy);
    }

    Vec3f* data() {
        return buffer.data();
    }

    Vec2i size() const {
        return {
            resx,
            resy
        };
    }

    int resx, resy;
    std::vector<Vec3f> buffer;
};

class FrameBuffer {
public:
    enum buffer {
        COLOR,
        ALBEDO,
        NORMAL,
        OIDN
    };

    FrameBuffer() = default;

    FrameBuffer(int resx, int resy, int spp=1, bool useOidn=false) :
            resx(resx),
            resy(resy),
            color(std::vector<Vec3f>(resx * resy)),
            spp(spp),
            useOidn(useOidn) {
        if (useOidn) {
            albedo = std::vector<Vec3f>(resx * resy);
            normal = std::vector<Vec3f>(resx * resy);
            oidn = std::vector<Vec3f>(resx * resy);
        }
    };

    void incSpp() { spp += 1; }

    void setSpp(int spp_) { spp = spp_; }

    void enableOidn() {
        useOidn = true;
        albedo = std::vector<Vec3f>(resx * resy);
        normal = std::vector<Vec3f>(resx * resy);
        oidn = std::vector<Vec3f>(resx * resy);
    }

    void add(int px, Vec3f v, buffer b=COLOR) {
        switch (b) {
        case COLOR:
            color[px] += v;
            break;
        case ALBEDO:
            albedo[px] += v;
            break;
        case NORMAL:
            normal[px] += v;
            break;
        }
    }

    void add(Vec2i px, Vec3f v, buffer b=COLOR) { add(px.y() * resx + px.x(), v, b); }

    void set(int px, Vec3f v, buffer b=COLOR) {
        switch(b) {
        case COLOR:
            color[px] = v;
            break;
        case ALBEDO:
            albedo[px] = v;
            break;
        case NORMAL:
            normal[px] = v;
            break;
        }
    }

    void set(Vec2i px, Vec3f v, buffer b=COLOR) { set(px.y() * resx + px.x(), v, b); }

    Vec3f get(int px, buffer b=COLOR) {
        switch(b) {
        case COLOR:
            return color[px];
        case ALBEDO:
            return albedo[px];
        case NORMAL:
            return normal[px];
        case OIDN:
            return oidn[px];
        }
    }

    Vec3f get(Vec2i px, buffer b=COLOR) { return get(px.y() * resx + px.x(), b); }

    void normalize(buffer b = COLOR);

    std::vector<Vec3c> tonemap(std::vector<Vec3f> hdr);

    std::vector<Vec3c> tonemap(buffer b) {
        switch (b) {
        case COLOR:
            return tonemap(color);
        case ALBEDO:
            return tonemap(albedo);
        case NORMAL:
            return tonemap(normal);
        case OIDN:
            return tonemap(oidn);
        }
    }

    void toPng(const char *filename, buffer b=COLOR);

    bool useOidn;
    int resx{};
    int resy{};
    int spp;
    std::vector<Vec3f> color;
    std::vector<Vec3f> albedo;
    std::vector<Vec3f> normal;
    std::vector<Vec3f> oidn;
};

#endif
