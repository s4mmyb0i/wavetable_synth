#pragma once

#include "dsp/WavetableOsc.h"
#include "dsp/Envelope.h"
#include "dsp/Filter.h"
#include <juce_audio_basics/juce_audio_basics.h>

class Wavetable;
class WavetableMipBank;

struct OscSource
{
    const Wavetable* table = nullptr;          // sine / triangle
    const WavetableMipBank* mipBank = nullptr; // saw / square
};

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

    // oscIndex: 0 = osc1, 1 = osc2, 2 = osc3.
    // Pass either table OR mipBank (the other nullptr).
    void setOscWave (int oscIndex, const Wavetable* table, const WavetableMipBank* mipBank);

    void setOscUnison (int oscIndex, int unisonCount, float detuneCents);
    void setOscWarp (int oscIndex, WarpMode mode, float amount);

    void setEnvelopeParameters (const Envelope::Parameters& params)
    { envelope_.setParameters (params); }

    void setFilterCutoff (float cutoffHz) { filter_.setCutoffHz (cutoffHz); }
    void setFilterResonance (float resonance01) { filter_.setResonance (resonance01); }

    void setOscLevels (float osc1Level, float osc2Level, float osc3Level);
    void setOscDetuneCents (float detuneBy, float osc2Detune, float osc3Detune);

private:
    void applyOscFrequencies();
    void refreshOscTables();
    void applySourceToOsc (WavetableOsc& osc, const OscSource& source, float frequencyHz);

    float frequencyForCents (float cents) const
    { return noteFrequencyHz_ * std::pow (2.0f, cents / 1200.0f); }

    OscSource& sourceForIndex (int oscIndex);
    WavetableOsc& oscForIndex (int oscIndex);

    WavetableOsc osc1_, osc2_, osc3_;
    OscSource osc1Source_, osc2Source_, osc3Source_;

    float osc1Level_ = 1.0f;
    float osc2Level_ = 0.0f;
    float osc3Level_ = 0.0f;
    float osc2Detune_ = 7.0f;
    float osc3Detune_ = -7.0f;

    Filter filter_;
    Envelope envelope_;
    float level_ = 0.0f;
    float noteFrequencyHz_ = 440.0f;
};
