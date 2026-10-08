#include "dsp/UnisonOsc.h"
#include <algorithm>

UnisonOsc::UnisonOsc (const Wavetable& table) : table_ (&table) {}

float UnisonOsc::spread (int i, int count)
{
    if (count == 1)
        return 0.0f;
    else
        return -1.0f + 2.0f * (static_cast<float> (i) / static_cast<float> (count - 1));
}

void UnisonOsc::updateIncrements()
{
    if (sampleRate_ < 0) return;

    for (int i = 0; i < unisonCount_; ++i)
        increment_[i] = baseFrequencyHz_ *
                        centsToRatio (spread (i, unisonCount_) * unisonDetuneCents_) / sampleRate_;
}

float UnisonOsc::warpPhase (float phase01, float amount, WarpMode mode)
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

void UnisonOsc::setSampleRate (float sampleRate)
{
    sampleRate_ = sampleRate;
    updateIncrements();
}

void UnisonOsc::setFrequency (float hz)
{
    baseFrequencyHz_ = hz;
    updateIncrements();
}

void UnisonOsc::setUnison (int count, float detuneCents)
{
    unisonCount_ = std::clamp (count, 1, maxUnison);
    unisonDetuneCents_ = detuneCents;
    updateIncrements();
}

void UnisonOsc::setWarp (WarpMode mode, float amount)
{
    warpMode_ = mode;
    warpAmount_ = std::clamp (amount, 0.0f, 1.0f);
}

void UnisonOsc::reset()
{
    for (int i = 0; i < maxUnison; ++i) phase_[i] = 0.0f;
}

float UnisonOsc::process()
{
    float sum = 0;
    for (int i = 0; i < unisonCount_; ++i)
    {
        const float readPhase = warpPhase (phase_[i], warpAmount_, warpMode_);
        sum += table_->lookup (readPhase);
        phase_[i] += increment_[i];
        phase_[i] -= std::floor (phase_[i]);
    }
    return sum * (1.0f / static_cast<float> (unisonCount_));
}
