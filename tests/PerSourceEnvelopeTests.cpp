#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "dsp/Envelope.h"

#include <algorithm>
#include <cmath>

// Mirrors SynthVoice's per-source amp envelopes (osc1–3 + noise) without linking
// JUCE SynthesiserVoice: each source is scaled by its own Envelope, then mixed.

namespace
{
bool anyActive (const Envelope& a, const Envelope& b, const Envelope& c, const Envelope& d)
{ return a.isActive() || b.isActive() || c.isActive() || d.isActive(); }

void noteOnAll (Envelope& a, Envelope& b, Envelope& c, Envelope& d)
{
    a.noteOn();
    b.noteOn();
    c.noteOn();
    d.noteOn();
}

void noteOffAll (Envelope& a, Envelope& b, Envelope& c, Envelope& d)
{
    a.noteOff();
    b.noteOff();
    c.noteOff();
    d.noteOff();
}

void advance (Envelope& env, int samples)
{
    for (int i = 0; i < samples; ++i) env.getNextSample();
}

float mixWithEnvelopes (float s1, float s2, float s3, float sn, Envelope& e1, Envelope& e2,
                        Envelope& e3, Envelope& e4)
{
    return s1 * e1.getNextSample() + s2 * e2.getNextSample() + s3 * e3.getNextSample() +
           sn * e4.getNextSample();
}
} // namespace

TEST_CASE ("per-source envelopes keep independent parameters", "[per-source-env]")
{
    Envelope e1, e2, e3, e4;
    constexpr double sr = 1000.0; // 1 sample = 1 ms
    for (auto* e : {&e1, &e2, &e3, &e4}) e->setSampleRate (sr);

    // e1: very short attack → near 1 quickly; e2: long attack → still climbing
    e1.setParameters ({0.005f, 0.001f, 1.0f, 0.05f});
    e2.setParameters ({0.100f, 0.001f, 1.0f, 0.05f});
    e3.setParameters ({0.001f, 0.001f, 0.25f, 0.05f});
    e4.setParameters ({0.001f, 0.001f, 0.75f, 0.05f});

    noteOnAll (e1, e2, e3, e4);

    // After 10 ms: e1 should be past attack; e2 still mid-attack.
    advance (e1, 10);
    advance (e2, 10);
    advance (e3, 30); // settle into sustain
    advance (e4, 30);

    const float l1 = e1.getNextSample();
    const float l2 = e2.getNextSample();
    const float l3 = e3.getNextSample();
    const float l4 = e4.getNextSample();

    REQUIRE (l1 > 0.9f);
    REQUIRE (l2 < 0.25f);
    REQUIRE (l1 > l2 + 0.5f);
    REQUIRE (l3 == Catch::Approx (0.25f).margin (0.05f));
    REQUIRE (l4 == Catch::Approx (0.75f).margin (0.05f));
}

TEST_CASE ("per-source mix scales each source by its own envelope", "[per-source-env]")
{
    Envelope e1, e2, e3, e4;
    constexpr double sr = 1000.0;
    for (auto* e : {&e1, &e2, &e3, &e4}) e->setSampleRate (sr);

    // Instant attack/decay so we sit on known sustain levels.
    e1.setParameters ({0.001f, 0.001f, 1.0f, 0.05f});
    e2.setParameters ({0.001f, 0.001f, 0.5f, 0.05f});
    e3.setParameters ({0.001f, 0.001f, 0.0f, 0.05f});
    e4.setParameters ({0.001f, 0.001f, 0.25f, 0.05f});

    noteOnAll (e1, e2, e3, e4);
    for (auto* e : {&e1, &e2, &e3, &e4}) advance (*e, 20);

    constexpr float s1 = 1.0f, s2 = 1.0f, s3 = 1.0f, sn = 1.0f;
    const float mixed = mixWithEnvelopes (s1, s2, s3, sn, e1, e2, e3, e4);

    // Expected: 1*1 + 1*0.5 + 1*0 + 1*0.25 = 1.75
    REQUIRE (mixed == Catch::Approx (1.75f).margin (0.08f));
}

