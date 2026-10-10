#pragma once

#include <juce_graphics/juce_graphics.h>
#include <array>

// Geometry helper for the ADSR plot. Straight segments today; swap buildPath
// interpolation later when curved envelope DSP lands.
struct EnvelopeShape
{
    struct Params
    {
        float attack = 0.01f;
        float decay = 0.1f;
        float sustain = 0.8f; // 0..1
        float release = 0.2f;
    };

    // Normalized plot coords: x in [0,1] across the component, y = level (0..1, up).
    struct Point
    {
        float x = 0.0f;
        float y = 0.0f;
    };

    static constexpr float sustainFraction = 0.22f;
    static constexpr int numPoints = 5;

    // Indices into the point array.
    enum Handle : int
    {
        start = 0,
        attackPeak = 1,
        decaySustain = 2,
        sustainEnd = 3,
        releaseEnd = 4
    };

    // Equal A/D/R bands share (1 - sustainFraction); sustain band is fixed width.
    static std::array<Point, numPoints>
    buildPoints (const Params& p, const juce::NormalisableRange<float>& timeRange);

    static juce::Path buildPath (juce::Rectangle<float> bounds, const Params& p,
                                 const juce::NormalisableRange<float>& timeRange);

    static juce::Point<float> toPixels (Point n, juce::Rectangle<float> bounds);
    static Point fromPixels (juce::Point<float> px, juce::Rectangle<float> bounds);
};
