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
            sampler->startPath(i + j, 0xFFFFFFFF);
            frame.set(px, tracer->trace(px, *sampler));
        }
        std::cout << "Completed row " << j << "\r";
    }
}

void OIDNIntegrator::render(const Scene& scene, FrameBuffer& frame) {
    Camera cam = scene.camera;
    int resx = cam.resx;
    int resy = cam.resy;
    auto albedoTracer = make_unique<AlbedoTracer>(scene);
    auto normalTracer = make_unique<NormalTracer>(scene);
    FrameBuffer albedoBuffer(resx, resy);
    FrameBuffer normalBuffer(resx, resy);
    tracer = make_unique<PathTracer>(scene);

    for (int j = 0; j < resy; j++) {
        for (int i = 0; i < resx; i++) {
            Vec2i px(i, j);
            sampler->startPath(i + j, 0xFFFFFFFF);
            Vec3f a(0.0f);
            Vec3f n(0.0f);
            Vec3f c(0.0f);
            int spp = 16;
            for (int i = 0; i < spp; i++) {
                a += albedoTracer->trace(px, *sampler);
                n += normalTracer->trace(px, *sampler);
                c += tracer->trace(px, *sampler);
            }
            albedoBuffer.set(px, a/(float)spp);
            normalBuffer.set(px, n/(float)spp);
            frame.set(px, c/(float)spp);
        }
        std::cout << "Completed row " << j << "\r";
    }

    albedoBuffer.toPng("albedo_img.png");
    normalBuffer.toPng("normal_img.png");

    FrameBuffer OIDNBuffer(resx, resy);

    OIDNDevice device = oidnNewDevice(OIDN_DEVICE_TYPE_DEFAULT);
    oidnCommitDevice(device);
    OIDNFilter filter = oidnNewFilter(device, "RT");
    oidnSetSharedFilterImage(filter, "color", frame.buf.data(), OIDN_FORMAT_FLOAT3, resx, resy, 0, 0, 0);
    oidnSetSharedFilterImage(filter, "albedo", albedoBuffer.buf.data(), OIDN_FORMAT_FLOAT3, resx, resy, 0, 0, 0);
    oidnSetSharedFilterImage(filter, "normal", normalBuffer.buf.data(), OIDN_FORMAT_FLOAT3, resx, resy, 0, 0, 0);
    oidnSetSharedFilterImage(filter, "output", OIDNBuffer.buf.data(), OIDN_FORMAT_FLOAT3, resx, resy, 0, 0, 0);
    oidnSetFilter1b(filter, "hdr", true);
    oidnCommitFilter(filter);

    oidnExecuteFilter(filter);

    const char* errorMessage;
    if (oidnGetDeviceError(device, &errorMessage) != OIDN_ERROR_NONE)
        printf("Error: %s\n", errorMessage);

    // Cleanup
    oidnReleaseFilter(filter);
    oidnReleaseDevice(device);

    OIDNBuffer.toPng("oidn_img.png");
}

void EARSIntegrator::render(const Scene& scene, FrameBuffer& frame) {
    Camera cam = scene.camera;
    int resx = cam.resx;
    int resy = cam.resy;
    tracer = make_unique<EARSTracer>(scene);
    auto albedoTracer = make_unique<AlbedoTracer>(scene);
    auto normalTracer = make_unique<NormalTracer>(scene);
    FrameBuffer albedoBuffer(resx, resy);
    FrameBuffer normalBuffer(resx, resy);

    for (int j = 0; j < resy; j++) {
        for (int i = 0; i < resx; i++) {
            Vec2i px(i, j);
            sampler->startPath(i + j, 0xFFFFFFFF);
            Vec3f a(0.0f);
            Vec3f n(0.0f);
            Vec3f c(0.0f);
            int spp = 2;
            for (int i = 0; i < spp; i++) {
                a += albedoTracer->trace(px, *sampler);
                n += normalTracer->trace(px, *sampler);
                c += tracer->trace(px, *sampler);
            }
            albedoBuffer.set(px, a / (float)spp);
            normalBuffer.set(px, n / (float)spp);
            frame.set(px, c / (float)spp);
        }
        std::cout << "Completed row " << j << "\r";
    }

    albedoBuffer.toPng("albedo_img.png");
    normalBuffer.toPng("normal_img.png");

    FrameBuffer OIDNBuffer(resx, resy);

    OIDNDevice device = oidnNewDevice(OIDN_DEVICE_TYPE_DEFAULT);
    oidnCommitDevice(device);
    OIDNFilter filter = oidnNewFilter(device, "RT");
    oidnSetSharedFilterImage(filter, "color", frame.buf.data(), OIDN_FORMAT_FLOAT3, resx, resy, 0, 0, 0);
    oidnSetSharedFilterImage(filter, "albedo", albedoBuffer.buf.data(), OIDN_FORMAT_FLOAT3, resx, resy, 0, 0, 0);
    oidnSetSharedFilterImage(filter, "normal", normalBuffer.buf.data(), OIDN_FORMAT_FLOAT3, resx, resy, 0, 0, 0);
    oidnSetSharedFilterImage(filter, "output", OIDNBuffer.buf.data(), OIDN_FORMAT_FLOAT3, resx, resy, 0, 0, 0);
    oidnSetFilter1b(filter, "hdr", true);
    oidnCommitFilter(filter);

    oidnExecuteFilter(filter);

    const char* errorMessage;
    if (oidnGetDeviceError(device, &errorMessage) != OIDN_ERROR_NONE)
        printf("Error: %s\n", errorMessage);

    // Cleanup
    oidnReleaseFilter(filter);
    oidnReleaseDevice(device);

    OIDNBuffer.toPng("oidn_img.png");
}
