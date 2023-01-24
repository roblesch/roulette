#include "framebuffer.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image_write.h"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

Vec3c FrameBuffer::Rgb(int px) {
    Vec3f pixel = get(px);
    return {
            static_cast<uint8>(255.0f * clamp(pixel.r())),
            static_cast<uint8>(255.0f * clamp(pixel.g())),
            static_cast<uint8>(255.0f * clamp(pixel.b()))
    };
}

Vec4c FrameBuffer::Rgba(int px) {
    Vec3f pixel = get(px);
    return {
            static_cast<uint8>(255.0f * clamp(pixel.r())),
            static_cast<uint8>(255.0f * clamp(pixel.g())),
            static_cast<uint8>(255.0f * clamp(pixel.b())),
            255
    };
}

void FrameBuffer::toPpm(const char *filename) {
    std::ofstream out(filename);
    out << "P3\n" << resx << ' ' << resy << "\n255\n";
    for (int j = 0; j < resy; j++) {
        for (int i = 0; i < resx; i++) {
            Vec3c px = Rgb(Vec2i(i, j));
            out << px.r() << ' '
                << px.g() << ' '
                << px.b() << '\n';
        }
    }
}

Vec3f v3fmax(Vec3f a, Vec3f b) {
    float x = a.x() > b.x() ? a.x() : b.x();
    float y = a.y() > b.y() ? a.y() : b.y();
    float z = a.z() > b.z() ? a.z() : b.z();
    return {
        x,
        y,
        z
    };
}

void FrameBuffer::toPng(const char *filename) {
    vector<Vec3c> ldr(resx*resy);
    for (int i = 0; i < buf.size(); i++) {
        Vec3f a = v3fmax(buf[i], Vec3f(0.0f));
        Vec3f x = v3fmax(a - 0.004f, Vec3f(0.0f));
        Vec3f tonemap = (x*(6.2f*x + 0.5f))/(x*(6.2f*x + 1.7f) + 0.06f);
        ldr[i] = Vec3c(255.0f * clamp(tonemap.x()), 255.0f * clamp(tonemap.y()), 255.0f * clamp(tonemap.z()));
    }
    stbi_write_png(filename, resx, resy, 3, ldr.data(), resx * 3);
//    rgbBuf = vector<Vec3c>(resx * resy);
//    for (int i = 0; i < buf.size(); i++) {
//        rgbBuf[i] = Rgb(i);
//    }
//    stbi_write_png(filename, resx, resy, 3, rgbBuf.data(), resx * 3);
}
