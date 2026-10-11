#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "dsp/Filter.h"

#include <cmath>

namespace
{
void processMono (Filter& filter, float in, float& outL, float& outR)
{ filter.process (in, in, outL, outR); }
} // namespace

TEST_CASE ("filter output stays finite", "[filter]")
{
    Filter filter;
    filter.setSampleRate (48000.0f);
    filter.setCutoffHz (1000.0f);
    filter.setResonance (0.5f);
    filter.reset();

    for (int i = 0; i < 1024; ++i)
    {
        const float x = std::sin (0.05f * static_cast<float> (i));
        float yL = 0.0f, yR = 0.0f;
        processMono (filter, x, yL, yR);
        REQUIRE (std::isfinite (yL));
        REQUIRE (std::isfinite (yR));
    }
}

TEST_CASE ("filter dc step settles toward input at high cutoff", "[filter]")
{
    Filter filter;
    filter.setSampleRate (48000.0f);
    filter.setCutoffHz (8000.0f);
    filter.setResonance (0.0f);
    filter.reset();

    constexpr float dc = 0.5f;
    float yL = 0.0f, yR = 0.0f;
    for (int i = 0; i < 2000; ++i) processMono (filter, dc, yL, yR);

    REQUIRE (yL == Catch::Approx (dc).margin (0.05f));
    REQUIRE (yR == Catch::Approx (dc).margin (0.05f));
}

TEST_CASE ("higher resonance changes the impulse response", "[filter]")
{
    auto runImpulse = [] (float resonance)
    {
        Filter filter;
        filter.setSampleRate (48000.0f);
        filter.setCutoffHz (1000.0f);
        filter.setResonance (resonance);
        filter.reset();

        float energy = 0.0f;
        float yL = 0.0f, yR = 0.0f;
        // Impulse then zeros — resonant filter rings longer / louder.
        processMono (filter, 1.0f, yL, yR);
        energy += std::abs (yL);
        for (int i = 0; i < 256; ++i)
        {
            processMono (filter, 0.0f, yL, yR);
            energy += std::abs (yL);
        }
        return energy;
    };

    const float lowQ = runImpulse (0.0f);
    const float highQ = runImpulse (0.9f);

    REQUIRE (highQ > lowQ);
    REQUIRE (std::isfinite (lowQ));
    REQUIRE (std::isfinite (highQ));
}

TEST_CASE ("cutoff near nyquist stays stable with resonance", "[filter]")
{
    Filter filter;
    filter.setSampleRate (48000.0f);
    filter.setCutoffHz (20000.0f); // clamped internally below 0.45·sr
    filter.setResonance (0.8f);
    filter.reset();

    for (int i = 0; i < 512; ++i)
    {
        float yL = 0.0f, yR = 0.0f;
        processMono (filter, 0.25f, yL, yR);
        REQUIRE (std::isfinite (yL));
        REQUIRE (std::isfinite (yR));
    }
}

TEST_CASE ("stereo channels have independent state", "[filter]")
{
    Filter filter;
    filter.setSampleRate (48000.0f);
    filter.setCutoffHz (1000.0f);
    filter.setResonance (0.5f);
    filter.reset();

    // Drive only the left channel with an impulse; right stays quiet after settling.
    float yL = 0.0f, yR = 0.0f;
    filter.process (1.0f, 0.0f, yL, yR);
    REQUIRE (std::abs (yL) > std::abs (yR));

    for (int i = 0; i < 64; ++i) filter.process (0.0f, 0.0f, yL, yR);

    // Left still ringing more than right from the impulse.
    float leftEnergy = 0.0f, rightEnergy = 0.0f;
    for (int i = 0; i < 128; ++i)
    {
        filter.process (0.0f, 0.0f, yL, yR);
        leftEnergy += std::abs (yL);
        rightEnergy += std::abs (yR);
    }
    REQUIRE (leftEnergy > rightEnergy);
}
