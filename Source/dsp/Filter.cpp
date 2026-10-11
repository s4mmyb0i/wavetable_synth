#include "dsp/Filter.h"
#include "MathConstants.h"

#include <algorithm>
#include <cmath>

void Filter::setSampleRate (float sampleRate)
{
    if (sampleRate > 0.0f) sampleRate_ = sampleRate;
    updateCoefficient();
}

void Filter::setCutoffHz (float cutoffHz)
{
    cutoffHz_ = std::clamp (cutoffHz, 20.0f, 0.45f * sampleRate_);
    updateCoefficient();
}

void Filter::setResonance (float resonance01)
{
    resonance_ = std::clamp (resonance01, 0.0f, 1.0f);
    updateCoefficient();
}

void Filter::reset()
{
    lpL_ = 0.0f;
    bpL_ = 0.0f;
    lpR_ = 0.0f;
    bpR_ = 0.0f;
}

void Filter::updateCoefficient()
{
    if (sampleRate_ <= 0.0f || cutoffHz_ <= 0.0f) return;

    g_ = static_cast<float> (std::tan (dsp::kPi * cutoffHz_ / sampleRate_));
    g_ = std::min (g_, 1.0f);
    R_ = 2.0f - 1.9f * resonance_;
}

float Filter::processChannel (float in, float& lp, float& bp)
{
    const float hp = in - lp - R_ * bp;
    bp += g_ * hp;
    lp += g_ * bp;
    return lp;
}

void Filter::process (float inL, float inR, float& outL, float& outR)
{
    outL = processChannel (inL, lpL_, bpL_);
    outR = processChannel (inR, lpR_, bpR_);
}
