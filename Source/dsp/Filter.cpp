#include "dsp/Filter.h"
#include "MathConstants.h"

#include <algorithm>
#include <cmath>

void Filter::setSampleRate (float sampleRate)
{
    if (sampleRate > 0.0f)
        sampleRate_ = sampleRate;

    updateCoefficient();
}

void Filter::setCutoffHz (float cutoffHz)
{
    cutoffHz_ = std::clamp(cutoffHz, 20.0f, 0.49f * sampleRate_);
    updateCoefficient();
}

void Filter::setResonance (float resonance01)
{
    resonance_ = std::clamp(resonance01, 0.0f, 1.0f);
}

void Filter::reset()
{
    z1_ = 0.0f;
}

void Filter::updateCoefficient()
{
    if (sampleRate_ <= 0.0f || cutoffHz_ <= 0.0f)
        return;
    
    alpha_ = static_cast<float>(
        1.0f - std::exp(-2.0f * dsp::kPi * cutoffHz_ / sampleRate_));
}

float Filter::process (float input)
{
    // One-pole low-pass recurrence:
    //
    //   y[n] = y[n-1] + alpha * (x[n] - y[n-1])
    //
    // Equivalent:
    //   y[n] = alpha * x[n] + (1 - alpha) * y[n-1]
    //
    // This is a leaky integrator / exponential smoother in the time domain.
    // In the frequency domain it attenuates highs (gentle -6 dB/oct slope).
    //
    float y = z1_ + alpha_ * (input - z1_);
    z1_ = y;
    return y;
}