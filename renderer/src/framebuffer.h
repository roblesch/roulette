#pragma once

#include "usings.h"

class FrameBuffer {
public:
    FrameBuffer() = default;

    FrameBuffer(int resx, int resy) :
            resx(resx),
            resy(resy),
            buf(vector<vec3f>(resx * resy)) {};

    void set(int px, vec3f v) { buf[px] = v; }

    void set(vec2i px, vec3f v) { set(px.y * resx + px.x, v); }

    vec3f get(int px) { return buf[px]; }

    vec3f get(vec2i px) { return get(px.y * resx + px.x); }

    vec3c Rgb(int px);

    vec3c Rgb(vec2i px) { return Rgb(px.y * resx + px.x); }

    vec4c Rgba(int px);

    vec4c Rgba(vec2i px) { return Rgba(px.y * resx + px.x); }

    void toPpm(const char *filename);

    void toPng(const char *filename);

    int resx{};
    int resy{};
    vector<vec3f> buf;
    vector<vec4c> rgbBuf;
};
