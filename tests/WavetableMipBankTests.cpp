#include <catch2/catch_test_macros.hpp>

#include "dsp/WavetableMipBank.h"

TEST_CASE("mip select uses duller tables at higher frequencies", "[mip]")
{
    WavetableMipBank bank;
    bank.build (WavetableMipBank::Shape::saw);

    constexpr float sr = 48000.0f;

    const auto& low = bank.select (100.0f, sr);   // 64 * 100 = 6400 < 21600
    const auto& high = bank.select (4000.0f, sr); // 64 * 4000 = 256000 > Nyquist*0.9

    // Different mip levels should be distinct table instances.
    REQUIRE (&low != &high);

    // Very high pitch should land on the dullest level.
    REQUIRE (&high == &bank.getLevel (WavetableMipBank::numLevels - 1));
}
