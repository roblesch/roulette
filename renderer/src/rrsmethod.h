#ifndef RRSMETHOD_H
#define RRSMETHOD_H

#include "usings.h"

#include "octtree.h"

namespace EARS {

struct RRSMethod {
    enum {
        ENone,
        EClassic,
        EGWTW,
        EADRRS,
        EEARS,
    } technique;

    float splittingMin;
    float splittingMax;

    int rrDepth;
    bool useAbsoluteThroughput;

    RRSMethod() {
        technique = ENone;
        splittingMin = 1;
        splittingMax = 1;
        rrDepth = 1;
        useAbsoluteThroughput = true;
    }

    static RRSMethod None() {
        RRSMethod rrs;
        rrs.technique = ENone;
        rrs.splittingMin = 1;
        rrs.splittingMax = 1;
        rrs.rrDepth = 1;
        rrs.useAbsoluteThroughput = true;
        return rrs;
    }

    static RRSMethod EARS() {
        /// parse parameters
        RRSMethod rrs;
        rrs.splittingMin = 0.05f;
        rrs.splittingMax = 20;
        rrs.rrDepth = 1;
        rrs.technique = EEARS;
        rrs.useAbsoluteThroughput = false;
        return rrs;
    }

    static RRSMethod ADRRS() {
        RRSMethod rrs;
        rrs.splittingMin = 0.05f;
        rrs.splittingMax = 20;
        rrs.rrDepth = 2;
        rrs.technique = EADRRS;
        rrs.useAbsoluteThroughput = false;
        return rrs;
    }

    static RRSMethod Classic() {
        RRSMethod rrs;
        rrs.technique = EClassic;
        rrs.splittingMin = 0;
        rrs.splittingMax = 0.95f;
        rrs.rrDepth = 5;
        rrs.useAbsoluteThroughput = true;
        return rrs;
    }

    float evaluate(
        const Octtree::SamplingNode *samplingNode,
        float imageEarsFactor,
        const Vec3f &albedo,
        const Vec3f &throughput,
        float shininess,
        bool bsdfHasSmoothComponent,
        int depth
    ) const {
        if (depth < rrDepth) {
            /// do not perform RR or splitting at this depth.
            return 1;
        }

        switch (technique) {
        case ENone: {
            /// the simplest mode of all. perform no splitting and no RR.
            return clamp(1);
        }

        case EClassic: {
            /// Classic RR(S) based on throughput weight
            if (albedo == Vec3f(0.0f))
                /// avoid bias for materials that might report their reflectance incorrectly
                return clamp(0.1f);
            return clamp((throughput * albedo).avg());
        }

        case EGWTW: {
            /// "Go with the Winners"
            const float Vr = 1.0;
            const float Vv = splittingMax * splittingMax - 1.0;
            return clamp((throughput * albedo).avg() * std::sqrt(Vr + Vv / std::pow(shininess + 1, 2)));
        }

        case EADRRS: {
            /// "Adjoint-driven Russian Roulette and Splitting"
            const Vec3f LiEstimate = samplingNode->lrEstimate;
            if (bsdfHasSmoothComponent && LiEstimate.max() > 0) {
                return clamp(weightWindow((throughput * LiEstimate).avg()));
            } else {
                return clamp(1);
            }
        }

        case EEARS: {
            /// "Efficiency-Aware Russian Roulette and Splitting"
            if (bsdfHasSmoothComponent) {
                const float splittingFactorS = std::sqrt( (throughput * throughput * samplingNode->earsFactorS).avg() ) * imageEarsFactor;
                const float splittingFactorR = std::sqrt( (throughput * throughput * samplingNode->earsFactorR).avg() ) * imageEarsFactor;

                if (splittingFactorR > 1) {
                    if (splittingFactorS < 1) {
                        /// second moment and variance disagree on whether to split or RR, resort to doing nothing.
                        return clamp(1);
                    } else {
                        /// use variance only if both modes recommend splitting.
                        return clamp(splittingFactorS);
                    }
                } else {
                    /// use second moment only if it recommends RR.
                    return clamp(splittingFactorR);
                }
            } else {
                return clamp(1);
            }
        }
        }

        /// make gcc happy
        return 0;
    }

    bool needsTrainingPhase() const {
        switch (technique) {
        case ENone:    return false;
        case EClassic: return false;
        case EGWTW:    return false;
        case EADRRS:   return true;
        case EEARS:    return true;
        }

        /// make gcc happy
        return false;
    }

    bool performsInvVarWeighting() const {
        return needsTrainingPhase();
    }

    bool needsPixelEstimate() const {
        return useAbsoluteThroughput == false;
    }

private:
    float clamp(float splittingFactor) const {
        /// not using std::clamp here since that's C++17
        splittingFactor = std::min(splittingFactor, splittingMax);
        splittingFactor = std::max(splittingFactor, splittingMin);
        return splittingFactor;
    }

    float weightWindow(float splittingFactor, float weightWindowSize = 5) const {
        const float dminus = 2 / (1 + weightWindowSize);
        const float dplus = dminus * weightWindowSize;

        if (splittingFactor < dminus) {
            /// russian roulette
            return splittingFactor / dminus;
        } else if (splittingFactor > dplus) {
            /// splitting
            return splittingFactor / dplus;
        } else {
            /// within weight window
            return 1;
        }
    }
};
}

#endif //RRSMETHOD_H
