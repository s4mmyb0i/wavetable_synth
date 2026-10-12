#include "dsp/Envelope.h"

#include <algorithm>
#include <cmath>

namespace
{
float safeDiv (float numerator, float denomiator)
{ return (denomiator > 1.0e-9f) ? (numerator / denomiator) : numerator; }
} // namespace

void Envelope::updateRates() noexcept
{
    const float sR = static_cast<float> (sampleRate_);
    const float sustain = std::clamp (parameters_.sustainLevel, 0.0f, 1.0f);

    attackIncrement_ = safeDiv (1.0f, parameters_.attackSeconds * sR);
    decayIncrement_ = safeDiv (1.0f - sustain, parameters_.decaySeconds * sR);

    // During release, noteOff owns releaseIncrement_ (scaled from current level).
    if (stage_ != Stage::release)
        releaseIncrement_ = safeDiv (1.0f, parameters_.releaseSeconds * sR);
}

void Envelope::setParameters (const Parameters& newParameters)
{
    const float previousRelease = parameters_.releaseSeconds;
    parameters_ = newParameters;
    parameters_.sustainLevel = std::clamp (parameters_.sustainLevel, 0.0f, 1.0f);
    updateRates();

    // Only re-arm when release time actually changes (processBlock sets params every
    // buffer; recomputing from level each time would accelerate the fade).
    if (stage_ == Stage::release
        && std::abs (parameters_.releaseSeconds - previousRelease) > 1.0e-9f)
        armReleaseIncrement();
}

void Envelope::setSampleRate (double newSampleRate)
{
    if (newSampleRate > 0) sampleRate_ = newSampleRate;
    updateRates();

    if (stage_ == Stage::release) armReleaseIncrement();
}

void Envelope::reset() noexcept
{
    stage_ = Stage::idle;
    level_f = 0.0f;
}

void Envelope::noteOn() noexcept
{
    // Soft retrigger: continue from the current level instead of snapping to 0.
    if (level_f >= 1.0f)
    {
        level_f = 1.0f;
        stage_ = Stage::decay;
    }
    else
    {
        stage_ = Stage::attack;
    }
}

void Envelope::noteOff() noexcept
{
    if (stage_ == Stage::idle) return;

    stage_ = Stage::release;
    armReleaseIncrement();
}

void Envelope::armReleaseIncrement() noexcept
{
    const float sR = static_cast<float> (sampleRate_);
    releaseIncrement_ = safeDiv (level_f, parameters_.releaseSeconds * sR);
}

float Envelope::getNextSample() noexcept
{
    const float sustain = std::clamp (parameters_.sustainLevel, 0.0f, 1.0f);

    switch (stage_)
    {
    case Stage::idle: level_f = 0.0f; break;

    case Stage::attack:
        level_f += attackIncrement_;
        if (level_f >= 1.0f)
        {
            level_f = 1.0f;
            stage_ = Stage::decay;
        }
        break;

    case Stage::decay:
        level_f -= decayIncrement_;
        if (level_f <= sustain)
        {
            level_f = sustain;
            stage_ = Stage::sustain;
        }
        break;

    case Stage::sustain: level_f = sustain; break;

    case Stage::release:
        level_f -= releaseIncrement_;
        if (level_f <= 0.0f)
        {
            level_f = 0.0f;
            stage_ = Stage::idle;
        }
        break;
    }

    return level_f;
}
