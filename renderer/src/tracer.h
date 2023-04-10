#ifndef PATHTRACER_H
#define PATHTRACER_H

#include "usings.h"

#include "ears.h"
#include "ray.h"
#include "sampler.h"
#include "scene.h"

class Tracer {
public:
    virtual ~Tracer() = default;
    virtual Vec3f trace(const Vec2i &px, PathSampleGenerator& sampler) = 0;
};

class CameraDebugTracer : public Tracer {
public:
    CameraDebugTracer(const Scene& scene) : scene(&scene) {};
    Vec3f trace(const Vec2i &px, PathSampleGenerator& sampler) override;
    const Scene* scene;
};

class IntersectionDebugTracer : public Tracer {
public:
    IntersectionDebugTracer(const Scene& scene) : scene(&scene) {};
    Vec3f trace(const Vec2i& px, PathSampleGenerator& sampler) override;
    const Scene* scene;
};

class AlbedoTracer: public Tracer {
public:
    AlbedoTracer(const Scene& scene) : scene(&scene) {};
    Vec3f trace(const Vec2i& px, PathSampleGenerator& sampler) override;
    const Scene* scene;
};

class NormalTracer: public Tracer {
public:
    NormalTracer(const Scene& scene) : scene(&scene) {};
    Vec3f trace(const Vec2i& px, PathSampleGenerator &sampler) override;
    const Scene* scene;
};

class PathTracer : public Tracer {
public:
    PathTracer(const Scene& scene) : scene(&scene) {};
    Vec3f trace(const Vec2i& px, PathSampleGenerator& sampler) override;
    bool handleSurface(SurfaceScatterEvent& event, Intersection& intersection, IntersectionData& data,
        int bounce, Ray& ray, Vec3f& throughput, Vec3f& emission, bool& wasSpecular);
    Vec3f estimateDirect(SurfaceScatterEvent& event, int bounce, const Ray& parentRay);
    Vec3f sampleDirect(const Primitive& light, SurfaceScatterEvent& event, int bounce, const Ray& parentRay);
    Vec3f lightSample(const Primitive& light, SurfaceScatterEvent& event, int bounce, const Ray& parentRay);
    Vec3f bsdfSample(const Primitive& light, SurfaceScatterEvent& event, int bounce, const Ray& parentRay);
    Vec3f attenuatedEmission(const Primitive& light, float expectedDist, Intersection& intersection, IntersectionData& data, int bounce, Ray& ray);
    Vec3f generalizedShadowRay(Ray& ray, const Primitive* endCap, int bounce) const;
    inline Vec3f generalizedShadowRayImpl(Ray& ray, const Primitive* endCap, int bounce, bool startsOnSurface, bool endsOnSurface, float& pdfForward, float& pdfBackward) const;
    SurfaceScatterEvent makeLocalScatterEvent(Intersection& intersection, IntersectionData& data, Ray& ray, PathSampleGenerator *sampler) const {
        TangentFrame frame(data.Ns);

        bool hitBackside = frame.normal.dot(ray.d()) > 0.0f;

        if (hitBackside) {
            frame.normal = -frame.normal;
            frame.tangent = -frame.tangent;
        }

        return SurfaceScatterEvent(
            &data,
            sampler,
            frame,
            frame.toLocal(-ray.d()),
            hitBackside 
        );
    }

    const Scene* scene;
    int maxBounces = 64;
};

class EARSTracer : public PathTracer {
public:
    EARSTracer(const Scene& scene) : PathTracer(scene) {
        imageEstimate = Film(scene.camera.resolution());
        imageEarsFactor = 0.0f;
        for (int i = 0; i < imageEstimate.resx * imageEstimate.resy; i++) {
            imageEstimate.add(i, Vec3f(0.5f));
        }
    };
    Vec3f trace(const Vec2i& px, PathSampleGenerator& sampler) override;

    Vec2f dirToCanonical(const Vec3f& d) {
        if (!std::isfinite(d.x()) || !std::isfinite(d.y()) || !std::isfinite(d.z())) {
            return { 0, 0 };
        }

        const float cosTheta = std::min(std::max(d.z(), -1.0f), 1.0f);
        float phi = std::atan2(d.y(), d.x());
        while (phi < 0)
            phi += 2.0 * PI;

        return { (cosTheta + 1) / 2, phi / (2 * PI) };
    }

    int mapOutgoingDirectionToHistogramBin(const Vec3f& wo) {
        const Vec2f p = dirToCanonical(wo);
        const int res = EARS::Octtree::HISTOGRAM_RESOLUTION;
        const int result =
            std::min(int(p.x() * res), res - 1) +
            std::min(int(p.y() * res), res - 1) * res;
        return result;
    }

    Vec3f mapPointToUnitCube(const Vec3f& point) {
        AABB aabb = scene->bounds;
        Vec3f size = aabb.getExtents();
        Vec3f result = point - aabb.min;
        result /= size;
        return result;
    }

    struct LiInput {
        Vec3f weight;
        Ray ray;
        int depth;
        bool wasSpecular;
    };

    struct LiOutput {
        Vec3f reflected{ 0.f };
        Vec3f emitted{ 0.f };
        float cost{ 0.f };

        int numSamples{ 0 };
        float depthAcc{ 0.f };
        float depthWeight{ 0.f };

        void markAsLeaf(int depth) {
            depthAcc = depth;
            depthWeight = 1;
        }

        float averagePathLength() const {
            return depthWeight > 0 ? depthAcc / depthWeight : 0;
        }

        float numberOfPaths() const {
            return depthWeight;
        }

        Vec3f totalContribution() const {
            return reflected + emitted;
        }
    };

    void updateImageStatistics(float actualTotalCost) {
        imageStatistics.reset(actualTotalCost);
        imageEarsFactor = imageStatistics.earsFactor();
    }

    void resetBlockAccumulators() {
        depthAcc = 0;
        depthWeight = 0;
        primarySplit = 0;
        samplesTaken = 0;
    }

    LiOutput Li(LiInput &input, PathSampleGenerator& sampler);

    EARS::Octtree cache;
    EARS::ImageStatistics imageStatistics;
    EARS::OutlierRejectedAverage blockStatistics;
    EARS::RRSMethod rrs;
    Film imageEstimate;
    float imageEarsFactor;
    float depthAcc;
    float depthWeight;
    float primarySplit;
    int samplesTaken;
};

#endif
