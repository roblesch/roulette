/* Adapted From Rath et. al.'s EARS
 * https://github.com/irath96/ears */

#ifndef IMAGESTATISTICS_H
#define IMAGESTATISTICS_H

#include "usings.h"

#include "outlierrejectedaverage.h"

namespace EARS {

struct ImageStatistics {
    void init() {

    }

    void setOutlierRejectionCount(int count) {
        m_average.resize(count);
    }

    void applyOutlierRejection() {
        m_average.computeOutlierContribution();
    }

    float cost() const {
        return m_lastStats.cost;
    }

    Vec3f squareError() const {
        return m_lastStats.squareError;
    }

    float earsFactor() const {
        return std::sqrt( cost() / squareError().avg() );
    }

    float efficiency() const {
        return 1 / (cost() * squareError().avg());
    }

    void reset(float actualTotalCost) {
        auto weight = m_average.weight();
        auto avgNoReject = m_average.averageWithoutRejection();
        auto avg = m_average.average();
        m_lastStats.squareError = avg.secondMoment;
        m_lastStats.cost = avg.cost;

        //m_average.dump();
        m_average.reset();

        float earsFactorNoReject = std::sqrt( avgNoReject.cost / avgNoReject.secondMoment.avg() );

        printf("Average path count:  %.3f\n", m_primarySamples > 0 ? m_depthWeight / m_primarySamples : 0);
        printf("Average path length: %.3f\n", m_depthWeight > 0 ? m_depthAcc / m_depthWeight : 0);
        printf("Average primary split: %.3f\n", m_primarySamples > 0 ? m_primarySplit / m_primarySamples : 0);
        printf("Statistics:\n"
                   "  (values in brackets are without outlier rejection)\n"
                   "  Estimated Cost    = %.3e (%.3e)\n"
                   "  Actual Cost       = %.3e (  n. a.  )\n"
                   "  Variance per SPP  = %.3e (%.3e)\n"
                   "  Est. Cost per SPP = %.3e (%.3e)\n"
                   "  Est. Efficiency   = %.3e (%.3e)\n"
                   "  Act. Efficiency   = %.3e (%.3e)\n"
                   "  EARS multiplier   = %.3e (%.3e)\n",
            avg.cost * weight, avgNoReject.cost * weight,
            actualTotalCost,
            squareError().avg(), avgNoReject.secondMoment.avg(),
            cost(), avgNoReject.cost,
            efficiency(), 1 / (avgNoReject.cost * avgNoReject.secondMoment.avg()),
            1 / (actualTotalCost / weight * squareError().avg()), 1 / (actualTotalCost / weight * avgNoReject.secondMoment.avg()),
            earsFactor(), earsFactorNoReject
        );

        m_depthAcc = 0;
        m_depthWeight = 0;
        m_primarySplit = 0;
        m_primarySamples = 0;
    }

    void operator+=(const OutlierRejectedAverage &blockStatistics) {
        m_average += blockStatistics;
    }

    void splatDepthAcc(float depthAcc, float depthWeight, float primarySplit, float primarySamples) {
        m_depthAcc += depthAcc;
        m_depthWeight += depthWeight;
        m_primarySplit += primarySplit;
        m_primarySamples += primarySamples;
    }

    bool hasOutlierLowerBound() const {
        return m_average.hasOutlierLowerBound();
    }

    OutlierRejectedAverage::Sample outlierLowerBound() const {
        return m_average.outlierLowerBound();
    }

public:
    int m_iterSpp;
    int m_totalSpp;

private:
    OutlierRejectedAverage m_average;
    float m_depthAcc { 0.f };
    float m_depthWeight { 0.f };
    float m_primarySplit { 0.f };
    float m_primarySamples { 0.f };

    struct {
        Vec3f squareError;
        float cost;
    } m_lastStats;
};

};

#endif //IMAGESTATISTICS_H
