#pragma once

#include <juce_graphics/juce_graphics.h>
#include <cmath>

// Geometry helper for the filter frequency-response plot (matches Filter SVF damping).
struct FilterResponse
{
    // Matches ParamIDs::cutoff range so the handle can travel the full plot width.
    static constexpr float minHz = 20.0f;
    static constexpr float maxHz = 2000.0f;
    static constexpr float minDb = -24.0f;
    static constexpr float maxDb = 24.0f;

    // Analog 2-pole LP magnitude approximating the voice SVF (R = 2 - 1.9 * res).
    static float magnitude (float freqHz, float cutoffHz, float resonance01)
    {
        const float R = 2.0f - 1.9f * juce::jlimit (0.0f, 1.0f, resonance01);
        const float Q = 1.0f / juce::jmax (R, 0.05f);
        const float x = freqHz / juce::jmax (cutoffHz, 1.0f);
        const float term = 1.0f - x * x;
        const float denom = std::sqrt (term * term + (x / Q) * (x / Q));
        return 1.0f / juce::jmax (denom, 1.0e-6f);
    }

    static float magnitudeToDb (float mag)
    { return 20.0f * std::log10 (juce::jmax (mag, 1.0e-6f)); }

    static float freqToNormX (float hz)
    {
        const float minL = std::log (minHz);
        const float maxL = std::log (maxHz);
        return juce::jlimit (0.0f, 1.0f, (std::log (juce::jmax (hz, minHz)) - minL) / (maxL - minL));
    }

    static float normXToFreq (float x01)
    {
        const float minL = std::log (minHz);
        const float maxL = std::log (maxHz);
        return std::exp (minL + juce::jlimit (0.0f, 1.0f, x01) * (maxL - minL));
    }

    static float dbToNormY (float db)
    { return juce::jlimit (0.0f, 1.0f, (db - minDb) / (maxDb - minDb)); }

    static float normYToDb (float y01)
    { return minDb + juce::jlimit (0.0f, 1.0f, y01) * (maxDb - minDb); }

    static juce::Point<float> toPixels (float normX, float normY, juce::Rectangle<float> bounds)
    {
        return {bounds.getX() + normX * bounds.getWidth(),
                bounds.getBottom() - normY * bounds.getHeight()};
    }

    static juce::Point<float> fromPixels (juce::Point<float> px, juce::Rectangle<float> bounds)
    {
        const float x = bounds.getWidth() > 0.0f ? (px.x - bounds.getX()) / bounds.getWidth() : 0.0f;
        const float y =
            bounds.getHeight() > 0.0f ? (bounds.getBottom() - px.y) / bounds.getHeight() : 0.0f;
        return {juce::jlimit (0.0f, 1.0f, x), juce::jlimit (0.0f, 1.0f, y)};
    }

    // Handle sits at cutoff on the response curve.
    static juce::Point<float> handleNorm (float cutoffHz, float resonance01)
    {
        const float db = magnitudeToDb (magnitude (cutoffHz, cutoffHz, resonance01));
        return {freqToNormX (cutoffHz), dbToNormY (db)};
    }

    static juce::Path buildPath (juce::Rectangle<float> bounds, float cutoffHz, float resonance01,
                                 int numPoints = 96)
    {
        juce::Path path;
        for (int i = 0; i < numPoints; ++i)
        {
            const float t = static_cast<float> (i) / static_cast<float> (numPoints - 1);
            const float hz = normXToFreq (t);
            const float db = magnitudeToDb (magnitude (hz, cutoffHz, resonance01));
            const auto pt = toPixels (t, dbToNormY (db), bounds);
            if (i == 0)
                path.startNewSubPath (pt);
            else
                path.lineTo (pt);
        }
        return path;
    }
};
