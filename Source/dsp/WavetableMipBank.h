#pragma once

#include "dsp/Wavetable.h"

#include <array>

// Several band-limited versions of one shape (mipmap).
// Pick a level from frequency + sample rate so harmonics stay under Nyquist.
//
// WavetableOsc does NOT choose mips — it only reads whatever table you give it.
// SynthVoice (or the processor) calls select() then osc.setTable(...).
class WavetableMipBank
{
public:
    enum class Shape
    {
        saw,
        square
    };

    static constexpr int numLevels = 12;

    // Harmonic caps per level (bright → dull). You may tweak these.
    static constexpr std::array<int, numLevels> kMaxHarmonics {128, 96, 64, 48, 32, 24,
                                                               16,  12, 8,  6,  4,  2};

    // Build all mip levels for this shape (call once at startup).
    void build (Shape shape);

    // Choose the brightest level whose harmonics fit under Nyquist.
    const Wavetable& select (float frequencyHz, float sampleRate) const;

    const Wavetable& getLevel (int index) const
    { return levels_[static_cast<std::size_t> (index)]; }

private:
    Shape shape_ = Shape::saw;
    std::array<Wavetable, numLevels> levels_ {};
};
