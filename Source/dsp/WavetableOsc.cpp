#include "dsp/WavetableOsc.h"
#include "dsp/Wavetable.h"

#include <cmath>

WavetableOsc::WavetableOsc (const Wavetable& table)
    : table_ (&table)
{
}

void WavetableOsc::setSampleRate (float sampleRate)
{
    sampleRate_ = sampleRate;
    increment_ = frequency_ / sampleRate_;
}

void WavetableOsc::setFrequency (float hz)
{
    frequency_ = hz;
    increment_ = frequency_ / sampleRate_;
}

void WavetableOsc::reset()
{
    phase_ = 0.0f;
}

float WavetableOsc::process()
{
    float sample = table_->lookup(phase_);
    phase_ += increment_;
    phase_ -= floor(phase_);
    return sample;

}
