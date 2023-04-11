#ifndef OCTTREE_H
#define OCTTREE_H

#include "usings.h"

#include <array>
#include <vector>

namespace EARS {

class Octtree {
public:
    static constexpr int HISTOGRAM_RESOLUTION = 4;
    static constexpr int BIN_COUNT = HISTOGRAM_RESOLUTION * HISTOGRAM_RESOLUTION;

    struct Configuration {
        float minimumLeafWeightForSampling = 40000;
        float minimumLeafWeightForTraining = 20000;
        float leafDecay = 0; /// set to 0 for hard reset after an iteration, 1 for no reset at all
        float maxNodeCount = 0;
    };

    struct TrainingNode {
        void decay(float decayFactor) {
            m_lrWeight *= decayFactor;
            m_lrFirstMoment *= decayFactor;
            m_lrSecondMoment *= decayFactor;
            m_lrCost *= decayFactor;
        }

        TrainingNode &operator+=(const TrainingNode &other) {
            m_lrWeight += other.m_lrWeight;
            m_lrFirstMoment += other.m_lrFirstMoment;
            m_lrSecondMoment += other.m_lrSecondMoment;
            m_lrCost += other.m_lrCost;
            return *this;
        }

        float getWeight() const {
            return m_lrWeight;
        }

        Vec3f getLrEstimate() const {
            return m_lrWeight > 0 ? m_lrFirstMoment / m_lrWeight : Vec3f(0.f);
        }

        Vec3f getLrSecondMoment() const {
            return m_lrWeight > 0 ? m_lrSecondMoment / m_lrWeight : Vec3f(0.f);
        }

        Vec3f getLrVariance() const {
            if (m_lrWeight == 0)
                return Vec3f(0.f);

            Vec3f result;
            for (int i = 0; i < 3; ++i) {
                result[i] =
                    std::max(m_lrSecondMoment[i] / m_lrWeight - float(std::pow(m_lrFirstMoment[i] / m_lrWeight, 2)),
                             float(0));
            }
            return result;
        }

        float getLrCost() const {
            return m_lrWeight > 0 ? m_lrCost / m_lrWeight : 0;
        }

        void splatLrEstimate(const Vec3f &sum, const Vec3f &sumSquares, float cost, float weight) {
            m_lrFirstMoment += sum;
            m_lrSecondMoment += sumSquares;
            m_lrCost += cost;
            m_lrWeight += weight;
        }

    private:
        float m_lrWeight{0.f};
        Vec3f m_lrFirstMoment{0.f};
        Vec3f m_lrSecondMoment{0.f};
        float m_lrCost{0.f};
    };

    struct SamplingNode {
        bool isValid() const { return m_isValid; }
        void learnFrom(const TrainingNode &trainingNode, const Configuration &config) {
            m_isValid = trainingNode.getWeight() >= config.minimumLeafWeightForSampling;

            if (trainingNode.getWeight() > 0) {
                lrEstimate = trainingNode.getLrEstimate();

                if (trainingNode.getLrCost() > 0) {
                    earsFactorR = trainingNode.getLrSecondMoment() / trainingNode.getLrCost();
                    earsFactorS = trainingNode.getLrVariance() / trainingNode.getLrCost();
                } else {
                    /// there can be caches where no work is done
                    /// (e.g., failed strict normals checks meaning no NEE samples or BSDF samples are ever taken)
                    earsFactorR = Vec3f(0.f);
                    earsFactorS = Vec3f(0.f);
                }
            }
        }

        Vec3f lrEstimate;
        Vec3f earsFactorR; // sqrt(2nd-moment / cost)
        Vec3f earsFactorS; // sqrt(variance / cost)

    private:
        bool m_isValid;
    };

    Configuration configuration;

    void setMaximumMemory(long bytes) {
        configuration.maxNodeCount = bytes / sizeof(Node);
    }

private:
    typedef uint32_t NodeIndex;

    struct Node {
        struct Child {
            NodeIndex index{0};
            std::array<TrainingNode, BIN_COUNT> training;
            std::array<SamplingNode, BIN_COUNT> sampling;

