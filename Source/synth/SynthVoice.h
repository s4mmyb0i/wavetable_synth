#pragma once

#include "dsp/WavetableOsc.h"
#include <juce_audio_basics/juce_audio_basics.h>

class Wavetable;

class SynthVoice final : public juce::SynthesiserVoice
{
public:
    explicit SynthVoice (const Wavetable& table);

    bool canPlaySound (juce::SynthesiserSound* sound) override;
    void startNote (int midiNoteNumber, float velocity,
                    juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote (float velocity, bool allowTailOff) override;
    void pitchWheelMoved (int newPitchWheelValue) override;
    void controllerMoved (int controllerNumber, int newControllerValue) override;
    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer,
                          int startSample, int numSamples) override;
    
    using juce::SynthesiserVoice::renderNextBlock;
    void setCurrentPlaybackSampleRate (double newRate) override;

private:
    WavetableOsc osc_;
    float level_ = 0.0f;
};