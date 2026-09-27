#pragma once

#include <array>
#include <cstddef>

// One cycle of a waveform. Amplitude samples only — no time or frequency columns.
class Wavetable
{
public:
    static constexpr std::size_t size = 2048;
    static_assert((size & (size-1)) == 0, "Wavetable size must be power of two");

    // Fill samples_ with one period.
    static Wavetable sine();
    static Wavetable saw();      // geometric (aliases) — keep for comparison / tests
    static Wavetable square();   // geometric
    static Wavetable triangle();

    // Band-limited additive tables: only harmonics 1..maxHarmonics (square: odd k only).
    // YOU fill the loops — stubs currently fall back to geometric so the project still builds.
    static Wavetable sawBandLimited (int maxHarmonics);
    static Wavetable squareBandLimited (int maxHarmonics);

    // phase01 is in [0, 1). Return the interpolated amplitude at that phase.
    float lookup (float phase01) const;
    
private:
    void normalizeRms (float targetRms);
    std::array<float, size> samples_ {};
};
