#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "dsp/Wavetable.h"
#include "dsp/WavetableOsc.h"
#include "dsp/WavetableMipBank.h"

#include <cmath>
#include <algorithm>

// Covers the multi-osc DSP pattern used by SynthVoice (mix + independent tables/freqs)
// without linking JUCE SynthesiserVoice.

TEST_CASE ("mixed oscillators weight each table by level", "[multi-osc]")
{
    const auto sine = Wavetable::sine();
    const auto saw = Wavetable::sawBandLimited (64);

    WavetableOsc osc1 (sine);
    WavetableOsc osc2 (saw);
    osc1.setSampleRate (48000.0f);
    osc2.setSampleRate (48000.0f);
    osc1.setUnison (1, 0.0f);
    osc2.setUnison (1, 0.0f);

    // Advance both to phase 0.25, then freeze.
    osc1.setFrequency (0.25f * 48000.0f);
    osc2.setFrequency (0.25f * 48000.0f);
    osc1.reset();
    osc2.reset();
    osc1.process();
    osc2.process();
    osc1.setFrequency (0.0f);
    osc2.setFrequency (0.0f);

    constexpr float level1 = 0.7f;
    constexpr float level2 = 0.3f;

    const float s1 = osc1.process();
    const float s2 = osc2.process();
    const float mixed = s1 * level1 + s2 * level2;

    REQUIRE (s1 == Catch::Approx (sine.lookup (0.25f)).margin (1.0e-4f));
    REQUIRE (s2 == Catch::Approx (saw.lookup (0.25f)).margin (1.0e-4f));
    REQUIRE (mixed == Catch::Approx (s1 * level1 + s2 * level2).margin (1.0e-5f));
    REQUIRE (s1 != Catch::Approx (s2).margin (1.0e-3f));
}

TEST_CASE ("detuned oscillators diverge in phase", "[multi-osc]")
{
    const auto table = Wavetable::sine();
    WavetableOsc oscA (table);
    WavetableOsc oscB (table);

    constexpr float sr = 48000.0f;
    oscA.setSampleRate (sr);
    oscB.setSampleRate (sr);
    oscA.setUnison (1, 0.0f);
    oscB.setUnison (1, 0.0f);
    oscA.reset();
    oscB.reset();

    constexpr float f0 = 440.0f;
    const float detuneRatio = std::pow (2.0f, 7.0f / 1200.0f); // +7 cents
    oscA.setFrequency (f0);
    oscB.setFrequency (f0 * detuneRatio);

    float maxDiff = 0.0f;
    for (int i = 0; i < 2048; ++i)
        maxDiff = std::max (maxDiff, std::abs (oscA.process() - oscB.process()));

    REQUIRE (maxDiff > 0.1f);
}

TEST_CASE ("independent tables: sine vs square mip selection", "[multi-osc][mip]")
{
    const auto sine = Wavetable::sine();

    WavetableMipBank squareBank;
    squareBank.build (WavetableMipBank::Shape::square);

    constexpr float sr = 48000.0f;
    constexpr float freq = 440.0f;

    WavetableOsc oscSine (sine);
    WavetableOsc oscSquare (squareBank.select (freq, sr));
    oscSine.setSampleRate (sr);
    oscSquare.setSampleRate (sr);
    oscSine.setUnison (1, 0.0f);
    oscSquare.setUnison (1, 0.0f);
    oscSine.setFrequency (0.0f);
    oscSquare.setFrequency (0.0f);
    oscSine.reset();
    oscSquare.reset();

    // Hold at phase 0.25 via one large step then freeze.
    oscSine.setFrequency (0.25f * sr);
    oscSquare.setFrequency (0.25f * sr);
    oscSine.process();
    oscSquare.process();
    oscSine.setFrequency (0.0f);
    oscSquare.setFrequency (0.0f);

    const float fromSine = oscSine.process();
    const float fromSquare = oscSquare.process();

    REQUIRE (fromSine == Catch::Approx (sine.lookup (0.25f)).margin (1.0e-4f));
    REQUIRE (fromSquare > 0.0f);
    REQUIRE (fromSine != Catch::Approx (fromSquare).margin (0.05f));
}

TEST_CASE ("band-limited saw with one harmonic matches sine shape", "[wavetable][multi-osc]")
{
    const auto sine = Wavetable::sine();
    const auto saw1 = Wavetable::sawBandLimited (1);

    REQUIRE (saw1.lookup (0.25f) == Catch::Approx (sine.lookup (0.25f)).margin (0.05f));
    REQUIRE (saw1.lookup (0.75f) == Catch::Approx (sine.lookup (0.75f)).margin (0.05f));
}
