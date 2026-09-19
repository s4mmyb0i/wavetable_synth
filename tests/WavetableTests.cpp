#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "dsp/Wavetable.h"

#include <cmath>

TEST_CASE("sine table lookup at cardinal phases", "[wavetable]")
{
    const auto table = Wavetable::sine();

    REQUIRE(table.lookup(0.0f) == Catch::Approx(0.0f).margin(1.0e-3f));
    REQUIRE(table.lookup(0.25f) == Catch::Approx(1.0f).margin(1.0e-3f));
    REQUIRE(table.lookup(0.5f) == Catch::Approx(0.0f).margin(1.0e-3f));
    REQUIRE(table.lookup(0.75f) == Catch::Approx(-1.0f).margin(1.0e-3f));
}

TEST_CASE("saw stay in range and have expected polarity", "[wavetable]")
{
    const auto saw = Wavetable::saw();
    
    for (float phase = 0.0f; phase < 1.0f; phase += 0.01f)
    {
        REQUIRE(saw.lookup(phase) >= -1.0f - 1.0e-4f);
        REQUIRE(saw.lookup(phase) <= 1.0f + 1.0e-4f);
    }
    
    REQUIRE(saw.lookup(0.0f) == Catch::Approx(-1.0f).margin(1.0e-3f));
    REQUIRE(saw.lookup(0.5f) == Catch::Approx(0.0f).margin(1.0e-3f));
}

TEST_CASE("square stay in range and have expected polarity", "[wavetable]")
{
    const auto square = Wavetable::square();
    
    for (float phase = 0.0f; phase < 1.0f; phase += 0.01f)
    REQUIRE(std::abs(square.lookup(phase)) <= 1.0f + 1.0e-4f);

REQUIRE(square.lookup(0.25f) == Catch::Approx(1.0f).margin(1.0e-3f));
REQUIRE(square.lookup(0.75f) == Catch::Approx(-1.0f).margin(1.0e-3f));
}

TEST_CASE("lookup wraps phase into one cycle", "[wavetable]")
{
    const auto table = Wavetable::sine();

    REQUIRE(table.lookup(0.1f) == Catch::Approx(table.lookup(1.1f)).margin(1.0e-5f));
    REQUIRE(table.lookup(-0.25f) == Catch::Approx(table.lookup(0.75f)).margin(1.0e-3f));
}
