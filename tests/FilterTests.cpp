#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "dsp/Filter.h"

#include <cmath>

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
        REQUIRE (std::isfinite (filter.process (x)));
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
    float y = 0.0f;
    for (int i = 0; i < 2000; ++i) y = filter.process (dc);

    REQUIRE (y == Catch::Approx (dc).margin (0.05f));
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
        // Impulse then zeros — resonant filter rings longer / louder.
        energy += std::abs (filter.process (1.0f));
        for (int i = 0; i < 256; ++i) energy += std::abs (filter.process (0.0f));
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

    for (int i = 0; i < 512; ++i) REQUIRE (std::isfinite (filter.process (0.25f)));
}
