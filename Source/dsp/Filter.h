#pragma once

// Process one sample at a time. State lives here.
class Filter
{
public:
    void setSampleRate (float sampleRate);
    void setCutoffHz (float cutoffHz);
    void setResonance (float resonance01);

    void reset();

    // y = filter(x)
    float process (float input);

private:
    void updateCoefficient();

    float sampleRate_ = 44100.0f;
    float cutoffHz_ = 1000.0f;
    float resonance_ = 0.0f;

    float lp_ = 0.0f;
    float bp_ = 0.0f;
    float g_ = 0.0f;
    float R_ = 0.0f;
};
