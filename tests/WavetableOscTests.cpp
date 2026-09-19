#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "dsp/Wavetable.h"
#include "dsp/WavetableOsc.h"

#include <cmath>

TEST_CASE("oscillator advances one cycle in sr/freq samples", "[osc]")
{
    const auto table = Wavetable::sine();
    WavetableOsc osc(table);
    osc.setSampleRate(44100.0f);
    osc.setFrequency(441.0f); // increment = 0.01 exactly
    osc.reset();

    const float first = osc.process();

    // 99 more advances → 100 increments of 0.01 → phase wrapped to 0
    for (int i = 0; i < 99; ++i)
        osc.process();

    const float afterOnePeriod = osc.process();
    REQUIRE(afterOnePeriod == Catch::Approx(first).margin(1.0e-4f));
}

TEST_CASE("oscillator output is finite", "[osc]")
{
    const auto table = Wavetable::sine();
    WavetableOsc osc(table);
    osc.setSampleRate(48000.0f);
    osc.setFrequency(440.0f);

    for (int i = 0; i < 512; ++i)
        REQUIRE(std::isfinite(osc.process()));
}

TEST_CASE("setTable switches the waveform being read", "[osc]")
{
    const auto sine = Wavetable::sine();
    const auto saw = Wavetable::saw();

    WavetableOsc osc(sine);
    osc.setSampleRate(48000.0f);
    osc.setFrequency(0.0f); // hold phase so lookup is deterministic
    osc.reset();

    const float fromSine = osc.process();
    REQUIRE(fromSine == Catch::Approx(sine.lookup(0.0f)).margin(1.0e-5f));

    osc.setTable(saw);
    osc.reset();

    const float fromSaw = osc.process();
    REQUIRE(fromSaw == Catch::Approx(saw.lookup(0.0f)).margin(1.0e-5f));
    REQUIRE(fromSaw != Catch::Approx(fromSine).margin(1.0e-3f));
}
