#include "ui/EnvelopeShape.h"

std::array<EnvelopeShape::Point, EnvelopeShape::numPoints>
EnvelopeShape::buildPoints (const Params& p, const juce::NormalisableRange<float>& timeRange)
{
    const float adr = 1.0f - sustainFraction;
    const float band = adr / 3.0f;

    const float a0 = 0.0f;
    const float d0 = band;
    const float s0 = 2.0f * band;
    const float r0 = 2.0f * band + sustainFraction;

    const float aN = timeRange.convertTo0to1 (timeRange.snapToLegalValue (p.attack));
    const float dN = timeRange.convertTo0to1 (timeRange.snapToLegalValue (p.decay));
    const float rN = timeRange.convertTo0to1 (timeRange.snapToLegalValue (p.release));
    const float s = juce::jlimit (0.0f, 1.0f, p.sustain);

    return {{
        {a0, 0.0f},
        {a0 + band * aN, 1.0f},
        {d0 + band * dN, s},
        {s0 + sustainFraction, s},
        {r0 + band * rN, 0.0f},
    }};
}

juce::Point<float> EnvelopeShape::toPixels (Point n, juce::Rectangle<float> bounds)
{ return {bounds.getX() + n.x * bounds.getWidth(), bounds.getBottom() - n.y * bounds.getHeight()}; }

EnvelopeShape::Point EnvelopeShape::fromPixels (juce::Point<float> px,
                                                juce::Rectangle<float> bounds)
{
    const float w = juce::jmax (1.0f, bounds.getWidth());
    const float h = juce::jmax (1.0f, bounds.getHeight());
    return {(px.x - bounds.getX()) / w, (bounds.getBottom() - px.y) / h};
}

juce::Path EnvelopeShape::buildPath (juce::Rectangle<float> bounds, const Params& p,
                                     const juce::NormalisableRange<float>& timeRange)
{
    const auto pts = buildPoints (p, timeRange);
    juce::Path path;
    const auto p0 = toPixels (pts[0], bounds);
    path.startNewSubPath (p0);

    // Straight segments for now — replace with curves when DSP supports them.
    for (int i = 1; i < numPoints; ++i)
        path.lineTo (toPixels (pts[static_cast<size_t> (i)], bounds));

    return path;
}
