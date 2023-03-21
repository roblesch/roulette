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
            sampler->startPath(i + j, 0xFFFF);
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
        std::cout << "Completed row " << j << "\r";
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

static float computeElapsedSeconds(std::chrono::steady_clock::time_point start) {
    auto current = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(current - start);
    return (float)ms.count() / 1000;
}

void EARSIntegrator::render(const Scene& scene, FrameBuffer& frame) {
    Camera cam = scene.camera;
    int resx = cam.resx;
    int resy = cam.resy;
    frame.setSpp(1);

    // EARS configuration
    EARSTracer EARStracer(scene);
    EARStracer.rrs = EARS::RRSMethod::Classic();
    EARStracer.rrs.technique = EARS::RRSMethod::EClassic;

    // oidn setup
    OIDNDevice device = oidnNewDevice(OIDN_DEVICE_TYPE_DEFAULT);
    oidnCommitDevice(device);
    auto albedoTracer = make_unique<AlbedoTracer>(scene);
    auto normalTracer = make_unique<NormalTracer>(scene);
    frame.enableOidn();

    auto renderStartTime = std::chrono::steady_clock::now();

    for (int i = 0; i < frame.spp; i++) {

        // initialize global statistics to 0 V_iter, C_iter
        // initialize local statistics C,E,M_iter_b

        const float timeBeforeIter = computeElapsedSeconds(renderStartTime);

        // while budget of iteration not exceeded {
        for (int j = 0; j < resy; j++) {
            for (int i = 0; i < resx; i++) {
                Vec2i px(i, j);
                sampler->startPath(i + j, 0xFFFF);
                frame.add(px, albedoTracer->trace(px, *sampler), FrameBuffer::ALBEDO);
                frame.add(px, normalTracer->trace(px, *sampler), FrameBuffer::NORMAL);
                frame.add(px, EARStracer.trace(px, *sampler), FrameBuffer::COLOR);
                // xi = sampleCamera
                // c, Lr = LrEstimate(xi, I_px)
                // C_itr += 1 + c
                // V_itr += relativeVariance
                // I_px += T(x_i, Lr)
            }
            std::cout << "Completed row " << j << "\r";
        }
        // normalize global cost, variance
        // I = MergeFramesByVariance(I)
        // I~ = Denoise(I)
        // updateCache

        EARStracer.updateImageStatistics(computeElapsedSeconds(renderStartTime) - timeBeforeIter);

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

        EARStracer.cache.build(true);

        std::cout << std::endl << "Completed sample " << i+1 << std::endl;
    }


//    frame.normalize(FrameBuffer::ALBEDO);
//    frame.normalize(FrameBuffer::NORMAL);
//    frame.normalize(FrameBuffer::COLOR);
//    frame.normalize(FrameBuffer::OIDN);
//
//    frame.toPng("albedo_img.png", FrameBuffer::ALBEDO);
//    frame.toPng("normal_img.png", FrameBuffer::NORMAL);

//    OIDNFilter filter = oidnNewFilter(device, "RT");
//    oidnSetSharedFilterImage(filter, "color", frame.color.data(), OIDN_FORMAT_FLOAT3, resx, resy, 0, 0, 0);
//    oidnSetSharedFilterImage(filter, "albedo", frame.albedo.data(), OIDN_FORMAT_FLOAT3, resx, resy, 0, 0, 0);
//    oidnSetSharedFilterImage(filter, "normal", frame.normal.data(), OIDN_FORMAT_FLOAT3, resx, resy, 0, 0, 0);
//    oidnSetSharedFilterImage(filter, "output", frame.oidn.data(), OIDN_FORMAT_FLOAT3, resx, resy, 0, 0, 0);
//    oidnSetFilter1b(filter, "hdr", true);
//    oidnCommitFilter(filter);
//    oidnExecuteFilter(filter);

    // Cleanup
    oidnReleaseDevice(device);

    frame.toPng("oidn_img.png", FrameBuffer::OIDN);
}
