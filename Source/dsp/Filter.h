#pragma once

// One-pole (or later SVF/biquad) low-pass filter.
// Process one sample at a time. State lives here.
class Filter
{
public:
    void setSampleRate (float sampleRate);
    void setCutoffHz (float cutoffHz);
    void setResonance (float resonance01); // unused for a pure 1-pole LP; keep for API growth

    void reset();

    // y = filter(x)
    float process (float input);

private:
    void updateCoefficient();

    float sampleRate_ = 44100.0f;
    float cutoffHz_ = 1000.0f;
    float resonance_ = 0.0f;

    float z1_ = 0.0f;
    float alpha_ = 0.0f;
};
