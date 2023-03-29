#include "framebuffer.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void FrameBuffer::normalize(buffer b) {
    for (int i = 0; i < resx * resy; i++) {
        Vec3f v = get(i, b) / (float)spp;
        set(i, v, b);
    }
}

std::vector<Vec3c> FrameBuffer::tonemap(std::vector<Vec3f> hdr) {
    std::vector<Vec3c> ldr(resx * resy);
    for (int i = 0; i < ldr.size(); i++) {
        Vec3f a = v3fmax(hdr[i], Vec3f(0.0f));
        Vec3f x = v3fmax(a - 0.004f, Vec3f(0.0f));
        Vec3f tonemap = (x * (6.2f * x + 0.5f)) / (x * (6.2f * x + 1.7f) + 0.06f);
        ldr[i] = Vec3c(255.0f * clamp(tonemap.x()), 255.0f * clamp(tonemap.y()), 255.0f * clamp(tonemap.z()));
    }
    return ldr;
}

void FrameBuffer::toPng(const char *filename, buffer b) {
    std::vector<Vec3c> ldr = tonemap(b);
    stbi_write_png(filename, resx, resy, 3, ldr.data(), resx * 3);
}
