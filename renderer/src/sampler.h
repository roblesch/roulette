#ifndef SAMPLER_H
#define SAMPLER_H

#include "usings.h"

enum SampleBlockStructure
{
    NumContinuousSamples = 6,
    NumTransmittanceSamples = 1,
    NumDiscreteSamples = 4,

    ContinuousBlockOffset = 0,
    TransmittanceBlockOffset = ContinuousBlockOffset + NumContinuousSamples,
    DiscreteBlockOffset = TransmittanceBlockOffset + NumTransmittanceSamples,

    FullBlockSize = NumContinuousSamples + NumDiscreteSamples + NumTransmittanceSamples,
    ContinuousBlockSize = NumContinuousSamples + NumTransmittanceSamples
};

enum SampleBlock : int
{
    EmitterSample = ContinuousBlockOffset,
    CameraSample = ContinuousBlockOffset,
    BsdfSample = ContinuousBlockOffset,
    MediumPhaseSample = ContinuousBlockOffset,
    MediumTransmittanceSample = TransmittanceBlockOffset,
    DiscreteEmitterSample = DiscreteBlockOffset,
    DiscreteCameraSample = DiscreteBlockOffset,
    DiscreteBsdfSample = DiscreteBlockOffset,
    DiscreteMediumSample = DiscreteBlockOffset,
    DiscreteTransmittanceSample = DiscreteBlockOffset + 1,
    DiscreteTransparencySample = DiscreteBlockOffset + 2,
    DiscreteRouletteSample = DiscreteBlockOffset + 3,
};

class PathSampleGenerator
{
public:
    virtual ~PathSampleGenerator() = default;

    virtual void startPath(uint32 pixelId, int sample) = 0;
    virtual void advancePath() = 0;
    // FIXME: advancePath() is currently never called for direct camera
    // connections. This is because there is currently no use case where
    // random numbers are consumed during this process, but there are going
    // to be in the future. This requires some re-evaluation of random
    // numbers for direct samples
    virtual bool nextBoolean(SampleBlock block, float pTrue) = 0;
    virtual int nextDiscrete(SampleBlock block, int numChoices) = 0;
    virtual float next1D(SampleBlock block) = 0;
    virtual Vec2f next2D(SampleBlock block) = 0;
};

class UniformSampler
{
    uint64 _state;
    uint64 _sequence;

public:
    UniformSampler()
        : UniformSampler(0xBA5EBA11)
    {
    }

    explicit UniformSampler(uint64 seed, uint64 sequence = 0)
        : _state(seed),
        _sequence(sequence)
    {
    }

    static inline float uintBitsToFloat(uint32 i)
    {
        union {
            float f;
            uint32 i;
        } unionHack{};
        unionHack.i = i;
        return unionHack.f;
    }

    static inline float normalizedUint(uint32 i)
    {
        return uintBitsToFloat((i >> 9u) | 0x3F800000u) - 1.0f;
    }

    // PCG random number generator
    // See http://www.pcg-random.org/
    inline uint32 nextI()
    {
        uint64 oldState = _state;
        _state = oldState * 6364136223846793005ULL + (_sequence | 1);
        auto xorShifted = uint32(((oldState >> 18u) ^ oldState) >> 27u);
        uint32 rot = oldState >> 59u;
        return (xorShifted >> rot) | (xorShifted << (uint32(-int32(rot)) & 31));
    }

    inline float next1D()
    {
        return normalizedUint(nextI());
    }

    inline Vec2f next2D()
    {
        return {next1D(), next1D()};
    }

    [[nodiscard]] uint64 state() const
    {
        return _state;
    }

    [[nodiscard]] uint64 sequence() const
    {
        return _sequence;
    }
};

class UniformPathSampler : public PathSampleGenerator
{
    UniformSampler _sampler;

public:
    explicit UniformPathSampler(uint32 seed)
        : _sampler(seed)
    {
    }
    explicit UniformPathSampler(const UniformSampler& sampler)
        : _sampler(sampler)
    {
    }

    void startPath(uint32 /*pixelId*/, int /*sample*/) override
    {
    }
    void advancePath() override
    {
    }

    bool nextBoolean(SampleBlock /*block*/, float pTrue) final
    {
        return _sampler.next1D() < pTrue;
    }
    int nextDiscrete(SampleBlock /*block*/, int numChoices) final
    {
        return int(_sampler.next1D() * (float)numChoices);
    }
    float next1D(SampleBlock /*block*/) final
    {
        return _sampler.next1D();
    }
    Vec2f next2D(SampleBlock /*block*/) final
    {
        return {_sampler.next1D(), _sampler.next1D()};
    }

    [[nodiscard]] const UniformSampler& sampler() const
    {
        return _sampler;
    }
};

#endif
