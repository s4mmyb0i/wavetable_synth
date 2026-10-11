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
    void process (float inL, float inR, float& outL, float& outR);

private:
    void updateCoefficient();
    float processChannel (float in, float& lp, float& bp);

    float sampleRate_ = 44100.0f;
    float cutoffHz_ = 1000.0f;
    float resonance_ = 0.0f;

    float g_ = 0.0f;
    float R_ = 0.0f;

    float lpL_ = 0.0f;
    float bpL_ = 0.0f;
    float lpR_ = 0.0f;
    float bpR_ = 0.0f;
};
