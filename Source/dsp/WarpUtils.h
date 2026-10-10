#pragma once

#include <algorithm>
#include <cmath>

enum class WarpMode
{
    off,
    bendPlus,
    sync
};

// Shared by WavetableOsc (audio) and OscWaveformDisplay (UI preview).
inline float warpPhase (float phase01, float amount, WarpMode mode)
{
    amount = std::clamp (amount, 0.0f, 1.0f);
    float p = phase01;

    switch (mode)
    {
    case WarpMode::off: break;

    case WarpMode::bendPlus:
    {
        const float kink = 0.5f - 0.49f * amount;
        if (phase01 < kink)
            p = 0.5f * (phase01 / kink);
        else
            p = 0.5f + 0.5f * ((phase01 - kink) / (1.0f - kink));
        break;
    }

    case WarpMode::sync:
    {
        const float ratio = 1.0f + amount * 7.0f;
        p = phase01 * ratio;
        break;
    }
    }

    return p - std::floor (p);
}

// Unison voice i of count → [-1, 1] detune weight (0 when count == 1).
inline float unisonSpread (int i, int count)
{
    if (count <= 1) return 0.0f;
    return -1.0f + 2.0f * (static_cast<float> (i) / static_cast<float> (count - 1));
}
