#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "dsp/Filter.h"

#include <cmath>

// Mirrors SynthVoice's dual-filter path: osc sum -> oscFilter, noise -> noiseFilter,
// then add, without linking JUCE SynthesiserVoice.

namespace
{
constexpr float kMixScale = 0.25f;
constexpr float kSr = 48000.0f;

float tone (int i, float freqHz)
{
    return std::sin (2.0f * 3.14159265f * freqHz * static_cast<float> (i) / kSr);
}

float rmsOfFilteredTone (Filter& filter, float freqHz, int samples)
{
    filter.reset();
    double sumSq = 0.0;
    for (int i = 0; i < samples; ++i)
    {
        const float x = tone (i, freqHz);
        float yL = 0.0f, yR = 0.0f;
        filter.process (x, x, yL, yR);
        sumSq += static_cast<double> (yL) * static_cast<double> (yL);
    }
    return static_cast<float> (std::sqrt (sumSq / static_cast<double> (samples)));
}

void mixDualFilters (Filter& oscFilter, Filter& noiseFilter, float oscIn, float noiseIn,
                     float& outL, float& outR)
{
    float oscL = 0.0f, oscR = 0.0f;
    float noiseL = 0.0f, noiseR = 0.0f;
    oscFilter.process (oscIn * kMixScale, oscIn * kMixScale, oscL, oscR);
    noiseFilter.process (noiseIn * kMixScale, noiseIn * kMixScale, noiseL, noiseR);
    outL = oscL + noiseL;
    outR = oscR + noiseR;
}
} // namespace

TEST_CASE ("osc and noise filters keep independent cutoffs", "[dual-filter]")
{
    Filter oscFilter, noiseFilter;
    for (auto* f : {&oscFilter, &noiseFilter})
    {
        f->setSampleRate (kSr);
        f->setResonance (0.0f);
    }

    // High tone: low cutoff should attenuate more than high cutoff.
    constexpr float toneHz = 4000.0f;
    oscFilter.setCutoffHz (200.0f);
    noiseFilter.setCutoffHz (8000.0f);

    const float oscRms = rmsOfFilteredTone (oscFilter, toneHz, 2048);
    const float noiseRms = rmsOfFilteredTone (noiseFilter, toneHz, 2048);

    REQUIRE (noiseRms > oscRms * 2.0f);
}

TEST_CASE ("changing noise filter does not alter osc-only mix", "[dual-filter]")
{
    Filter oscFilter, noiseFilter;
    for (auto* f : {&oscFilter, &noiseFilter})
    {
        f->setSampleRate (kSr);
        f->setCutoffHz (1000.0f);
        f->setResonance (0.0f);
        f->reset();
    }

    auto runBlock = [&] (float noiseCutoff)
    {
        noiseFilter.setCutoffHz (noiseCutoff);
        noiseFilter.reset();
        oscFilter.reset();

        double sumSq = 0.0;
        for (int i = 0; i < 1024; ++i)
        {
            const float oscIn = tone (i, 440.0f);
            constexpr float noiseIn = 0.0f; // osc-only
            float outL = 0.0f, outR = 0.0f;
            mixDualFilters (oscFilter, noiseFilter, oscIn, noiseIn, outL, outR);
            sumSq += static_cast<double> (outL) * static_cast<double> (outL);
        }
        return static_cast<float> (std::sqrt (sumSq / 1024.0));
    };

    const float a = runBlock (200.0f);
    const float b = runBlock (5000.0f);

    REQUIRE (a == Catch::Approx (b).margin (1.0e-5f));
}

TEST_CASE ("noise-only path uses noise filter stereo outputs", "[dual-filter]")
{
    Filter oscFilter, noiseFilter;
    for (auto* f : {&oscFilter, &noiseFilter})
    {
        f->setSampleRate (kSr);
        f->setCutoffHz (2000.0f);
        f->setResonance (0.3f);
        f->reset();
    }

    // Drive noise on the right channel only (panned hard right), osc silent.
    // Regresses the bug where sampleR used noiseFilteredL.
    float maxAbsL = 0.0f, maxAbsR = 0.0f;
    for (int i = 0; i < 512; ++i)
    {
        const float noiseSample = tone (i, 800.0f);
        float oscL = 0.0f, oscR = 0.0f;
        float noiseL = 0.0f, noiseR = 0.0f;

        oscFilter.process (0.0f, 0.0f, oscL, oscR);
        noiseFilter.process (0.0f, noiseSample * kMixScale, noiseL, noiseR);

        const float sampleL = oscL + noiseL;
        const float sampleR = oscR + noiseR; // must use noiseR, not noiseL

        maxAbsL = std::max (maxAbsL, std::abs (sampleL));
        maxAbsR = std::max (maxAbsR, std::abs (sampleR));
    }

    REQUIRE (maxAbsR > 0.05f);
    REQUIRE (maxAbsL < maxAbsR * 0.25f);
}

TEST_CASE ("dual-filter mix applies the same scale to osc and noise", "[dual-filter]")
{
    Filter oscFilter, noiseFilter;
    for (auto* f : {&oscFilter, &noiseFilter})
    {
        f->setSampleRate (kSr);
        f->setCutoffHz (10000.0f); // nearly bypass
        f->setResonance (0.0f);
        f->reset();
    }

    // At high cutoff, LP ≈ through; DC inputs should appear scaled by kMixScale each.
    constexpr float oscDc = 1.0f;
    constexpr float noiseDc = 1.0f;
    float outL = 0.0f, outR = 0.0f;
    for (int i = 0; i < 4000; ++i)
        mixDualFilters (oscFilter, noiseFilter, oscDc, noiseDc, outL, outR);

    // Each path ≈ kMixScale → sum ≈ 0.5
    REQUIRE (outL == Catch::Approx (2.0f * kMixScale).margin (0.05f));
    REQUIRE (outR == Catch::Approx (2.0f * kMixScale).margin (0.05f));
}
