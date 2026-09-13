#pragma once

class Wavetable;

// Reads a wavetable at a rate set by pitch. Phase lives here only.
class WavetableOsc
{
public:
    explicit WavetableOsc (const Wavetable& table);

    void setSampleRate (float sampleRate);
    void setFrequency (float hz);
    void reset();

    // One output sample, then advance phase.
    float process();

private:
    const Wavetable* table_ = nullptr;

    float sampleRate_ = 44100.0f;
    float frequency_ = 0.0f;
    float phase_ = 0.0f;     // [0, 1)
    float increment_ = 0.0f; // frequency / sampleRate
};
