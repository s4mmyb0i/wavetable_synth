#pragma once

class Envelope
{
public:
    struct Parameters
    {
        float attackSeconds = 0.01f;
        float decaySeconds = 0.1f;
        float sustainLevel = 0.8f; // 0..1
        float releaseSeconds = 0.2f;
        // Bezier through-handles in segment unit space (0.5, 0.5) = linear
        float attackCurveX = 0.5f;
        float attackCurveY = 0.5f;
        float decayCurveX = 0.5f;
        float decayCurveY = 0.5f;
        float releaseCurveX = 0.5f;
        float releaseCurveY = 0.5f;
    };

    Parameters getParameters() const noexcept { return parameters_; }
    void setParameters (const Parameters& newParameters);
    bool isActive() const noexcept { return stage_ != Stage::idle; }
    void setSampleRate (double newSampleRate);
    void reset() noexcept;
    void noteOn() noexcept;
    void noteOff() noexcept;
    float getNextSample() noexcept;

private:
    enum class Stage
    {
        idle,
        attack,
        decay,
        sustain,
        release
    };

    void beginStage (Stage stage, float startLevel, float endLevel, float seconds) noexcept;
    void refreshTIncrement (float seconds) noexcept;
    void advanceTimedStage (float curveX, float curveY) noexcept;
    void finishTimedStage (float sustain) noexcept;

    Parameters parameters_;

    Stage stage_ = Stage::idle;
    double sampleRate_ = 44100.0;
    float level_f = 0.0f;

    float t_ = 0.0f;
    float tIncrement_ = 0.0f;
    float startLevel_ = 0.0f;
    float endLevel_ = 0.0f;
};
