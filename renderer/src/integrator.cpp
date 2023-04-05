#include "integrator.h"

#include <iostream>
#include <chrono>

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
    //etracer.rrs = EARS::RRSMethod::EARS();
    //etracer.rrs.technique = EARS::RRSMethod::EEARS;

    // oidn setup
    OIDNDevice device = oidnNewDevice(OIDN_DEVICE_TYPE_DEFAULT);
    oidnCommitDevice(device);
    auto albedoTracer = make_unique<AlbedoTracer>(scene);
    auto normalTracer = make_unique<NormalTracer>(scene);
    Film albedo(resx, resy);
    Film normal(resx, resy);
    Film rawEstimate(resx, resy);
    Film estimate(resx, resy);
    Film denoised(resx, resy);
    Film finalImg(resx, resy);
    EARS::WeightedBitmapAccumulator finalImage;
    etracer.imageStatistics.setOutlierRejectionCount(10);

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

    float budget = 300.0f;
    float until = 0;
    int baseSpp = 2;
    int trainingSpp = 16;
    int spp = 0;
    int iteration;
    float iterationTime = 10;

    etracer.cache.configuration.leafDecay = 1;
    etracer.cache.setMaximumMemory(long(24) * 1024 * 1024);

    std::chrono::steady_clock::time_point renderStartTime = std::chrono::steady_clock::now();

    for (iteration = 0; iteration <= 5; iteration++) {
        const float timeBeforeIter = computeElapsedSeconds(renderStartTime);
        //if (timeBeforeIter >= budget) {
        //    //break;
        //}

        estimate.clear();
        rawEstimate.clear();

        bool isPretraining = iteration < 3;

        // don't use learning based methods unless caches have begun to converge
        if (isPretraining) {
            etracer.rrs = EARS::RRSMethod::Classic();
        }
        else {
            etracer.rrs = EARS::RRSMethod::EARS();
        }

        // stretch this iteration since next would finish anyway
        //until += iterationTime;
        //if (until > budget - iterationTime) {
        //    until = budget;
        //}

        // render until time is out
        float sampleTime;
        float depthAcc = 0;
        float depthWeight = 0;
        float primarySplit = 0;
        float samplesTaken = 0;

        for (int j = 0; j < resy; j++) {
            for (int i = 0; i < resx; i++) {
                Vec2i px(i, j);
                Vec3f r(0.0f);
                if (isPretraining) {
                    spp = trainingSpp;
                    for (int ss = 0; ss < spp; ss++) {
                        sampler->startPath(i + j, 0xFFFF);
                        r += etracer.trace(px, *sampler);
                    }
                    estimate.add(px, r / spp);
                    rawEstimate.add(px, r);
                } else {
                    spp = baseSpp;
                    for (int ss = 0; ss < spp; ss++) {
                        sampler->startPath(i + j, 0xFFFF);
                        r += etracer.trace(px, *sampler);
                    }
                    estimate.add(px, r / spp);
                    rawEstimate.add(px, r);
                }
            }
            std::cout << (isPretraining ? "(Pretraining)" : "(Rendering)") << " Completed row " << j << " with " << spp << "spp\r";
        }
        std::cout << std::endl;

        etracer.imageStatistics.applyOutlierRejection();

        // update caches
        etracer.cache.build(true);

        // update image statistics
        etracer.updateImageStatistics((computeElapsedSeconds(renderStartTime) - timeBeforeIter));

        const bool hasVarianceEstimate = iteration > 0;
        const float avgVariance = etracer.imageStatistics.squareError().avg();
        finalImage.add(&rawEstimate, spp, hasVarianceEstimate ? etracer.imageStatistics.squareError().avg() : 0);

        // denoise estimate
        OIDNFilter filter = oidnNewFilter(device, "RT");
        oidnSetSharedFilterImage(filter, "color", estimate.data(), OIDN_FORMAT_FLOAT3, resx, resy, 0, 0, 0);
        oidnSetSharedFilterImage(filter, "albedo", albedo.data(), OIDN_FORMAT_FLOAT3, resx, resy, 0, 0, 0);
        oidnSetSharedFilterImage(filter, "normal", normal.data(), OIDN_FORMAT_FLOAT3, resx, resy, 0, 0, 0);
        oidnSetSharedFilterImage(filter, "output", etracer.imageEstimate.data(), OIDN_FORMAT_FLOAT3, resx, resy, 0, 0, 0);
        oidnSetFilter1b(filter, "hdr", true);
        oidnCommitFilter(filter);
        oidnExecuteFilter(filter);

        const char* errorMessage;
        if (oidnGetDeviceError(device, &errorMessage) != OIDN_ERROR_NONE)
            printf("Error: %s\n", errorMessage);

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

        std::cout << "Iteration : " << iteration << " Spp : " << spp << " Avg variance : " << etracer.imageStatistics.squareError().avg() << " Image EARS Factor : " << etracer.imageEarsFactor << " Elapsed : " << timeBeforeIter << std::endl;
        baseSpp *= 2;
    }

    oidnReleaseDevice(device);

    frame.useOidn = true;
    frame.color = finalImg.buffer;
    frame.oidn = estimate.buffer;
}
