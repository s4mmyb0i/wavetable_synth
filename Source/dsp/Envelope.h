#pragma once

class Envelope
{
public:
    struct Parameters
    {
        float attackSeconds  = 0.01f;
        float decaySeconds   = 0.1f;
        float sustainLevel   = 0.8f;  // 0..1
        float releaseSeconds = 0.2f;
    };

    Parameters getParameters() const noexcept {return parameters_;}
    void setParameters (const Parameters& newParameters);
    bool isActive() const noexcept {return stage_ != Stage::idle;}
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

    void updateRates() noexcept;

    Parameters parameters_;
    
    Stage stage_            = Stage::idle;
    double sampleRate_      = 44100.0;
    float level_f           = 0.0f;

    float attackIncrement_  = 0.0f;
    float decayIncrement_   = 0.0f;
    float releaseIncrement_ = 0.0f;
};
