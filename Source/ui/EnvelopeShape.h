#pragma once

#include "dsp/EnvelopeCurve.h"
#include <juce_graphics/juce_graphics.h>
#include <array>

// Geometry helper for the ADSR plot. Timed corners + Bezier curve mid-handles.
struct EnvelopeShape
{
    struct Params
    {
        float attack = 0.01f;
        float decay = 0.1f;
        float sustain = 0.8f; // 0..1
        float release = 0.2f;
        EnvelopeCurve::Handle attackCurve {};
        EnvelopeCurve::Handle decayCurve {};
        EnvelopeCurve::Handle releaseCurve {};
    };

    // Normalized plot coords: x in [0,1] across the component, y = level (0..1, up).
    struct Point
    {
        float x = 0.0f;
        float y = 0.0f;
    };

    static constexpr float sustainFraction = 0.22f;
    static constexpr int numCornerPoints = 5;
    static constexpr int curveSamplesPerSegment = 24;

    // Corner handles (time / sustain).
    enum Handle : int
    {
        start = 0,
        attackPeak = 1,
        decaySustain = 2,
        sustainEnd = 3,
        releaseEnd = 4,
        // Mid-segment curve handles (free 2D drag within the segment).
        attackCurve = 5,
        decayCurve = 6,
        releaseCurve = 7
    };

    static std::array<Point, numCornerPoints>
    buildPoints (const Params& p, const juce::NormalisableRange<float>& timeRange);

    // On-curve Bezier handles in plot-normalized coords.
    static std::array<Point, 3> buildCurveHandles (const Params& p,
                                                   const juce::NormalisableRange<float>& timeRange);

    static juce::Path buildPath (juce::Rectangle<float> bounds, const Params& p,
                                 const juce::NormalisableRange<float>& timeRange);

    static juce::Point<float> toPixels (Point n, juce::Rectangle<float> bounds);
    static Point fromPixels (juce::Point<float> px, juce::Rectangle<float> bounds);

    // Map a plot point on a segment into a clamped Bezier handle.
    static EnvelopeCurve::Handle handleFromSegmentPoint (Point segmentStart, Point segmentEnd,
                                                         Point plotPoint);
};
