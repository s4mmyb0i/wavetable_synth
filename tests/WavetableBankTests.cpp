#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "dsp/Wavetable.h"
#include "dsp/WavetableBank.h"
#include "dsp/WavetableOsc.h"

TEST_CASE ("bank position 0 matches first frame", "[bank]")
{
    const auto bank = WavetableBank::makeBasicMorph();
    constexpr float phase = 0.25f;

    REQUIRE (bank.lookup (0.0f, phase)
             == Catch::Approx (bank.frame (0).lookup (phase)).margin (1.0e-5f));
}

TEST_CASE ("bank position 1 matches last frame", "[bank]")
{
    const auto bank = WavetableBank::makeBasicMorph();
    constexpr float phase = 0.25f;
    const int last = bank.getNumFrames() - 1;

    REQUIRE (bank.lookup (1.0f, phase)
             == Catch::Approx (bank.frame (last).lookup (phase)).margin (1.0e-5f));
}

TEST_CASE ("bank midpoint blends neighboring frames", "[bank]")
{
    WavetableBank bank;
    bank.setNumFrames (2);
    bank.frame (0) = Wavetable::sine();
    bank.frame (1) = Wavetable::triangle();

    constexpr float phase = 0.25f;
    const float a = bank.frame (0).lookup (phase);
    const float b = bank.frame (1).lookup (phase);
    const float mid = bank.lookup (0.5f, phase);

    REQUIRE (mid == Catch::Approx (0.5f * (a + b)).margin (1.0e-5f));
}

TEST_CASE ("osc with bank morphs when position changes", "[bank][osc]")
{
    const auto bank = WavetableBank::makeBasicMorph();
    constexpr float sr = 48000.0f;

    WavetableOsc atStart (Wavetable::sine());
    atStart.setSampleRate (sr);
    atStart.setUnison (1, 0.0f);
    atStart.setBank (bank);
    atStart.setPosition (0.0f);
    atStart.setFrequency (0.25f * sr);
    atStart.reset();
    atStart.process();
    atStart.setFrequency (0.0f);

    WavetableOsc atEnd (Wavetable::sine());
    atEnd.setSampleRate (sr);
    atEnd.setUnison (1, 0.0f);
    atEnd.setBank (bank);
    atEnd.setPosition (1.0f);
    atEnd.setFrequency (0.25f * sr);
    atEnd.reset();
    atEnd.process();
    atEnd.setFrequency (0.0f);

    const float s0 = atStart.process();
    const float s1 = atEnd.process();

    REQUIRE (s0 == Catch::Approx (bank.frame (0).lookup (0.25f)).margin (1.0e-4f));
    REQUIRE (s1 == Catch::Approx (bank.frame (bank.getNumFrames() - 1).lookup (0.25f)).margin (1.0e-4f));
    REQUIRE (s0 != Catch::Approx (s1).margin (0.05f));
}

TEST_CASE ("setTable clears bank path", "[bank][osc]")
{
    const auto bank = WavetableBank::makeBasicMorph();
    const auto sine = Wavetable::sine();
    constexpr float sr = 48000.0f;

    WavetableOsc osc (sine);
    osc.setSampleRate (sr);
    osc.setUnison (1, 0.0f);
    osc.setBank (bank);
    osc.setPosition (1.0f);
    osc.setTable (sine); // back to single-table mode

    osc.setFrequency (0.25f * sr);
    osc.reset();
    osc.process();
    osc.setFrequency (0.0f);

    REQUIRE (osc.process() == Catch::Approx (sine.lookup (0.25f)).margin (1.0e-4f));
}
