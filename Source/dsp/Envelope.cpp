#include "dsp/Envelope.h"
#include "dsp/EnvelopeCurve.h"

#include <algorithm>
#include <cmath>

void Envelope::refreshTIncrement (float seconds) noexcept
{
    const float sR = static_cast<float> (sampleRate_);
    if (seconds <= 1.0e-9f || sR <= 0.0f)
        tIncrement_ = 1.0e6f; // complete on the next advance
    else
        tIncrement_ = 1.0f / (seconds * sR);
}

void Envelope::beginStage (Stage newStage, float startLevel, float endLevel, float seconds) noexcept
{
    stage_ = newStage;
    startLevel_ = startLevel;
    endLevel_ = endLevel;
    t_ = 0.0f;
    level_f = startLevel;
    refreshTIncrement (seconds);

    // Zero-length stage: snap to the end immediately.
    if (seconds <= 1.0e-9f)
    {
        t_ = 1.0f;
        level_f = endLevel;
    }
}

void Envelope::advanceTimedStage (float curveX, float curveY) noexcept
{
    t_ += tIncrement_;
    if (t_ >= 1.0f)
    {
        t_ = 1.0f;
        level_f = endLevel_;
        return;
    }

    const float shaped = EnvelopeCurve::shape (t_, curveX, curveY);
    level_f = startLevel_ + (endLevel_ - startLevel_) * shaped;
}

void Envelope::finishTimedStage (float sustain) noexcept
{
    switch (stage_)
    {
    case Stage::idle: break;
    case Stage::sustain: break;
    case Stage::attack:
        beginStage (Stage::decay, 1.0f, sustain, parameters_.decaySeconds);
        if (t_ >= 1.0f) // zero-length decay
        {
            level_f = sustain;
            stage_ = Stage::sustain;
        }
        break;

    case Stage::decay:
        level_f = sustain;
        stage_ = Stage::sustain;
        break;

    case Stage::release:
        level_f = 0.0f;
        stage_ = Stage::idle;
        break;

    default: break;
    }
}

void Envelope::setParameters (const Parameters& newParameters)
{
    const float prevAttack = parameters_.attackSeconds;
    const float prevDecay = parameters_.decaySeconds;
    const float prevRelease = parameters_.releaseSeconds;

    parameters_ = newParameters;
    parameters_.sustainLevel = std::clamp (parameters_.sustainLevel, 0.0f, 1.0f);

    auto clampHandle = [] (float& x, float& y)
    {
        const auto h = EnvelopeCurve::clampHandle ({x, y});
        x = h.x;
        y = h.y;
    };
    clampHandle (parameters_.attackCurveX, parameters_.attackCurveY);
    clampHandle (parameters_.decayCurveX, parameters_.decayCurveY);
    clampHandle (parameters_.releaseCurveX, parameters_.releaseCurveY);

    const float sustain = parameters_.sustainLevel;

    switch (stage_)
    {
    case Stage::idle: break;

    case Stage::attack:
        if (std::abs (parameters_.attackSeconds - prevAttack) > 1.0e-9f)
            refreshTIncrement (parameters_.attackSeconds);
        break;

    case Stage::decay:
        endLevel_ = sustain;
        if (std::abs (parameters_.decaySeconds - prevDecay) > 1.0e-9f)
            refreshTIncrement (parameters_.decaySeconds);
        break;

    case Stage::sustain: level_f = sustain; break;

    case Stage::release:
        // Keep t_; only retarget the clock when release time changes (processBlock-safe).
        if (std::abs (parameters_.releaseSeconds - prevRelease) > 1.0e-9f)
            refreshTIncrement (parameters_.releaseSeconds);
        break;
    }
}

void Envelope::setSampleRate (double newSampleRate)
{
    if (newSampleRate > 0.0) sampleRate_ = newSampleRate;

    switch (stage_)
    {
    case Stage::idle:
    case Stage::sustain: break;
    case Stage::attack: refreshTIncrement (parameters_.attackSeconds); break;
    case Stage::decay: refreshTIncrement (parameters_.decaySeconds); break;
    case Stage::release: refreshTIncrement (parameters_.releaseSeconds); break;
    }
}

void Envelope::reset() noexcept
{
    stage_ = Stage::idle;
    level_f = 0.0f;
    t_ = 0.0f;
    tIncrement_ = 0.0f;
    startLevel_ = 0.0f;
    endLevel_ = 0.0f;
}

void Envelope::noteOn() noexcept
{
    const float sustain = std::clamp (parameters_.sustainLevel, 0.0f, 1.0f);

    // Soft retrigger: continue from the current level instead of snapping to 0.
    if (level_f >= 1.0f)
    {
        beginStage (Stage::decay, 1.0f, sustain, parameters_.decaySeconds);
        if (t_ >= 1.0f)
        {
            level_f = sustain;
            stage_ = Stage::sustain;
        }
    }
    else
    {
        beginStage (Stage::attack, level_f, 1.0f, parameters_.attackSeconds);
        if (t_ >= 1.0f) // zero-length attack
            finishTimedStage (sustain);
    }
}

void Envelope::noteOff() noexcept
{
    if (stage_ == Stage::idle) return;

    beginStage (Stage::release, level_f, 0.0f, parameters_.releaseSeconds);
    if (t_ >= 1.0f)
    {
        level_f = 0.0f;
        stage_ = Stage::idle;
    }
}

float Envelope::getNextSample() noexcept
{
    const float sustain = std::clamp (parameters_.sustainLevel, 0.0f, 1.0f);

    switch (stage_)
    {
    case Stage::idle: level_f = 0.0f; break;

    case Stage::attack:
        advanceTimedStage (parameters_.attackCurveX, parameters_.attackCurveY);
        if (t_ >= 1.0f) finishTimedStage (sustain);
        break;

    case Stage::decay:
        advanceTimedStage (parameters_.decayCurveX, parameters_.decayCurveY);
        if (t_ >= 1.0f) finishTimedStage (sustain);
        break;

    case Stage::sustain: level_f = sustain; break;

    case Stage::release:
        advanceTimedStage (parameters_.releaseCurveX, parameters_.releaseCurveY);
        if (t_ >= 1.0f) finishTimedStage (sustain);
        break;
    }

    return level_f;
}
