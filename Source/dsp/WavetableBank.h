#pragma once

#include "dsp/Wavetable.h"

#include <algorithm>
#include <array>

// Stack of single-cycle frames for wavetable-position morphing.
// Owned by the processor (shared). Oscillators only hold a pointer + a position.
class WavetableBank
{
public:
    static constexpr int maxFrames = 64;

    void setNumFrames (int n) { numFrames_ = std::clamp (n, 1, maxFrames); }
    int getNumFrames() const { return numFrames_; }

    // Returns a reference so you can assign: bank.frame (0) = Wavetable::sine();
    Wavetable& frame (int i)
    { return frames_[static_cast<std::size_t> (std::clamp (i, 0, numFrames_ - 1))]; }

    const Wavetable& frame (int i) const
    { return frames_[static_cast<std::size_t> (std::clamp (i, 0, numFrames_ - 1))]; }

    // position01 in [0, 1]: which frames to blend.
    // phase01 in [0, 1): where we are inside the cycle (same as Wavetable::lookup).
    float lookup (float position01, float phase01) const
    {
        if (numFrames_ <= 1) return frames_[0].lookup (phase01);

        const float pos = std::clamp (position01, 0.0f, 1.0f);
        const float x = pos * static_cast<float> (numFrames_ - 1);
        const int i0 = static_cast<int> (x);
        const int i1 = std::min (i0 + 1, numFrames_ - 1);
        const float frac = x - static_cast<float> (i0);

        const float a = frames_[static_cast<std::size_t> (i0)].lookup (phase01);
        const float b = frames_[static_cast<std::size_t> (i1)].lookup (phase01);
        return a + frac * (b - a);
    }

    // Demo bank: sine → triangle → saw across 3 frames.
    static WavetableBank makeBasicMorph()
    {
        WavetableBank bank;
        bank.setNumFrames (3);
        bank.frame (0) = Wavetable::sine();
        bank.frame (1) = Wavetable::triangle();
        bank.frame (2) = Wavetable::sawBandLimited (64);
        return bank;
    }

private:
    int numFrames_ = 1;
    std::array<Wavetable, maxFrames> frames_ {};
};
