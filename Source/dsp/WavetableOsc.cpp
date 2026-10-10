#include "dsp/WavetableOsc.h"

WavetableOsc::WavetableOsc (const Wavetable& table) : table_ (&table) {}

void WavetableOsc::setTable (const Wavetable& table)
{
    table_ = &table;
    bank_ = nullptr;
}

void WavetableOsc::setBank (const WavetableBank& bank)
{
    bank_ = &bank;
    table_ = nullptr;
}

void WavetableOsc::setPosition (float position01)
{ position_ = std::clamp (position01, 0.0f, 1.0f); }

void WavetableOsc::updateIncrements()
{
    if (sampleRate_ < 0) return;

    for (int i = 0; i < unisonCount_; ++i)
        increment_[i] = baseFrequencyHz_ *
                        centsToRatio (unisonSpread (i, unisonCount_) * unisonDetuneCents_) /
                        sampleRate_;
}

void WavetableOsc::setSampleRate (float sampleRate)
{
    sampleRate_ = sampleRate;
    updateIncrements();
}

void WavetableOsc::setFrequency (float hz)
{
    baseFrequencyHz_ = hz;
    updateIncrements();
}

void WavetableOsc::setUnison (int count, float detuneCents)
{
    unisonCount_ = std::clamp (count, 1, maxUnison);
    unisonDetuneCents_ = detuneCents;
    updateIncrements();
}

void WavetableOsc::setWarp (WarpMode mode, float amount)
{
    warpMode_ = mode;
    warpAmount_ = std::clamp (amount, 0.0f, 1.0f);
}

void WavetableOsc::reset()
{
    for (int i = 0; i < maxUnison; ++i) phase_[i] = 0.0f;
}

float WavetableOsc::process()
{
    float sum = 0;
    for (int i = 0; i < unisonCount_; ++i)
    {
        const float readPhase = warpPhase (phase_[i], warpAmount_, warpMode_);

        if (bank_ != nullptr)
            sum += bank_->lookup (position_, readPhase);
        else if (table_ != nullptr)
            sum += table_->lookup (readPhase);

        phase_[i] += increment_[i];
        phase_[i] -= std::floor (phase_[i]);
    }
    return sum * (1.0f / static_cast<float> (unisonCount_));
}
