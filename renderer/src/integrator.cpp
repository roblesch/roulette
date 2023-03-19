#include "integrator.h"

#include <iostream>

#include <OpenImageDenoise/oidn.h>

void RayCastIntegrator::render(const Scene &scene, FrameBuffer &frame) {
    Camera cam = scene.camera;
    int resx = cam.resx;
    int resy = cam.resy;
    tracer = make_unique<IntersectionDebugTracer>(scene);
    for (int j = 0; j < resy; j++) {
        for (int i = 0; i < resx; i++) {
            Vec2i px(i, j);
            frame.set(px, tracer->trace(px, *sampler));
        }
    }
}

void PathTraceIntegrator::render(const Scene &scene, FrameBuffer &frame) {
    Camera cam = scene.camera;
    int resx = cam.resx;
    int resy = cam.resy;
    tracer = make_unique<PathTracer>(scene);
    for (int j = 0; j < resy; j++) {
        for (int i = 0; i < resx; i++) {
            Vec2i px(i, j);
            sampler->startPath(i + j, 0xFFFF);
            frame.set(px, tracer->trace(px, *sampler));
        }
        std::cout << "Completed row " << j << "\r";
    }
}

void OIDNIntegrator::render(const Scene& scene, FrameBuffer& frame) {
    Camera cam = scene.camera;
    int resx = cam.resx;
    int resy = cam.resy;
    tracer = make_unique<PathTracer>(scene);
    auto albedoTracer = make_unique<AlbedoTracer>(scene);
    auto normalTracer = make_unique<NormalTracer>(scene);
    frame.enableOidn();

    for (int j = 0; j < resy; j++) {
        for (int i = 0; i < resx; i++) {
            Vec2i px(i, j);
            sampler->startPath(i + j, 0xFFFF);
            for (int i = 0; i < frame.spp; i++) {
                frame.add(px, albedoTracer->trace(px, *sampler), FrameBuffer::ALBEDO);
                frame.add(px, normalTracer->trace(px, *sampler), FrameBuffer::NORMAL);
                frame.add(px, tracer->trace(px, *sampler), FrameBuffer::COLOR);
            }
        }
        std::cout << "Completed row " << j << std::endl;
    }

    frame.normalize(FrameBuffer::ALBEDO);
    frame.normalize(FrameBuffer::NORMAL);
    frame.normalize(FrameBuffer::COLOR);

    frame.toPng("albedo_img.png", FrameBuffer::ALBEDO);
    frame.toPng("normal_img.png", FrameBuffer::NORMAL);

    OIDNDevice device = oidnNewDevice(OIDN_DEVICE_TYPE_DEFAULT);
    oidnCommitDevice(device);
    OIDNFilter filter = oidnNewFilter(device, "RT");
    oidnSetSharedFilterImage(filter, "color", frame.color.data(), OIDN_FORMAT_FLOAT3, resx, resy, 0, 0, 0);
    oidnSetSharedFilterImage(filter, "albedo", frame.albedo.data(), OIDN_FORMAT_FLOAT3, resx, resy, 0, 0, 0);
    oidnSetSharedFilterImage(filter, "normal", frame.normal.data(), OIDN_FORMAT_FLOAT3, resx, resy, 0, 0, 0);
    oidnSetSharedFilterImage(filter, "output", frame.oidn.data(), OIDN_FORMAT_FLOAT3, resx, resy, 0, 0, 0);
    oidnSetFilter1b(filter, "hdr", true);
    oidnCommitFilter(filter);

    oidnExecuteFilter(filter);

    const char* errorMessage;
    if (oidnGetDeviceError(device, &errorMessage) != OIDN_ERROR_NONE)
        printf("Error: %s\n", errorMessage);

    // Cleanup
    oidnReleaseFilter(filter);
    oidnReleaseDevice(device);

    frame.toPng("oidn_img.png", FrameBuffer::OIDN);
}

void EARSIntegrator::render(const Scene& scene, FrameBuffer& frame) {
    Camera cam = scene.camera;
    int resx = cam.resx;
    int resy = cam.resy;
    tracer = make_unique<EARSTracer>(scene);
    auto albedoTracer = make_unique<AlbedoTracer>(scene);
    auto normalTracer = make_unique<NormalTracer>(scene);
    frame.enableOidn();

    for (int j = 0; j < resy; j++) {
        for (int i = 0; i < resx; i++) {
            Vec2i px(i, j);
            sampler->startPath(i + j, 0xFFFF);
            for (int i = 0; i < frame.spp; i++) {
                frame.add(px, albedoTracer->trace(px, *sampler), FrameBuffer::ALBEDO);
                frame.add(px, normalTracer->trace(px, *sampler), FrameBuffer::NORMAL);
                frame.add(px, tracer->trace(px, *sampler), FrameBuffer::COLOR);
            }
        }
        std::cout << "Completed row " << j << std::endl;
    }

    frame.normalize(FrameBuffer::ALBEDO);
    frame.normalize(FrameBuffer::NORMAL);
    frame.normalize(FrameBuffer::COLOR);

    frame.toPng("albedo_img.png", FrameBuffer::ALBEDO);
    frame.toPng("normal_img.png", FrameBuffer::NORMAL);

    OIDNDevice device = oidnNewDevice(OIDN_DEVICE_TYPE_DEFAULT);
    oidnCommitDevice(device);
    OIDNFilter filter = oidnNewFilter(device, "RT");
    oidnSetSharedFilterImage(filter, "color", frame.color.data(), OIDN_FORMAT_FLOAT3, resx, resy, 0, 0, 0);
    oidnSetSharedFilterImage(filter, "albedo", frame.albedo.data(), OIDN_FORMAT_FLOAT3, resx, resy, 0, 0, 0);
    oidnSetSharedFilterImage(filter, "normal", frame.normal.data(), OIDN_FORMAT_FLOAT3, resx, resy, 0, 0, 0);
    oidnSetSharedFilterImage(filter, "output", frame.oidn.data(), OIDN_FORMAT_FLOAT3, resx, resy, 0, 0, 0);
    oidnSetFilter1b(filter, "hdr", true);
    oidnCommitFilter(filter);

    oidnExecuteFilter(filter);

    const char* errorMessage;
    if (oidnGetDeviceError(device, &errorMessage) != OIDN_ERROR_NONE)
        printf("Error: %s\n", errorMessage);

    // Cleanup
    oidnReleaseFilter(filter);
    oidnReleaseDevice(device);

    frame.toPng("oidn_img.png", FrameBuffer::OIDN);
}
