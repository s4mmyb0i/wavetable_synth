#include <catch2/catch_test_macros.hpp>

#include "dsp/WavetableMipBank.h"

TEST_CASE ("mip select uses duller tables at higher frequencies", "[mip]")
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

TEST_CASE ("square mip bank selects by the same harmonic limit rule", "[mip]")
{
    WavetableMipBank bank;
    bank.build (WavetableMipBank::Shape::square);

    constexpr float sr = 48000.0f;

    REQUIRE (&bank.select (80.0f, sr) == &bank.getLevel (0));
    REQUIRE (&bank.select (15000.0f, sr) == &bank.getLevel (WavetableMipBank::numLevels - 1));
}

TEST_CASE ("mip select picks expected level at a known boundary", "[mip]")
{
    WavetableMipBank bank;
    bank.build (WavetableMipBank::Shape::saw);

    constexpr float sr = 48000.0f;
    constexpr float limit = 0.9f * 0.5f * sr; // 21600

    // First level that fits for f=800: maxH * 800 < 21600 → maxH < 27 → level with 24.
    // Brightest with 24 harmonics is index of 24 in kMaxHarmonics = 5.
    const auto& selected = bank.select (800.0f, sr);

    int expected = WavetableMipBank::numLevels - 1;
    for (int i = 0; i < WavetableMipBank::numLevels; ++i)
    {
        if (static_cast<float> (WavetableMipBank::kMaxHarmonics[static_cast<std::size_t> (i)]) *
                800.0f <
            limit)
        {
            expected = i;
            break;
        }
    }

    REQUIRE (&selected == &bank.getLevel (expected));
    REQUIRE (expected == 5); // 24 harmonics
}

TEST_CASE ("invalid frequency or sample rate falls back to dullest mip", "[mip]")
{
    WavetableMipBank bank;
    bank.build (WavetableMipBank::Shape::saw);

    const auto& dullest = bank.getLevel (WavetableMipBank::numLevels - 1);

    REQUIRE (&bank.select (0.0f, 48000.0f) == &dullest);
    REQUIRE (&bank.select (440.0f, 0.0f) == &dullest);
    REQUIRE (&bank.select (-10.0f, 48000.0f) == &dullest);
}
