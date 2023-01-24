#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "usings.h"

class FrameBuffer {
public:
    FrameBuffer() = default;

    FrameBuffer(int resx, int resy) :
            resx(resx),
            resy(resy),
            buf(vector<Vec3f>(resx * resy)) {};

    void set(int px, Vec3f v) { buf[px] = v; }

    void set(Vec2i px, Vec3f v) { set(px.y() * resx + px.x(), v); }

    Vec3f get(int px) { return buf[px]; }

    Vec3f get(Vec2i px) { return get(px.y() * resx + px.x()); }

    Vec3c Rgb(int px);

    Vec3c Rgb(Vec2i px) { return Rgb(px.y() * resx + px.x()); }

    Vec4c Rgba(int px);

    Vec4c Rgba(Vec2i px) { return Rgba(px.y() * resx + px.x()); }

    inline Vec3f getLinear(int x, int y);

    void toPpm(const char *filename);

    void toPng(const char *filename);

    int resx{};
    int resy{};
    vector<Vec3f> buf;
    vector<float> luminance;
    vector<Vec3c> rgbBuf;
};

#endif
