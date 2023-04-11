#include "integrator.h"

#include <iostream>
#include <chrono>

#include "weightedbitmapaccumulator.h"

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

    // EARS configuration
    EARSTracer etracer(scene);

    // oidn setup
    OIDNDevice device = oidnNewDevice(OIDN_DEVICE_TYPE_DEFAULT);
    oidnCommitDevice(device);
    auto albedoTracer = make_unique<AlbedoTracer>(scene);
    auto normalTracer = make_unique<NormalTracer>(scene);
    Film albedo(resx, resy);
    Film normal(resx, resy);

    std::cout << "Rendering denoising auxillaries" << std::endl;
    // render denoising auxillaries 
    for (int j = 0; j < resy; j++) {
        for (int i = 0; i < resx; i++) {
            Vec2i px(i, j);
            sampler->startPath(i + j, 0xFFFF);
            albedo.add(px, albedoTracer->trace(px, *sampler));
            normal.add(px, normalTracer->trace(px, *sampler));
        }
    }

    Film rawEstimate(resx, resy);
    Film estimate(resx, resy);
    Film denoised(resx, resy);
    Film finalImg(resx, resy);
    Film lrEstImg(resx, resy);
    EARS::WeightedBitmapAccumulator finalImage{};
    finalImage.clear();
    etracer.imageStatistics.setOutlierRejectionCount(10);

    int spp = 4;
    int iteration;

    etracer.cache.configuration.leafDecay = 1;
    etracer.cache.setMaximumMemory(long(24) * 1024 * 1024);

    std::chrono::steady_clock::time_point renderStartTime = std::chrono::steady_clock::now();

    for (iteration = 0; iteration <= 16; iteration++) {
        const float timeBeforeIter = computeElapsedSeconds(renderStartTime);

        estimate.clear();
        rawEstimate.clear();

        bool isPretraining = iteration < 3;

        // don't use learning based methods unless caches have begun to converge
        if (isPretraining) {
            etracer.rrs = EARS::RRSMethod::Classic();
        }
        else {
            etracer.rrs = EARS::RRSMethod::ADRRS();
        }

        for (int pass = 1; pass <= spp; pass++) {
            // block rendering
            for (int blockY = 0; blockY < resy; blockY += 32) {
                for (int blockX = 0; blockX < resx; blockX += 32) {
                    EARS::OutlierRejectedAverage blockStatistics;
                    blockStatistics.resize(10);
                    if (etracer.imageStatistics.hasOutlierLowerBound()) {
                        blockStatistics.setRemoteOutlierLowerBound(etracer.imageStatistics.outlierLowerBound());
                    }
                    etracer.blockStatistics = blockStatistics;
                    etracer.resetBlockAccumulators();
                    std::cout << (isPretraining ? "(Pretraining)" : "(Rendering)") <<
                        " blockX : " << blockX << " blockY : " << blockY <<
                        " with " << pass <<"/" << spp << "spp\r";
                    for (int y = blockY; y < blockY + 32; y++) {
                        for (int x = blockX; x < blockX + 32; x++) {
                            Vec2i px(x, y);
                            sampler->startPath(x + y, 0xFFFF);
                            Vec3f li = etracer.trace(px, *sampler);
                            estimate.add(px, li/spp);
                            rawEstimate.add(px, li/spp);
                        }
                    }
                    etracer.imageStatistics += etracer.blockStatistics;
                    etracer.imageStatistics.splatDepthAcc(etracer.depthAcc, etracer.depthWeight, etracer.primarySplit, etracer.samplesTaken);
                }
            }
        }
        std::cout << std::endl;

        // draw lr cache
        for (int y = 0; y < resy; y++) {
            for (int x = 0; x < resy; x++) {
                Vec2i px(x, y);
                sampler->startPath(x + y, 0xFFFF);
                Vec3f lr = etracer.LrEstimate(px, *sampler);
                lrEstImg.add(px, lr);
            }
        }

        etracer.imageStatistics.m_iterSpp = spp;
        etracer.imageStatistics.m_totalSpp += spp;

        // reject outliers
        etracer.imageStatistics.applyOutlierRejection();
        // update caches
        etracer.cache.build(true);
        // update image statistics
        etracer.updateImageStatistics((computeElapsedSeconds(renderStartTime) - timeBeforeIter));

        // finalImage.add(rawEstimate, spp);
        finalImage.add(
            rawEstimate, spp,
            etracer.rrs.performsInvVarWeighting() ?
            (iteration > 0 ? etracer.imageStatistics.squareError().avg() : 0) :
            1
        );

        if (finalImage.hasData())
            finalImage.develop(&etracer.imageEstimate);

        // denoise estimate
        OIDNFilter filter = oidnNewFilter(device, "RT");
        oidnSetSharedFilterImage(filter, "color", etracer.imageEstimate.data(), OIDN_FORMAT_FLOAT3, resx, resy, 0, 0, 0);
        oidnSetSharedFilterImage(filter, "albedo", albedo.data(), OIDN_FORMAT_FLOAT3, resx, resy, 0, 0, 0);
        oidnSetSharedFilterImage(filter, "normal", normal.data(), OIDN_FORMAT_FLOAT3, resx, resy, 0, 0, 0);
        oidnSetSharedFilterImage(filter, "output", etracer.imageEstimate.data(), OIDN_FORMAT_FLOAT3, resx, resy, 0, 0, 0);
        oidnSetFilter1b(filter, "hdr", true);
        oidnCommitFilter(filter);
        oidnExecuteFilter(filter);

        const char* errorMessage;
        if (oidnGetDeviceError(device, &errorMessage) != OIDN_ERROR_NONE)
            printf("Error: %s\n", errorMessage);

        // debug images
        char fname[32];
        frame.color = etracer.imageEstimate.buffer;
        snprintf(fname, sizeof(fname), "iteration_%d_denoise.png", iteration);
        frame.toPng(fname);

        frame.color = estimate.buffer;
        snprintf(fname, sizeof(fname), "iteration_%d_estimate.png", iteration);
        frame.toPng(fname);

        finalImage.develop(&finalImg);
        frame.color = finalImg.buffer;
        snprintf(fname, sizeof(fname), "iteration_%d_merged.png", iteration);
        frame.toPng(fname);

        frame.color = lrEstImg.buffer;
        snprintf(fname, sizeof(fname), "iteration_%d_lr.png", iteration);
        frame.toPng(fname);

        std::cout << "Iteration : " << iteration << " Spp : " << spp << " Avg variance : " << etracer.imageStatistics.squareError().avg() << " Image EARS Factor : " << etracer.imageEarsFactor << " Elapsed : " << timeBeforeIter << std::endl;
    }

    oidnReleaseDevice(device);

    frame.useOidn = true;
    frame.color = finalImg.buffer;
    frame.oidn = estimate.buffer;
}
