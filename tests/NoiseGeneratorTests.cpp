#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "dsp/NoiseGenerator.h"

#include <algorithm>
#include <cmath>

namespace
{
float rms (NoiseGenerator& gen, int samples)
{
    double sumSq = 0.0;
    for (int i = 0; i < samples; ++i)
    {
        const float x = gen.process();
        sumSq += static_cast<double> (x) * static_cast<double> (x);
    }
    return static_cast<float> (std::sqrt (sumSq / static_cast<double> (samples)));
}

// Energy of a slow one-pole lowpass — pink should sit higher than white.
float lowpassEnergy (NoiseGenerator& gen, int samples)
{
    float lp = 0.0f;
    double sumSq = 0.0;
    constexpr float coeff = 0.02f;
    for (int i = 0; i < samples; ++i)
    {
        const float x = gen.process();
        lp += coeff * (x - lp);
        sumSq += static_cast<double> (lp) * static_cast<double> (lp);
    }
    return static_cast<float> (sumSq / static_cast<double> (samples));
}
} // namespace

TEST_CASE ("white noise samples stay in [-1, 1]", "[noise]")
{
    NoiseGenerator gen (0xA11CEu);
    gen.setType (NoiseGenerator::Type::white);

    for (int i = 0; i < 4096; ++i)
    {
        const float x = gen.process();
        REQUIRE (x >= -1.0f);
        REQUIRE (x <= 1.0f);
    }
}

TEST_CASE ("white noise is not constant", "[noise]")
{
    NoiseGenerator gen (0xBEEFu);
    gen.setType (NoiseGenerator::Type::white);

    const float a = gen.process();
    bool differed = false;
    for (int i = 0; i < 64; ++i)
    {
        if (gen.process() != a)
        {
            differed = true;
            break;
        }
    }
    REQUIRE (differed);
}

TEST_CASE ("same seed reproduces the same white sequence", "[noise]")
{
    NoiseGenerator a (0xC0FFEEu);
    NoiseGenerator b (0xC0FFEEu);
    a.setType (NoiseGenerator::Type::white);
    b.setType (NoiseGenerator::Type::white);

    for (int i = 0; i < 256; ++i)
        REQUIRE (a.process() == Catch::Approx (b.process()));
}

TEST_CASE ("different seeds diverge", "[noise]")
{
    NoiseGenerator a (1u);
    NoiseGenerator b (2u);
    a.setType (NoiseGenerator::Type::white);
    b.setType (NoiseGenerator::Type::white);

    int mismatches = 0;
    for (int i = 0; i < 64; ++i)
        if (a.process() != b.process())
            ++mismatches;

    REQUIRE (mismatches > 0);
}

TEST_CASE ("pink noise has more low-frequency energy than white", "[noise]")
{
    NoiseGenerator white (0xD00Du);
    NoiseGenerator pink (0xD00Du);
    white.setType (NoiseGenerator::Type::white);
    pink.setType (NoiseGenerator::Type::pink);

    constexpr int N = 20000;
    // Warm up pink filter state.
    for (int i = 0; i < 2048; ++i)
    {
        white.process();
        pink.process();
    }

    const float whiteLow = lowpassEnergy (white, N);
    const float pinkLow = lowpassEnergy (pink, N);

    REQUIRE (pinkLow > whiteLow * 1.5f);
}

TEST_CASE ("pink reset clears filter memory", "[noise]")
{
    NoiseGenerator warm (0xABCDu);
    NoiseGenerator cold (0xABCDu);
    warm.setType (NoiseGenerator::Type::pink);
    cold.setType (NoiseGenerator::Type::pink);

    for (int i = 0; i < 512; ++i)
    {
        warm.process();
        cold.process();
    }

    warm.reset();

    // Same RNG phase, different filter state → next sample differs.
    REQUIRE (warm.process() != Catch::Approx (cold.process()).margin (1.0e-8f));
}

TEST_CASE ("setType switches white and pink output", "[noise]")
{
    NoiseGenerator a (0x1234u);
    NoiseGenerator b (0x1234u);
    a.setType (NoiseGenerator::Type::white);
    b.setType (NoiseGenerator::Type::pink);

    float maxDiff = 0.0f;
    for (int i = 0; i < 256; ++i)
        maxDiff = std::max (maxDiff, std::abs (a.process() - b.process()));

    REQUIRE (maxDiff > 1.0e-3f);
}

TEST_CASE ("white rms is in a plausible band", "[noise]")
{
    NoiseGenerator gen (0x5150u);
    gen.setType (NoiseGenerator::Type::white);
    // Uniform [-1,1] → theoretical RMS = 1/sqrt(3) ≈ 0.577
    REQUIRE (rms (gen, 50000) == Catch::Approx (0.577f).margin (0.05f));
}
