#pragma once

#include "dsp/Wavetable.h"
#include "dsp/WavetableBank.h"

#include <algorithm>
#include <cmath>

enum class WarpMode
{
    off,
    bendPlus,
    sync
};

class WavetableOsc
{
public:
    explicit WavetableOsc (const Wavetable& table);

    void setSampleRate (float sampleRate);
    void setFrequency (float hz);
    void setUnison (int count, float detuneCents);
    void setWarp (WarpMode mode, float amount);

    // Single-cycle path (sine / triangle / mip select). Clears any bank.
    void setTable (const Wavetable& table);

    // Multi-frame morph path. Clears single-table mode.
    void setBank (const WavetableBank& bank);
    void setPosition (float position01);

    void reset();
    float process();

private:
    const Wavetable* table_ = nullptr;
    const WavetableBank* bank_ = nullptr;
    float position_ = 0.0f;

    float warpPhase (float phase01, float amount, WarpMode mode);

    float spread (int index, int count);
    float centsToRatio (float cents) { return std::pow (2.0f, cents / 1200.0f); }
    void updateIncrements();

    float sampleRate_ = 44100.0f;
    float baseFrequencyHz_ = 0.0f;
    int unisonCount_ = 1;
    float unisonDetuneCents_ = 7.0f;

    static constexpr int maxUnison = 8;
    float warpAmount_ = 0;
    WarpMode warpMode_ = WarpMode::off;

    float phase_[maxUnison] {};
    float increment_[maxUnison] {};
};
