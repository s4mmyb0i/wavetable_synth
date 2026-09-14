#include "dsp/Envelope.h"

#include <algorithm>
#include <cmath>

namespace
{
float safeDiv (float numerator, float denomiator)
{
    return (denomiator > 1.0e-9f) ? (numerator / denomiator) : numerator;
}
}

void Envelope::updateRates () noexcept
{
    const float sR = static_cast<float>(sampleRate_);
    const double sustain = std::clamp (parameters_.sustainLevel, 0.0f, 1.0f);

    attackIncrement_    = safeDiv(1.0f, parameters_.attackSeconds * sR);
    decayIncrement_     = safeDiv(static_cast<float>(1.0f - sustain), parameters_.decaySeconds * sR);

    releaseIncrement_   = safeDiv(1.0f, parameters_.releaseSeconds * sR);
}


void Envelope::setParameters (const Parameters& newParameters)
{
    parameters_ = newParameters;
    parameters_.sustainLevel = std::clamp (parameters_.sustainLevel, 0.0f, 1.0f);
    updateRates();
}

void Envelope::setSampleRate (double newSampleRate)
{
    if (newSampleRate > 0)
        sampleRate_ = newSampleRate;
    updateRates();
}

void Envelope::reset () noexcept
{
    stage_ = Stage::idle;
    level_f = 0.0f;
}

void Envelope::noteOn () noexcept
{
    stage_ = Stage::attack;
    level_f = 0.0f;
}

void Envelope::noteOff () noexcept
{
    if (stage_ == Stage::idle)
        return;
    
    stage_ = Stage::release;

    const float sR = static_cast<float>(sampleRate_);
    releaseIncrement_ = safeDiv(level_f, parameters_.releaseSeconds * sR);
}

float Envelope::getNextSample() noexcept
{
    const float sustain = std::clamp (parameters_.sustainLevel, 0.0f, 1.0f);
    
    switch (stage_)
    {
        case Stage::idle:
            level_f = 0.0f;
            break;
        
        case Stage::attack:
            level_f += attackIncrement_;
            if (level_f >= 1.0f)
            {
                level_f = 1.0f;
                stage_  = Stage::decay;
            }
            break;

        case Stage::decay:
            level_f -= decayIncrement_;
            if (level_f <= sustain)
            {
                level_f = sustain;
                stage_  = Stage::sustain;
            }
            break;
        
        case Stage::sustain:
            level_f = sustain;
            break;
        
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