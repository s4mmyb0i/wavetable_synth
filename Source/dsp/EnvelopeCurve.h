#pragma once

#include <algorithm>
#include <cmath>

// Shared by Envelope (audio) and EnvelopeShape (UI).
// Each A/D/R segment is a quadratic Bezier in unit progress/shape space:
//   shape(t) = 2*C*t*(1-t) + t^2,  with C chosen so the curve passes through handle (hx, hy).
// Time along the segment stays linear; only level is bent. C is clamped to [0, 1] so the
// curve stays monotonic and both endpoints have finite slope (no t^p kink).
namespace EnvelopeCurve
{
inline constexpr float handleXMin = 0.05f;
inline constexpr float handleXMax = 0.95f;

struct Handle
{
    float x = 0.5f; // segment progress 0..1
    float y = 0.5f; // shaped level 0..1 (0 = start of segment, 1 = end)
};

// Valid hy for a given hx when C is kept in [0, 1].
inline float minYForX (float hx) noexcept
{
    const float x = std::clamp (hx, handleXMin, handleXMax);
    return x * x;
}

inline float maxYForX (float hx) noexcept
{
    const float x = std::clamp (hx, handleXMin, handleXMax);
    return 2.0f * x - x * x;
}

inline Handle clampHandle (Handle h) noexcept
{
    h.x = std::clamp (h.x, handleXMin, handleXMax);
    h.y = std::clamp (h.y, minYForX (h.x), maxYForX (h.x));
    return h;
}

inline float controlFromHandle (Handle h) noexcept
{
    h = clampHandle (h);
    const float denom = 2.0f * h.x * (1.0f - h.x);
    if (denom < 1.0e-8f) return 0.5f;
    return std::clamp ((h.y - h.x * h.x) / denom, 0.0f, 1.0f);
}

// shape(0)=0, shape(1)=1, monotonic for a clamped handle.
inline float shape (float t01, Handle h) noexcept
{
    const float t = std::clamp (t01, 0.0f, 1.0f);
    const float c = controlFromHandle (h);
    return 2.0f * c * t * (1.0f - t) + t * t;
}

// Convenience overload.
inline float shape (float t01, float hx, float hy) noexcept
{ return shape (t01, Handle {hx, hy}); }
} // namespace EnvelopeCurve
