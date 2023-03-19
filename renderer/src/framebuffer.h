#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "usings.h"

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
            color(vector<Vec3f>(resx * resy)),
            spp(spp),
            useOidn(useOidn) {
        if (useOidn) {
            albedo = vector<Vec3f>(resx * resy);
            normal = vector<Vec3f>(resx * resy);
            oidn = vector<Vec3f>(resx * resy);
        }
    };

    void setSpp(int spp_) { spp = spp_; }

    void enableOidn() {
        useOidn = true;
        albedo = vector<Vec3f>(resx * resy);
        normal = vector<Vec3f>(resx * resy);
        oidn = vector<Vec3f>(resx * resy);
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
    vector<Vec3f> color;
    vector<Vec3f> albedo;
    vector<Vec3f> normal;
    vector<Vec3f> oidn;
};

#endif
