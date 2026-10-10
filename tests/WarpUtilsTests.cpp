#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "dsp/WarpUtils.h"

#include <cmath>

TEST_CASE ("unisonSpread single voice is zero", "[warp][utils]")
{
    REQUIRE (unisonSpread (0, 1) == Catch::Approx (0.0f));
    REQUIRE (unisonSpread (0, 0) == Catch::Approx (0.0f));
}

TEST_CASE ("unisonSpread spans -1 to +1", "[warp][utils]")
{
    constexpr int count = 5;
    REQUIRE (unisonSpread (0, count) == Catch::Approx (-1.0f));
    REQUIRE (unisonSpread (count - 1, count) == Catch::Approx (1.0f));
    REQUIRE (unisonSpread (2, count) == Catch::Approx (0.0f));
}

TEST_CASE ("warpPhase stays in unit interval", "[warp][utils]")
{
    for (WarpMode mode : {WarpMode::off, WarpMode::bendPlus, WarpMode::sync})
    {
        for (float amount : {0.0f, 0.5f, 1.0f, -2.0f, 3.0f})
        {
            for (int i = 0; i <= 32; ++i)
            {
                const float phase = static_cast<float> (i) / 32.0f;
                const float out = warpPhase (phase, amount, mode);
                REQUIRE (out >= 0.0f);
                REQUIRE (out < 1.0f);
            }
        }
    }
}

TEST_CASE ("warpPhase amount zero is identity for bend and sync", "[warp][utils]")
{
    for (float phase : {0.0f, 0.25f, 0.5f, 0.75f, 0.999f})
    {
        REQUIRE (warpPhase (phase, 0.0f, WarpMode::off) == Catch::Approx (phase).margin (1.0e-6f));
        REQUIRE (warpPhase (phase, 0.0f, WarpMode::bendPlus)
                 == Catch::Approx (phase).margin (1.0e-5f));
        REQUIRE (warpPhase (phase, 0.0f, WarpMode::sync) == Catch::Approx (phase).margin (1.0e-6f));
    }
}

TEST_CASE ("warpPhase bend and sync change mid-cycle phase when amount > 0", "[warp][utils]")
{
    constexpr float phase = 0.3f;
    REQUIRE (std::abs (warpPhase (phase, 0.8f, WarpMode::bendPlus) - phase) > 1.0e-3f);
    REQUIRE (std::abs (warpPhase (phase, 0.5f, WarpMode::sync) - phase) > 1.0e-3f);
}

TEST_CASE ("warpPhase sync wraps above 1", "[warp][utils]")
{
    // ratio = 1 + 1*7 = 8 → phase 0.2 → 1.6 → wraps to 0.6
    REQUIRE (warpPhase (0.2f, 1.0f, WarpMode::sync) == Catch::Approx (0.6f).margin (1.0e-5f));
}
