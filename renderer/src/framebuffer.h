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

    Vec3c Rgb(int px, buffer b=COLOR) {
        Vec3f pixel = get(px, b);
        return {
            static_cast<uint8>(255.0f * clamp(pixel.r())),
            static_cast<uint8>(255.0f * clamp(pixel.g())),
            static_cast<uint8>(255.0f * clamp(pixel.b()))
        };
    }

    Vec3c Rgb(Vec2i px, buffer b=COLOR) { return Rgb(px.y() * resx + px.x(), b); }

    Vec4c Rgba(int px) {
        Vec3f pixel = get(px);
        return {
            static_cast<uint8>(255.0f * clamp(pixel.r())),
            static_cast<uint8>(255.0f * clamp(pixel.g())),
            static_cast<uint8>(255.0f * clamp(pixel.b())),
            255
        };
    }

    Vec4c Rgba(Vec2i px) { return Rgba(px.y() * resx + px.x()); }

    void normalize(buffer b=COLOR) {
        for (int i = 0; i < resx*resy; i++) {
            Vec3f v = get(i, b) / (float) spp;
            set(i, v, b);
        }
    }

    void toPpm(const char *filename);

    void toPng(const char *filename, buffer b=COLOR);

    bool useOidn;
    int resx{};
    int resy{};
    vector<Vec3f> color;
    int spp;
    // oidn
    vector<Vec3f> albedo;
    vector<Vec3f> normal;
    vector<Vec3f> oidn;
};

#endif
