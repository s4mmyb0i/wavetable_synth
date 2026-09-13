#pragma once

#include <array>
#include <cstddef>

// One cycle of a waveform. Amplitude samples only — no time or frequency columns.
class Wavetable
{
public:
    static constexpr std::size_t size = 2048;

    // Fill samples_ with one period of a sine (use std::sin once here, at init time).
    static Wavetable sine();

    // phase01 is in [0, 1). Return the interpolated amplitude at that phase.
    float lookup (float phase01) const;

private:
    std::array<float, size> samples_ {};
};
