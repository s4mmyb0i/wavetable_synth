#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "dsp/Wavetable.h"
#include "dsp/WavetableOsc.h"

#include <cmath>
#include <algorithm>

namespace
{
WavetableOsc makeOsc (const Wavetable& table, float sr, float freq, WarpMode mode, float amount)
{
    WavetableOsc osc (table);
    osc.setSampleRate (sr);
    osc.setUnison (1, 0.0f);
    osc.setWarp (mode, amount);
    osc.setFrequency (freq);
    osc.reset();
    return osc;
}

float maxAbsDiff (WavetableOsc& a, WavetableOsc& b, int samples)
{
    float maxDiff = 0.0f;
    for (int i = 0; i < samples; ++i)
        maxDiff = std::max (maxDiff, std::abs (a.process() - b.process()));
    return maxDiff;
}
} // namespace

TEST_CASE ("warp off matches default oscillator", "[warp]")
{
    const auto table = Wavetable::sine();
    constexpr float sr = 48000.0f;
    constexpr float freq = 440.0f;

    WavetableOsc off = makeOsc (table, sr, freq, WarpMode::off, 0.75f);
    WavetableOsc plain = makeOsc (table, sr, freq, WarpMode::off, 0.0f);

    for (int i = 0; i < 1024; ++i)
        REQUIRE (off.process() == Catch::Approx (plain.process()).margin (1.0e-5f));
}

TEST_CASE ("bendPlus with zero amount is identity", "[warp]")
{
    const auto table = Wavetable::sine();
    constexpr float sr = 48000.0f;
    constexpr float freq = 330.0f;

    WavetableOsc bent = makeOsc (table, sr, freq, WarpMode::bendPlus, 0.0f);
    WavetableOsc plain = makeOsc (table, sr, freq, WarpMode::off, 0.0f);

    for (int i = 0; i < 1024; ++i)
        REQUIRE (bent.process() == Catch::Approx (plain.process()).margin (1.0e-5f));
}

TEST_CASE ("sync with zero amount is identity", "[warp]")
{
    const auto table = Wavetable::sine();
    constexpr float sr = 48000.0f;
    constexpr float freq = 330.0f;

    WavetableOsc synced = makeOsc (table, sr, freq, WarpMode::sync, 0.0f);
    WavetableOsc plain = makeOsc (table, sr, freq, WarpMode::off, 0.0f);

    for (int i = 0; i < 1024; ++i)
        REQUIRE (synced.process() == Catch::Approx (plain.process()).margin (1.0e-5f));
}

TEST_CASE ("bendPlus with amount diverges from plain", "[warp]")
{
    const auto table = Wavetable::sine();
    constexpr float sr = 48000.0f;
    constexpr float freq = 440.0f;

    WavetableOsc bent = makeOsc (table, sr, freq, WarpMode::bendPlus, 0.8f);
    WavetableOsc plain = makeOsc (table, sr, freq, WarpMode::off, 0.0f);

    REQUIRE (maxAbsDiff (bent, plain, 2048) > 0.1f);
}

TEST_CASE ("sync with amount diverges from plain", "[warp]")
{
    const auto table = Wavetable::sine();
    constexpr float sr = 48000.0f;
    constexpr float freq = 440.0f;

    WavetableOsc synced = makeOsc (table, sr, freq, WarpMode::sync, 0.5f);
    WavetableOsc plain = makeOsc (table, sr, freq, WarpMode::off, 0.0f);

    REQUIRE (maxAbsDiff (synced, plain, 2048) > 0.1f);
}

TEST_CASE ("bendPlus and sync produce different outputs", "[warp]")
{
    const auto table = Wavetable::sine();
    constexpr float sr = 48000.0f;
    constexpr float freq = 440.0f;

    WavetableOsc bent = makeOsc (table, sr, freq, WarpMode::bendPlus, 0.7f);
    WavetableOsc synced = makeOsc (table, sr, freq, WarpMode::sync, 0.7f);

    REQUIRE (maxAbsDiff (bent, synced, 2048) > 0.05f);
}

TEST_CASE ("warp amount is clamped to 0..1", "[warp]")
{
    const auto table = Wavetable::sine();
    constexpr float sr = 48000.0f;
    constexpr float freq = 440.0f;

    WavetableOsc below = makeOsc (table, sr, freq, WarpMode::bendPlus, -5.0f);
    WavetableOsc zero = makeOsc (table, sr, freq, WarpMode::bendPlus, 0.0f);
    for (int i = 0; i < 512; ++i)
        REQUIRE (below.process() == Catch::Approx (zero.process()).margin (1.0e-5f));

    WavetableOsc above = makeOsc (table, sr, freq, WarpMode::sync, 5.0f);
    WavetableOsc one = makeOsc (table, sr, freq, WarpMode::sync, 1.0f);
    for (int i = 0; i < 512; ++i)
        REQUIRE (above.process() == Catch::Approx (one.process()).margin (1.0e-5f));
}

