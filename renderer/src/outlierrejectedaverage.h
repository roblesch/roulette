#ifndef OUTLIERREJECTEDAVERAGE_H
#define OUTLIERREJECTEDAVERAGE_H

#include "usings.h"

#include <vector>
#include <iostream>

namespace EARS {

/**
 * Helper class to build averages that discared a given amount of outliers.
 * Used for our image variance estimate.
 */
class OutlierRejectedAverage {
public:
    struct Sample {
        Vec3f secondMoment;
        float cost;

        Sample()
            : secondMoment(Vec3f(0.f)), cost(0) {}

        Sample(const Vec3f &sm, float cost)
            : secondMoment(sm), cost(cost) {}

        void operator+=(const Sample &other) {
            secondMoment += other.secondMoment;
            cost += other.cost;
        }

        void operator-=(const Sample &other) {
            secondMoment -= other.secondMoment;
            cost -= other.cost;
        }

        Sample operator-(const Sample &other) const {
            Sample s = *this;
            s -= other;
            return s;
        }

        Sample operator/(float weight) const {
            return Sample{
                secondMoment / weight,
                cost / weight
            };
        }

        bool operator>=(const Sample &other) const {
            return secondMoment.avg() >= other.secondMoment.avg();
        }
    };

    /**
     * Resizes the history buffer to account for up to \c length outliers.
     */
    void resize(int length) {
        m_length = length;
        m_history.resize(length);
        reset();
    }

    /**
     * Resets all statistics, including outlier history and current average.
     */
    void reset() {
        m_index = 0;
        m_knownMinimum = Sample();
        m_accumulation = Sample();
        m_weight = 0;
        m_outlierAccumulation = Sample();
        m_outlierWeight = 0;
    }

    /**
     * Returns whether a lower bound can be given on what will definitely not count as outlier.
     */
    bool hasOutlierLowerBound() const {
        return m_length > 0 && m_index >= m_length;
    }

    /**
     * Returns the lower bound of what will definitely count as outlier.
     * Useful if multiple \c OutlierRejectedAverage from different threads will be combined.
     */
    Sample outlierLowerBound() const {
        return m_history[m_index - 1];
    }

    /**
     * Sets a manual lower bound of what will count as outlier.
     * This avoids wasting time on adding samples to the outlier history that are known to be less significant
     * than outliers that have already been collected by other instances of \c OutlierRejectedAverage that
     * will eventually be merged.
     */
    void setRemoteOutlierLowerBound(const Sample &minimum) {
        m_knownMinimum = minimum;
    }

    /**
     * Records one sample.
     */
    void operator+=(Sample sample) {
        m_weight += 1;
        m_accumulation += sample;

        if (m_knownMinimum >= sample) {
            return;
        }

        int insertionPoint = m_index;

        while (insertionPoint > 0 && sample >= m_history[insertionPoint - 1]) {
            if (insertionPoint < m_length) {
                m_history[insertionPoint] = m_history[insertionPoint - 1];
            }
            insertionPoint--;
        }

        if (insertionPoint < m_length) {
            m_history[insertionPoint] = sample;
            if (m_index < m_length) {
                ++m_index;
            }
        }
    }

    /**
     * Merges the statistics of another \c OutlierRejectedAverage into this instance.
     */
    void operator+=(const OutlierRejectedAverage &other) {
        int m_writeIndex = m_index + other.m_index;
        int m_readIndexLocal = m_index - 1;
        int m_readIndexOther = other.m_index - 1;

        while (m_writeIndex > 0) {
            Sample sample;
            if (m_readIndexOther < 0
                || (m_readIndexLocal >= 0 && other.m_history[m_readIndexOther] >= m_history[m_readIndexLocal])) {
                /// we take the local sample next
                sample = m_history[m_readIndexLocal--];
            } else {
                /// we take the other sample next
                sample = other.m_history[m_readIndexOther--];
            }

            if (--m_writeIndex < m_length) {
                m_history[m_writeIndex] = sample;
            }
        }

        m_index = std::min(m_index + other.m_index, m_length);
        m_weight += other.m_weight;
        m_accumulation += other.m_accumulation;
    }

    void dump() const {
        std::cout << m_index << " vs " << m_length << std::endl;
        for (int i = 0; i < m_index; ++i)
            std::cout << m_history[i].secondMoment.avg() << std::endl;
    }

    void computeOutlierContribution() {
        for (int i = 0; i < m_index; ++i) {
            m_outlierAccumulation += m_history[i];
        }
        m_outlierWeight += m_index;

        /// reset ourselves
        m_index = 0;
    }

    Sample average() const {
        if (m_index > 0) {
            printf("There are some outliers that have not yet been removed. Did you forget to call computeOutlierContribution()?");
        }

        return (m_accumulation - m_outlierAccumulation) / (m_weight - m_outlierWeight);
    }

    Sample averageWithoutRejection() const {
        return m_accumulation / m_weight;
    }

    long weight() const {
        return m_weight;
    }

private:
    long m_weight;
    int m_index;
    int m_length;
    Sample m_accumulation;
    Sample m_knownMinimum;
    std::vector<Sample> m_history;

    Sample m_outlierAccumulation;
    long m_outlierWeight;
};

};

#endif //OUTLIERREJECTEDAVERAGE_H
