#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "dsp/Envelope.h"

TEST_CASE("envelope attack raises level then reaches sustain", "[envelope]")
{
    Envelope env;
    env.setSampleRate(1000.0); // easy mental math: 1 sample = 1 ms
    env.setParameters({0.01f, 0.01f, 0.5f, 0.05f}); // 10ms attack/decay, sustain 0.5

    REQUIRE_FALSE(env.isActive());

    env.noteOn();
    REQUIRE(env.isActive());

    float prev = 0.0f;
    for (int i = 0; i < 10; ++i) // attack window
    {
        const float level = env.getNextSample();
        REQUIRE(level >= prev - 1.0e-6f);
        prev = level;
    }

    // Finish decay into sustain
    for (int i = 0; i < 50; ++i)
        env.getNextSample();

    REQUIRE(env.getNextSample() == Catch::Approx(0.5f).margin(0.05f));
}

TEST_CASE("envelope release becomes inactive", "[envelope]")
{
    Envelope env;
    env.setSampleRate(1000.0);
    env.setParameters({0.001f, 0.001f, 1.0f, 0.02f});

    env.noteOn();
    for (int i = 0; i < 20; ++i)
        env.getNextSample();

    env.noteOff();
    REQUIRE(env.isActive());

    for (int i = 0; i < 100; ++i)
        env.getNextSample();

    REQUIRE_FALSE(env.isActive());
    REQUIRE(env.getNextSample() == Catch::Approx(0.0f).margin(1.0e-6f));
}
