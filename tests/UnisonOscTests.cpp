#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "dsp/Wavetable.h"
#include "dsp/UnisonOsc.h"

#include <cmath>
#include <algorithm>

namespace
{
UnisonOsc makeMono (const Wavetable& table, float sr, float freq)
{
    UnisonOsc osc (table);
    osc.setSampleRate (sr);
    osc.setUnison (1, 0.0f);
    osc.setFrequency (freq);
    osc.reset();
    return osc;
}
} // namespace

TEST_CASE ("unison count 1 advances one cycle in sr/freq samples", "[unison][osc]")
{
    const auto table = Wavetable::sine();
    UnisonOsc osc = makeMono (table, 44100.0f, 441.0f); // increment = 0.01 exactly

    const float first = osc.process();

    // 99 more advances → 100 increments of 0.01 → phase wrapped to 0
    for (int i = 0; i < 99; ++i) osc.process();

    const float afterOnePeriod = osc.process();
    REQUIRE (afterOnePeriod == Catch::Approx (first).margin (1.0e-4f));
}

TEST_CASE ("unison count 1 detune is ignored", "[unison]")
{
    const auto table = Wavetable::sine();

    constexpr float sr = 48000.0f;
    constexpr float freq = 440.0f;

    UnisonOsc withDetune (table);
    UnisonOsc mono = makeMono (table, sr, freq);

    withDetune.setSampleRate (sr);
    withDetune.setUnison (1, 12.0f);
    withDetune.setFrequency (freq);
    withDetune.reset();

    for (int i = 0; i < 512; ++i)
        REQUIRE (withDetune.process() == Catch::Approx (mono.process()).margin (1.0e-5f));
}

TEST_CASE ("unison with zero detune equals mono regardless of count", "[unison]")
{
    const auto table = Wavetable::sine();

    constexpr float sr = 48000.0f;
    constexpr float freq = 220.0f;

    UnisonOsc uni (table);
    UnisonOsc mono = makeMono (table, sr, freq);

    uni.setSampleRate (sr);
    uni.setUnison (5, 0.0f);
    uni.setFrequency (freq);
    uni.reset();

    for (int i = 0; i < 1024; ++i)
        REQUIRE (uni.process() == Catch::Approx (mono.process()).margin (1.0e-5f));
}

TEST_CASE ("unison with detune diverges from mono", "[unison]")
{
    const auto table = Wavetable::sine();

    constexpr float sr = 48000.0f;
    constexpr float freq = 440.0f;

    UnisonOsc uni (table);
    UnisonOsc mono = makeMono (table, sr, freq);

    uni.setSampleRate (sr);
    uni.setUnison (5, 20.0f);
    uni.setFrequency (freq);
    uni.reset();

    float maxDiff = 0.0f;
    for (int i = 0; i < 4096; ++i)
        maxDiff = std::max (maxDiff, std::abs (uni.process() - mono.process()));

    REQUIRE (maxDiff > 0.1f);
}

TEST_CASE ("unison output stays finite and roughly bounded", "[unison]")
{
    const auto table = Wavetable::sine();
    UnisonOsc uni (table);
    uni.setSampleRate (48000.0f);
    uni.setUnison (8, 25.0f);
    uni.setFrequency (880.0f);
    uni.reset();

    for (int i = 0; i < 2048; ++i)
    {
        const float s = uni.process();
        REQUIRE (std::isfinite (s));
        REQUIRE (std::abs (s) < 2.0f);
    }
}

TEST_CASE ("setUnison clamps count below 1 and above 8", "[unison]")
{
    const auto table = Wavetable::sine();
    UnisonOsc uni (table);
    uni.setSampleRate (48000.0f);
    uni.setFrequency (440.0f);

    uni.setUnison (0, 10.0f);
    uni.reset();
    REQUIRE (std::isfinite (uni.process()));

    uni.setUnison (100, 10.0f);
    uni.reset();
    for (int i = 0; i < 256; ++i)
    {
        const float s = uni.process();
        REQUIRE (std::isfinite (s));
        REQUIRE (std::abs (s) < 2.0f);
    }
}

TEST_CASE ("setTable switches waveform under unison", "[unison]")
{
    const auto sine = Wavetable::sine();
    const auto saw = Wavetable::sawBandLimited (64);

    constexpr float sr = 48000.0f;
    UnisonOsc uni (sine);
    uni.setSampleRate (sr);
    uni.setUnison (1, 0.0f);

    uni.setFrequency (0.25f * sr);
    uni.reset();
    uni.process();
    uni.setFrequency (0.0f);

    const float fromSine = uni.process();
    REQUIRE (fromSine == Catch::Approx (sine.lookup (0.25f)).margin (1.0e-5f));

    uni.setTable (saw);
    const float fromSaw = uni.process();
    REQUIRE (fromSaw == Catch::Approx (saw.lookup (0.25f)).margin (1.0e-5f));
    REQUIRE (fromSaw != Catch::Approx (fromSine).margin (1.0e-3f));
}

TEST_CASE ("independent UnisonOsc slots can use different unison settings", "[unison][multi-osc]")
{
    const auto table = Wavetable::sine();

    constexpr float sr = 48000.0f;
    constexpr float freq = 440.0f;

    UnisonOsc osc1 (table);
    UnisonOsc osc2 (table);
    UnisonOsc osc3 (table);

    for (auto* o : {&osc1, &osc2, &osc3})
    {
        o->setSampleRate (sr);
        o->setFrequency (freq);
        o->reset();
    }

    osc1.setUnison (1, 0.0f);
    osc2.setUnison (3, 15.0f);
    osc3.setUnison (7, 30.0f);

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

TEST_CASE ("reset returns unison voices to phase 0", "[unison]")
{
    const auto table = Wavetable::sine();
    UnisonOsc uni (table);
    uni.setSampleRate (48000.0f);
    uni.setUnison (4, 12.0f);
    uni.setFrequency (440.0f);

    for (int i = 0; i < 100; ++i) uni.process();

    uni.reset();
    const float afterReset = uni.process();

    UnisonOsc fresh (table);
    fresh.setSampleRate (48000.0f);
    fresh.setUnison (4, 12.0f);
    fresh.setFrequency (440.0f);
    fresh.reset();
    const float fromFresh = fresh.process();

    REQUIRE (afterReset == Catch::Approx (fromFresh).margin (1.0e-5f));
}