TEST_CASE ("changing one envelope does not alter another source contribution", "[per-source-env]")
{
    Envelope e1, e2;
    constexpr double sr = 1000.0;
    e1.setSampleRate (sr);
    e2.setSampleRate (sr);

    e1.setParameters ({0.001f, 0.001f, 1.0f, 0.05f});
    e2.setParameters ({0.001f, 0.001f, 1.0f, 0.05f});
    e1.noteOn();
    e2.noteOn();
    advance (e1, 20);
    advance (e2, 20);

    constexpr float source1 = 0.8f;
    constexpr float source2 = 0.6f;

    const float before1 = source1 * e1.getNextSample();
    const float before2 = source2 * e2.getNextSample();

    // Retarget only e1 to a lower sustain and re-trigger so it reaches the new level.
    e1.setParameters ({0.001f, 0.001f, 0.2f, 0.05f});
    e1.noteOn();
    advance (e1, 20);

    // Keep e2 advancing in parallel (same stage); contribution should stay ~same.
    advance (e2, 20);

    const float after1 = source1 * e1.getNextSample();
    const float after2 = source2 * e2.getNextSample();

    REQUIRE (after1 < before1 * 0.5f);
    REQUIRE (after2 == Catch::Approx (before2).margin (0.05f));
}

TEST_CASE ("voice stays active while any per-source envelope is releasing", "[per-source-env]")
{
    Envelope e1, e2, e3, e4;
    constexpr double sr = 1000.0;
    for (auto* e : {&e1, &e2, &e3, &e4}) e->setSampleRate (sr);

    // Short releases on 1–3; long release on noise (e4).
    e1.setParameters ({0.001f, 0.001f, 1.0f, 0.010f});
    e2.setParameters ({0.001f, 0.001f, 1.0f, 0.010f});
    e3.setParameters ({0.001f, 0.001f, 1.0f, 0.010f});
    e4.setParameters ({0.001f, 0.001f, 1.0f, 0.200f});

    noteOnAll (e1, e2, e3, e4);
    for (auto* e : {&e1, &e2, &e3, &e4}) advance (*e, 20);

    REQUIRE (anyActive (e1, e2, e3, e4));

    noteOffAll (e1, e2, e3, e4);

    // After 40 ms: osc envelopes done; noise still releasing.
    for (int i = 0; i < 40; ++i) (void) mixWithEnvelopes (1.0f, 1.0f, 1.0f, 1.0f, e1, e2, e3, e4);

    REQUIRE_FALSE (e1.isActive());
    REQUIRE_FALSE (e2.isActive());
    REQUIRE_FALSE (e3.isActive());
    REQUIRE (e4.isActive());
    REQUIRE (anyActive (e1, e2, e3, e4));

    // Finish noise release.
    for (int i = 0; i < 250; ++i) (void) mixWithEnvelopes (1.0f, 1.0f, 1.0f, 1.0f, e1, e2, e3, e4);

    REQUIRE_FALSE (anyActive (e1, e2, e3, e4));
}

TEST_CASE ("silent source still advances its envelope independently", "[per-source-env]")
{
    Envelope e1, e2;
    constexpr double sr = 1000.0;
    e1.setSampleRate (sr);
    e2.setSampleRate (sr);

    e1.setParameters ({0.001f, 0.001f, 1.0f, 0.05f});
    e2.setParameters ({0.050f, 0.001f, 1.0f, 0.05f}); // slower attack
    e1.noteOn();
    e2.noteOn();

    // Osc2 level is 0 (silent) but its envelope must still run so timing stays correct
    // if the level is raised later — same as SynthVoice always calling getNextSample().
    constexpr float level1 = 1.0f;
    constexpr float level2 = 0.0f;

    float lastEnv2 = 0.0f;
    for (int i = 0; i < 25; ++i)
    {
        const float env1 = e1.getNextSample();
        const float env2 = e2.getNextSample();
        const float mixed = level1 * env1 + level2 * env2;
        REQUIRE (mixed == Catch::Approx (env1).margin (1.0e-5f));
        lastEnv2 = env2;
    }

    REQUIRE (lastEnv2 > 0.3f);
    REQUIRE (lastEnv2 < 0.7f);
}
