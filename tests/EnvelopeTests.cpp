#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "dsp/Envelope.h"

namespace
{
Envelope::Parameters makeParams (float attack, float decay, float sustain, float release)
{
    Envelope::Parameters p;
    p.attackSeconds = attack;
    p.decaySeconds = decay;
    p.sustainLevel = sustain;
    p.releaseSeconds = release;
    return p;
}
} // namespace

TEST_CASE ("envelope attack raises level then reaches sustain", "[envelope]")
{
    Envelope env;
    env.setSampleRate (1000.0); // 1 sample = 1 ms
    env.setParameters (makeParams (0.01f, 0.01f, 0.5f, 0.05f));

    REQUIRE_FALSE (env.isActive());

    env.noteOn();
    REQUIRE (env.isActive());

    float prev = 0.0f;
    for (int i = 0; i < 10; ++i) // attack window
    {
        const float level = env.getNextSample();
        REQUIRE (level >= prev - 1.0e-6f);
        prev = level;
    }

    for (int i = 0; i < 50; ++i) env.getNextSample();

    REQUIRE (env.getNextSample() == Catch::Approx (0.5f).margin (0.05f));
}

TEST_CASE ("envelope release becomes inactive", "[envelope]")
{
    Envelope env;
    env.setSampleRate (1000.0);
    env.setParameters (makeParams (0.001f, 0.001f, 1.0f, 0.02f));

    env.noteOn();
    for (int i = 0; i < 20; ++i) env.getNextSample();

    env.noteOff();
    REQUIRE (env.isActive());

    for (int i = 0; i < 100; ++i) env.getNextSample();

    REQUIRE_FALSE (env.isActive());
    REQUIRE (env.getNextSample() == Catch::Approx (0.0f).margin (1.0e-6f));
}

TEST_CASE ("setParameters during release keeps release duration", "[envelope]")
{
    Envelope env;
    env.setSampleRate (1000.0);
    env.setParameters (makeParams (0.001f, 0.001f, 0.5f, 0.020f));

    env.noteOn();
    for (int i = 0; i < 30; ++i) env.getNextSample();

    env.noteOff();

    for (int i = 0; i < 15; ++i)
    {
        env.setParameters (makeParams (0.001f, 0.001f, 0.5f, 0.020f));
        env.getNextSample();
    }

    REQUIRE (env.isActive());
    REQUIRE (env.getNextSample() > 0.05f);

    for (int i = 0; i < 20; ++i)
    {
        env.setParameters (makeParams (0.001f, 0.001f, 0.5f, 0.020f));
        env.getNextSample();
    }

    REQUIRE_FALSE (env.isActive());
}

TEST_CASE ("soft retrigger continues from current level", "[envelope]")
{
    Envelope env;
    env.setSampleRate (1000.0);
    env.setParameters (makeParams (0.100f, 0.001f, 1.0f, 0.050f));

    env.noteOn();
    for (int i = 0; i < 50; ++i) env.getNextSample();

    const float before = env.getNextSample();
    REQUIRE (before == Catch::Approx (0.51f).margin (0.05f));

    env.noteOn();
    const float after = env.getNextSample();
    REQUIRE (after >= before - 1.0e-5f);
    REQUIRE (after < 0.75f);
}

TEST_CASE ("curved attack still reaches peak near attack time", "[envelope]")
{
    Envelope env;
    env.setSampleRate (1000.0);
    auto p = makeParams (0.020f, 0.001f, 1.0f, 0.05f);
    p.attackCurveX = 0.5f;
    p.attackCurveY = 0.25f; // slow-then-fast (below linear)
    env.setParameters (p);

    env.noteOn();
    for (int i = 0; i < 19; ++i) env.getNextSample();

    REQUIRE (env.getNextSample() == Catch::Approx (1.0f).margin (0.05f));
}

TEST_CASE ("slow-then-fast attack sits below linear at mid segment", "[envelope]")
{
    Envelope linear, curved;
    linear.setSampleRate (1000.0);
    curved.setSampleRate (1000.0);
    linear.setParameters (makeParams (0.100f, 0.001f, 1.0f, 0.05f));
    auto cp = makeParams (0.100f, 0.001f, 1.0f, 0.05f);
    cp.attackCurveX = 0.5f;
    cp.attackCurveY = 0.25f;
    curved.setParameters (cp);

    linear.noteOn();
    curved.noteOn();
    for (int i = 0; i < 50; ++i)
    {
        linear.getNextSample();
        curved.getNextSample();
    }

    const float lin = linear.getNextSample();
    const float crv = curved.getNextSample();
    REQUIRE (crv < lin - 0.05f);
}