TEST_CASE ("warped output stays finite and bounded", "[warp]")
{
    const auto table = Wavetable::sine();
    WavetableOsc osc (table);
    osc.setSampleRate (48000.0f);
    osc.setUnison (1, 0.0f);
    osc.setFrequency (880.0f);

    for (WarpMode mode : {WarpMode::bendPlus, WarpMode::sync})
    {
        osc.setWarp (mode, 1.0f);
        osc.reset();
        for (int i = 0; i < 2048; ++i)
        {
            const float s = osc.process();
            REQUIRE (std::isfinite (s));
            REQUIRE (std::abs (s) < 2.0f);
        }
    }
}

TEST_CASE ("warp plus unison stays finite", "[warp][unison]")
{
    const auto table = Wavetable::sine();
    WavetableOsc osc (table);
    osc.setSampleRate (48000.0f);
    osc.setUnison (5, 15.0f);
    osc.setWarp (WarpMode::sync, 0.6f);
    osc.setFrequency (440.0f);
    osc.reset();

    for (int i = 0; i < 2048; ++i)
    {
        const float s = osc.process();
        REQUIRE (std::isfinite (s));
        REQUIRE (std::abs (s) < 2.0f);
    }
}

TEST_CASE ("independent oscs can use different warp settings", "[warp][multi-osc]")
{
    const auto table = Wavetable::sine();
    constexpr float sr = 48000.0f;
    constexpr float freq = 440.0f;

    WavetableOsc osc1 = makeOsc (table, sr, freq, WarpMode::off, 0.0f);
    WavetableOsc osc2 = makeOsc (table, sr, freq, WarpMode::bendPlus, 0.8f);
    WavetableOsc osc3 = makeOsc (table, sr, freq, WarpMode::sync, 0.6f);

    float maxDiff12 = 0.0f;
    float maxDiff13 = 0.0f;
    for (int i = 0; i < 2048; ++i)
    {
        const float s1 = osc1.process();
        const float s2 = osc2.process();
        const float s3 = osc3.process();
        const float mixed = s1 * 0.5f + s2 * 0.3f + s3 * 0.2f;

        REQUIRE (std::isfinite (mixed));
        maxDiff12 = std::max (maxDiff12, std::abs (s1 - s2));
        maxDiff13 = std::max (maxDiff13, std::abs (s1 - s3));
    }

    REQUIRE (maxDiff12 > 0.05f);
    REQUIRE (maxDiff13 > 0.05f);
}

TEST_CASE ("live setWarp changes the waveform", "[warp]")
{
    const auto table = Wavetable::sine();
    constexpr float sr = 48000.0f;
    constexpr float freq = 440.0f;

    WavetableOsc osc = makeOsc (table, sr, freq, WarpMode::off, 0.0f);
    WavetableOsc plain = makeOsc (table, sr, freq, WarpMode::off, 0.0f);

    // Match while off.
    for (int i = 0; i < 64; ++i)
        REQUIRE (osc.process() == Catch::Approx (plain.process()).margin (1.0e-5f));

    osc.setWarp (WarpMode::sync, 0.7f);

    float maxDiff = 0.0f;
    for (int i = 0; i < 1024; ++i)
        maxDiff = std::max (maxDiff, std::abs (osc.process() - plain.process()));

    REQUIRE (maxDiff > 0.1f);
}

TEST_CASE ("warp off after sync returns toward plain after reset", "[warp]")
{
    const auto table = Wavetable::sine();
    constexpr float sr = 48000.0f;
    constexpr float freq = 440.0f;

    WavetableOsc osc = makeOsc (table, sr, freq, WarpMode::sync, 0.8f);
    for (int i = 0; i < 256; ++i) osc.process();

    osc.setWarp (WarpMode::off, 0.0f);
    osc.reset();

    WavetableOsc plain = makeOsc (table, sr, freq, WarpMode::off, 0.0f);
    for (int i = 0; i < 512; ++i)
        REQUIRE (osc.process() == Catch::Approx (plain.process()).margin (1.0e-5f));
}

TEST_CASE ("bendPlus remaps frozen phase away from linear read", "[warp]")
{
    const auto table = Wavetable::sine();
    constexpr float sr = 48000.0f;

    // Land both at phase 0.25, freeze, then compare warped vs unwarped read.
    WavetableOsc plain (table);
    plain.setSampleRate (sr);
    plain.setUnison (1, 0.0f);
    plain.setWarp (WarpMode::off, 0.0f);
    plain.setFrequency (0.25f * sr);
    plain.reset();
    plain.process();
    plain.setFrequency (0.0f);

    WavetableOsc bent (table);
    bent.setSampleRate (sr);
    bent.setUnison (1, 0.0f);
    bent.setWarp (WarpMode::bendPlus, 0.9f);
    bent.setFrequency (0.25f * sr);
    bent.reset();
    bent.process();
    bent.setFrequency (0.0f);

    const float plainSample = plain.process();
    const float bentSample = bent.process();

    REQUIRE (plainSample == Catch::Approx (table.lookup (0.25f)).margin (1.0e-4f));
    REQUIRE (bentSample != Catch::Approx (plainSample).margin (0.01f));
    REQUIRE (std::isfinite (bentSample));
}
