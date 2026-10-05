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

void UnisonOsc::reset()
{
    for (int i = 0; i < maxUnison; ++i) phase_[i] = 0.0f;
}

float UnisonOsc::process()
{
    float sum = 0;
    for (int i = 0; i < unisonCount_; ++i)
    {
        sum += table_->lookup (phase_[i]);
        phase_[i] += increment_[i];
        phase_[i] -= std::floor (phase_[i]);
    }
    return sum * (1.0f / static_cast<float> (unisonCount_));
}
