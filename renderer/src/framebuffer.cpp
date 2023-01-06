#include "framebuffer.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

vec3c FrameBuffer::Rgb(int px) {
    vec3f pixel = get(px);
    return {
            static_cast<char>(256 * clamp(pixel.r)),
            static_cast<char>(256 * clamp(pixel.g)),
            static_cast<char>(256 * clamp(pixel.b))
    };
}

vec4c FrameBuffer::Rgba(int px) {
    vec3f pixel = get(px);
    return {
            static_cast<char>(256 * clamp(pixel.r)),
            static_cast<char>(256 * clamp(pixel.g)),
            static_cast<char>(256 * clamp(pixel.b)),
            255
    };
}

void FrameBuffer::toPpm(const char *filename) {
    std::ofstream out(filename);
    out << "P3\n" << resx << ' ' << resy << "\n255\n";
    for (int j = 0; j < resy; j++) {
        for (int i = 0; i < resx; i++) {
            vec3f px = Rgb(vec2i(i, j));
            out << px.r << ' '
                << px.g << ' '
                << px.b << '\n';
        }
    }
}

void FrameBuffer::toPng(const char *filename) {
    rgbBuf = vector<vec4c>(resx * resy);
    for (int i = 0; i < buf.size(); i++) {
        rgbBuf[i] = Rgba(i);
    }
    stbi_write_png(filename, resx, resy, 4, rgbBuf.data(), resx * 4);
}
