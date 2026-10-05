#pragma once

#include "dsp/Wavetable.h"

#include <cmath>

class Wavetable;

class UnisonOsc
{
public:
    explicit UnisonOsc (const Wavetable& table);

    void setSampleRate (float sampleRate);
    void setFrequency (float hz);
    void setUnison (int count, float detuneCents);

    void setTable (const Wavetable& table) { table_ = &table; }

    void reset();
    float process();

private:
    const Wavetable* table_ = nullptr;

    float spread (int index, int count);
    float centsToRatio (float cents) { return std::pow (2.0f, cents / 1200.0f); }
    void updateIncrements();

    float sampleRate_ = 44100.0f;
    float baseFrequencyHz_ = 0.0f;
    int unisonCount_ = 1;
    float unisonDetuneCents_ = 7.0f;

    static constexpr int maxUnison = 8;
    float phase_[maxUnison] {};
    float increment_[maxUnison] {};
};
