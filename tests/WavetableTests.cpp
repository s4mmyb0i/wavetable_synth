#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "dsp/Wavetable.h"

#include <cmath>

namespace
{
float measureRms (const Wavetable& table)
{
    double sumSquares = 0.0;
    constexpr int n = 2048;

    for (int i = 0; i < n; ++i)
    {
        const float phase = static_cast<float> (i) / static_cast<float> (n);
        const float s = table.lookup (phase);
        sumSquares += static_cast<double> (s) * s;
    }

    return static_cast<float> (std::sqrt (sumSquares / n));
}
}

TEST_CASE("sine table lookup at cardinal phases", "[wavetable]")
{
    const auto table = Wavetable::sine();

    REQUIRE(table.lookup(0.0f) == Catch::Approx(0.0f).margin(1.0e-3f));
    REQUIRE(table.lookup(0.25f) == Catch::Approx(1.0f).margin(1.0e-2f));
    REQUIRE(table.lookup(0.5f) == Catch::Approx(0.0f).margin(1.0e-3f));
    REQUIRE(table.lookup(0.75f) == Catch::Approx(-1.0f).margin(1.0e-2f));
}

TEST_CASE("saw polarity after rms normalize", "[wavetable]")
{
    const auto saw = Wavetable::saw();

    REQUIRE(saw.lookup(0.0f) < 0.0f);
    REQUIRE(saw.lookup(0.25f) < 0.0f);
    REQUIRE(saw.lookup(0.5f) == Catch::Approx(0.0f).margin(0.05f));
    REQUIRE(saw.lookup(0.75f) > 0.0f);
}

TEST_CASE("square polarity after rms normalize", "[wavetable]")
{
    const auto square = Wavetable::square();

    REQUIRE(square.lookup(0.25f) > 0.0f);
    REQUIRE(square.lookup(0.75f) < 0.0f);
}

TEST_CASE("triangle has expected polarity", "[wavetable]")
{
    const auto triangle = Wavetable::triangle();

    REQUIRE(triangle.lookup(0.125f) < 0.0f);
    REQUIRE(triangle.lookup(0.375f) > 0.0f);
    REQUIRE(triangle.lookup(0.625f) > 0.0f);
    REQUIRE(triangle.lookup(0.875f) < 0.0f);
}

TEST_CASE("lookup wraps phase into one cycle", "[wavetable]")
{
    const auto table = Wavetable::sine();

    REQUIRE(table.lookup(0.1f) == Catch::Approx(table.lookup(1.1f)).margin(1.0e-5f));
    REQUIRE(table.lookup(-0.25f) == Catch::Approx(table.lookup(0.75f)).margin(1.0e-3f));
}

TEST_CASE("all waveforms share similar rms", "[wavetable]")
{
    constexpr float target = 0.70710678118f;
    constexpr float tol = 0.02f;

    REQUIRE(measureRms(Wavetable::sine()) == Catch::Approx(target).margin(tol));
    REQUIRE(measureRms(Wavetable::saw()) == Catch::Approx(target).margin(tol));
    REQUIRE(measureRms(Wavetable::square()) == Catch::Approx(target).margin(tol));
    REQUIRE(measureRms(Wavetable::triangle()) == Catch::Approx(target).margin(tol));
}