            bool isLeaf() const { return index == 0; }
            float maxTrainingWeight() const {
                float weight = 0;
                for (const auto &t : training)
                    weight = std::max(weight, t.getWeight());
                return weight;
            }
        };

        std::array<Child, 8> children;
    };

    std::vector<Node> m_nodes;

    int stratumIndex(Vec3f &pos) {
        int index = 0;
        for (int dim = 0; dim < 3; ++dim) {
            int bit = pos[dim] >= 0.5f;
            index |= bit << dim;
            pos[dim] = pos[dim] * 2 - bit;
        }
        return index;
    }

    NodeIndex splitNodeIfNecessary(float weight) {
        if (weight < configuration.minimumLeafWeightForTraining)
            /// splitting not necessary
            return 0;

        if (configuration.maxNodeCount && long(m_nodes.size()) > configuration.maxNodeCount)
            /// we have already reached the maximum node number
            return 0;

        NodeIndex newNodeIndex = NodeIndex(m_nodes.size());
        m_nodes.emplace_back();

        for (int stratum = 0; stratum < 8; ++stratum) {
            /// split recursively if needed
            NodeIndex newChildIndex = splitNodeIfNecessary(weight / 8);
            m_nodes[newNodeIndex].children[stratum].index = newChildIndex;
        }

        return newNodeIndex;
    }

    std::array<TrainingNode, BIN_COUNT> build(NodeIndex index, bool needsSplitting) {
        std::array<TrainingNode, BIN_COUNT> sum;

        for (int stratum = 0; stratum < 8; ++stratum) {
            if (m_nodes[index].children[stratum].isLeaf()) {
                if (needsSplitting) {
                    NodeIndex newChildIndex = splitNodeIfNecessary(
                        m_nodes[index].children[stratum].maxTrainingWeight()
                    );
                    m_nodes[index].children[stratum].index = newChildIndex;
                }
            } else {
                /// build recursively
                auto buildResult = build(
                    m_nodes[index].children[stratum].index,
                    needsSplitting
                );
                m_nodes[index].children[stratum].training = buildResult;
            }

            auto &child = m_nodes[index].children[stratum];
            for (int bin = 0; bin < BIN_COUNT; ++bin) {
                sum[bin] += child.training[bin];
                child.sampling[bin].learnFrom(child.training[bin], configuration);
                child.training[bin].decay(configuration.leafDecay);
            }
        }

        return sum;
    }

public:
    Octtree() {
        m_nodes.emplace_back();

        /// initialize tree to some depth
        for (int stratum = 0; stratum < 8; ++stratum) {
            NodeIndex newChildIndex = splitNodeIfNecessary(
                8 * configuration.minimumLeafWeightForSampling
            );
            m_nodes[0].children[stratum].index = newChildIndex;
        }
    }

    /**
     * Accumulates all the data from training into the sampling nodes, refines the tree and resets the training nodes.
     */
    void build(bool needsSplitting) {
        auto sum = build(0, needsSplitting);
        m_nodes.shrink_to_fit();

        float weightSum = 0;
        for (int bin = 0; bin < BIN_COUNT; ++bin)
            weightSum += sum[bin].getWeight();

        printf("Octtree built [%ld samples, %ld nodes, %.1f MiB]",
               long(weightSum),
               m_nodes.size(),
               m_nodes.capacity() * sizeof(Node) / (1024.f * 1024.f)
        );
    }

    void lookup(Vec3f pos, int bin, const SamplingNode *&sampling, TrainingNode *&training) {
        NodeIndex currentNodeIndex = 0;
        while (true) {
            int stratum = stratumIndex(pos);
            auto &child = m_nodes[currentNodeIndex].children[stratum];
            if (currentNodeIndex == 0 || child.sampling[bin].isValid())
                /// a valid node for sampling
                sampling = &child.sampling[bin];

            if (child.isLeaf()) {
                /// reached a leaf node
                training = &child.training[bin];
                break;
            }

            currentNodeIndex = child.index;
        }
    }
};

};

#endif //OCTTREE_H
