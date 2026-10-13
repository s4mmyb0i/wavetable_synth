#include "ui/EnvelopeShape.h"

namespace
{
void appendShapedSegment (juce::Path& path, juce::Rectangle<float> bounds,
                          EnvelopeShape::Point a, EnvelopeShape::Point b,
                          EnvelopeCurve::Handle curve)
{
    for (int i = 1; i <= EnvelopeShape::curveSamplesPerSegment; ++i)
    {
        const float u = static_cast<float> (i)
                        / static_cast<float> (EnvelopeShape::curveSamplesPerSegment);
        const float shaped = EnvelopeCurve::shape (u, curve);
        const EnvelopeShape::Point n {a.x + (b.x - a.x) * u, a.y + (b.y - a.y) * shaped};
        path.lineTo (EnvelopeShape::toPixels (n, bounds));
    }
}

EnvelopeShape::Point handleOnSegment (EnvelopeShape::Point a, EnvelopeShape::Point b,
                                      EnvelopeCurve::Handle h)
{
    h = EnvelopeCurve::clampHandle (h);
    return {a.x + (b.x - a.x) * h.x, a.y + (b.y - a.y) * h.y};
}
} // namespace

std::array<EnvelopeShape::Point, EnvelopeShape::numCornerPoints>
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

std::array<EnvelopeShape::Point, 3>
EnvelopeShape::buildCurveHandles (const Params& p, const juce::NormalisableRange<float>& timeRange)
{
    const auto pts = buildPoints (p, timeRange);
    return {{
        handleOnSegment (pts[start], pts[attackPeak], p.attackCurve),
        handleOnSegment (pts[attackPeak], pts[decaySustain], p.decayCurve),
        handleOnSegment (pts[sustainEnd], pts[releaseEnd], p.releaseCurve),
    }};
}

EnvelopeCurve::Handle EnvelopeShape::handleFromSegmentPoint (Point segmentStart, Point segmentEnd,
                                                             Point plotPoint)
{
    const float dx = segmentEnd.x - segmentStart.x;
    const float dy = segmentEnd.y - segmentStart.y;

    EnvelopeCurve::Handle h;
    h.x = std::abs (dx) < 1.0e-6f ? 0.5f : (plotPoint.x - segmentStart.x) / dx;
    h.y = std::abs (dy) < 1.0e-6f ? 0.5f : (plotPoint.y - segmentStart.y) / dy;
    return EnvelopeCurve::clampHandle (h);
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
    path.startNewSubPath (toPixels (pts[start], bounds));

    appendShapedSegment (path, bounds, pts[start], pts[attackPeak], p.attackCurve);
    appendShapedSegment (path, bounds, pts[attackPeak], pts[decaySustain], p.decayCurve);
    path.lineTo (toPixels (pts[sustainEnd], bounds));
    appendShapedSegment (path, bounds, pts[sustainEnd], pts[releaseEnd], p.releaseCurve);

    return path;
}
