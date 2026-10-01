#include <catch2/catch_test_macros.hpp>

#include "dsp/WavetableMipBank.h"

TEST_CASE("mip select uses duller tables at higher frequencies", "[mip]")
{
    WavetableMipBank bank;
    bank.build (WavetableMipBank::Shape::saw);

    constexpr float sr = 48000.0f;
    // limit = 0.9 * Nyquist = 21600

    const auto& low = bank.select (100.0f, sr);    // 128 * 100 = 12800 < 21600 → brightest
    const auto& mid = bank.select (1000.0f, sr);   // needs ≤21 harmonics → between levels
    const auto& high = bank.select (12000.0f, sr); // 2 * 12000 = 24000 > 21600 → dullest

    REQUIRE (&low == &bank.getLevel (0));
    REQUIRE (&low != &mid);
    REQUIRE (&high == &bank.getLevel (WavetableMipBank::numLevels - 1));
}
