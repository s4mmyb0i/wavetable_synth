#pragma once

#include "dsp/WavetableOsc.h"
#include "dsp/Envelope.h"
#include "dsp/Filter.h"
#include <juce_audio_basics/juce_audio_basics.h>

class Wavetable;
class WavetableMipBank;

class SynthVoice final : public juce::SynthesiserVoice
{
public:
    explicit SynthVoice (const Wavetable& table);

    bool canPlaySound (juce::SynthesiserSound* sound) override;
    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound* sound,
                    int currentPitchWheelPosition) override;
    void stopNote (float velocity, bool allowTailOff) override;
    void pitchWheelMoved (int newPitchWheelValue) override;
    void controllerMoved (int controllerNumber, int newControllerValue) override;
    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample,
                          int numSamples) override;

    using juce::SynthesiserVoice::renderNextBlock;
    void setCurrentPlaybackSampleRate (double newRate) override;

    // Single-table path (sine / triangle, or any fixed table).
    void setWavetable (const Wavetable& table);

    // Band-limited path: voice will pick a mip from this bank using note frequency.
    // Pass nullptr to clear and go back to setWavetable-only behaviour.
    void setMipBank (const WavetableMipBank* bank);

    void setEnvelopeParameters (const Envelope::Parameters& params)
    {
        envelope_.setParameters (params);
    }

    void setFilterCutoff (float cutoffHz) { filter_.setCutoffHz (cutoffHz); }
    void setFilterResonance (float resonance01) { filter_.setResonance (resonance01); }

    void setOscLevels (float osc1Level, float osc2Level, float osc3Level);
    void setOscDetuneCents (float detuneBy, float osc2Detune, float osc3Detune);

private:
    // Apply note + detune to each osc; refresh mip tables for those frequencies.
    void applyOscFrequencies();

    // If mipBank_ != nullptr, select a table per osc from its (possibly detuned) frequency.
    void refreshMipTable();

    float frequencyForCents (float cents) const;

    WavetableOsc osc1_;
    WavetableOsc osc2_;
    WavetableOsc osc3_;

    // Match APVTS defaults: osc1 only until the user raises osc2/3.
    float osc1Level_ = 1.0f;
    float osc2Level_ = 0.0f;
    float osc3Level_ = 0.0f;
    float osc2Detune_ = 7.0f;
    float osc3Detune_ = -7.0f;

    Filter filter_;
    Envelope envelope_;
    float level_ = 0.0f;

    const WavetableMipBank* mipBank_ = nullptr;
    float noteFrequencyHz_ = 440.0f;
};
