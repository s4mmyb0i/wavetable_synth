#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "dsp/EnvelopeCurve.h"

TEST_CASE ("envelope curve shape endpoints and linear handle", "[envelope-curve]")
{
    const EnvelopeCurve::Handle linear {0.5f, 0.5f};

    REQUIRE (EnvelopeCurve::shape (0.0f, linear) == Catch::Approx (0.0f));
    REQUIRE (EnvelopeCurve::shape (1.0f, linear) == Catch::Approx (1.0f));
    REQUIRE (EnvelopeCurve::shape (0.5f, linear) == Catch::Approx (0.5f).margin (1.0e-5f));

    // Off-centre handle still hits endpoints.
    REQUIRE (EnvelopeCurve::shape (0.0f, {0.25f, 0.2f}) == Catch::Approx (0.0f));
    REQUIRE (EnvelopeCurve::shape (1.0f, {0.75f, 0.8f}) == Catch::Approx (1.0f));
}

TEST_CASE ("envelope curve passes through handle", "[envelope-curve]")
{
    const EnvelopeCurve::Handle h = EnvelopeCurve::clampHandle ({0.3f, 0.45f});
    REQUIRE (EnvelopeCurve::shape (h.x, h) == Catch::Approx (h.y).margin (1.0e-4f));
}

TEST_CASE ("envelope curve shape is monotonic in t", "[envelope-curve]")
{
    const EnvelopeCurve::Handle handles[] = {
        {0.5f, 0.5f},
        {0.25f, EnvelopeCurve::minYForX (0.25f)},
        {0.25f, EnvelopeCurve::maxYForX (0.25f)},
        {0.75f, EnvelopeCurve::minYForX (0.75f)},
        {0.75f, EnvelopeCurve::maxYForX (0.75f)},
    };

    for (const auto& h : handles)
    {
        float prev = EnvelopeCurve::shape (0.0f, h);
        for (int i = 1; i <= 20; ++i)
        {
            const float t = static_cast<float> (i) / 20.0f;
            const float y = EnvelopeCurve::shape (t, h);
            REQUIRE (y >= prev - 1.0e-6f);
            prev = y;
        }
    }
}

TEST_CASE ("handle below diagonal is slow-then-fast at mid", "[envelope-curve]")
{
    // At x=0.5, minY=0.25 (bowed below linear), maxY=0.75 (above linear).
    REQUIRE (EnvelopeCurve::shape (0.5f, {0.5f, 0.25f})
             < EnvelopeCurve::shape (0.5f, {0.5f, 0.5f}));
    REQUIRE (EnvelopeCurve::shape (0.5f, {0.5f, 0.75f})
             > EnvelopeCurve::shape (0.5f, {0.5f, 0.5f}));
}

TEST_CASE ("clampHandle keeps C in monotonic range", "[envelope-curve]")
{
    const auto h = EnvelopeCurve::clampHandle ({0.5f, 0.99f});
    REQUIRE (h.y == Catch::Approx (EnvelopeCurve::maxYForX (0.5f)));
    REQUIRE (EnvelopeCurve::controlFromHandle (h) <= 1.0f + 1.0e-6f);
}
