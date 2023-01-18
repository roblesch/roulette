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

void FrameBuffer::toPng(const char *filename) {
    rgbBuf = vector<Vec3c>(resx * resy);
    for (int i = 0; i < buf.size(); i++) {
        rgbBuf[i] = Rgb(i);
    }
    stbi_write_png(filename, resx, resy, 3, rgbBuf.data(), resx * 3);
}
