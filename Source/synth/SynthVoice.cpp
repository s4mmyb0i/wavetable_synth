#include "synth/SynthVoice.h"
#include "synth/SynthSound.h"
#include "dsp/Wavetable.h"
#include "dsp/WavetableBank.h"
#include "dsp/WavetableMipBank.h"
#include "dsp/Envelope.h"
#include "dsp/MathConstants.h"

#include <cmath>

SynthVoice::SynthVoice (const Wavetable& table) : osc1_ (table), osc2_ (table), osc3_ (table)
{
    osc1Source_ = {&table, nullptr};
    osc2Source_ = {&table, nullptr};
    osc3Source_ = {&table, nullptr};

    osc1_.setUnison (1, 0.0f);
    osc2_.setUnison (1, 0.0f);
    osc3_.setUnison (1, 0.0f);

    osc1_.setWarp (WarpMode::off, 0.0f);
    osc2_.setWarp (WarpMode::off, 0.0f);
    osc3_.setWarp (WarpMode::off, 0.0f);
}

void SynthVoice::setOscUnison (int oscIndex, int unisonCount, float detuneCents)
{ oscForIndex (oscIndex).setUnison (unisonCount, detuneCents); }

void SynthVoice::setOscWarp (int oscIndex, WarpMode mode, float amount)
{ oscForIndex (oscIndex).setWarp (mode, amount); }

bool SynthVoice::canPlaySound (juce::SynthesiserSound* sound)
{ return dynamic_cast<SynthSound*> (sound) != nullptr; }

OscSource& SynthVoice::sourceForIndex (int oscIndex)
{
    switch (oscIndex)
    {
    case 1: return osc2Source_;
    case 2: return osc3Source_;
    default: return osc1Source_;
    }
}

WavetableOsc& SynthVoice::oscForIndex (int oscIndex)
{
    switch (oscIndex)
    {
    case 1: return osc2_;
    case 2: return osc3_;
    default: return osc1_;
    }
}

void SynthVoice::applySourceToOsc (WavetableOsc& osc, const OscSource& source, float frequencyHz)
{
    if (source.bank != nullptr)
    {
        osc.setBank (*source.bank);
        return;
    }

    if (source.mipBank != nullptr)
    {
        const double sr = getSampleRate();
        if (sr > 0.0 && frequencyHz > 0.0f)
            osc.setTable (source.mipBank->select (frequencyHz, static_cast<float> (sr)));
        return;
    }

    if (source.table != nullptr) osc.setTable (*source.table);
}

void SynthVoice::refreshOscTables()
{
    applySourceToOsc (osc1_, osc1Source_, noteFrequencyHz_);
    applySourceToOsc (osc2_, osc2Source_, frequencyForCents (osc2Detune_));
    applySourceToOsc (osc3_, osc3Source_, frequencyForCents (osc3Detune_));
}

void SynthVoice::setOscWave (int oscIndex, const Wavetable* table, const WavetableMipBank* mipBank,
                             const WavetableBank* bank)
{
    auto& source = sourceForIndex (oscIndex);
    source.table = table;
    source.mipBank = mipBank;
    source.bank = bank;

    float freq = noteFrequencyHz_;
    if (oscIndex == 1)
        freq = frequencyForCents (osc2Detune_);
    else if (oscIndex == 2)
        freq = frequencyForCents (osc3Detune_);

    applySourceToOsc (oscForIndex (oscIndex), source, freq);
}

void SynthVoice::setOscPosition (int oscIndex, float position01)
{ oscForIndex (oscIndex).setPosition (position01); }

void SynthVoice::applyOscFrequencies()
{
    if (noteFrequencyHz_ <= 0.0f) return;

    osc1_.setFrequency (noteFrequencyHz_);
    osc2_.setFrequency (frequencyForCents (osc2Detune_));
    osc3_.setFrequency (frequencyForCents (osc3Detune_));

    refreshOscTables();
}

void SynthVoice::setOscLevels (float osc1Level, float osc2Level, float osc3Level)
{
    osc1Level_ = osc1Level;
    osc2Level_ = osc2Level;
    osc3Level_ = osc3Level;
}

void SynthVoice::setNoise (NoiseGenerator::Type type, float level)
{
    noise_.setType (type);
    noiseLevel_ = level;
}

void SynthVoice::setSourceEnabled (bool osc1On, bool osc2On, bool osc3On, bool noiseOn)
{
    osc1On_ = osc1On;
    osc2On_ = osc2On;
    osc3On_ = osc3On;
    noiseOn_ = noiseOn;
}

void SynthVoice::setOscDetuneCents (float detuneBy, float osc2Detune, float osc3Detune)
{
    osc2Detune_ = osc2Detune + detuneBy;
    osc3Detune_ = osc3Detune + detuneBy;

    if (isVoiceActive()) applyOscFrequencies();
}

void SynthVoice::setOscPan (float osc1Pan, float osc2Pan, float osc3Pan)
{
    osc1Pan_ = std::clamp (osc1Pan, -1.0f, 1.0f);
    osc2Pan_ = std::clamp (osc2Pan, -1.0f, 1.0f);
    osc3Pan_ = std::clamp (osc3Pan, -1.0f, 1.0f);
}

void SynthVoice::setNoisePan (float noisePan)
{ noisePan_ = std::clamp (noisePan, -1.0f, 1.0f); }

void SynthVoice::applyPan (float sample, float pan, float& L, float& R) const
{
    pan = std::clamp (pan, -1.0f, 1.0f);
    const float angle = (pan + 1.0f) * static_cast<float> (dsp::kPi) * 0.25f;
    L = sample * std::cos (angle);
    R = sample * std::sin (angle);
}

void SynthVoice::setEnvelopeParameters (const Envelope::Parameters& param1,
                                        const Envelope::Parameters& param2,
                                        const Envelope::Parameters& param3,
                                        const Envelope::Parameters& param4)
{
    envelope1_.setParameters (param1);
    envelope2_.setParameters (param2);
    envelope3_.setParameters (param3);
    envelope4_.setParameters (param4);
}

void SynthVoice::startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound* sound,
                            int currentPitchWheelPosition)
{
    (void) sound;
    (void) currentPitchWheelPosition;

    noteFrequencyHz_ = static_cast<float> (juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber));
    applyOscFrequencies();

    osc1_.reset();
    osc2_.reset();
    osc3_.reset();

    oscFilter_.reset();
    noiseFilter_.reset();

    level_ = std::clamp (velocity, 0.0f, 1.0f);
    envelope1_.noteOn();
    envelope2_.noteOn();
    envelope3_.noteOn();
    envelope4_.noteOn();
}

void SynthVoice::stopNote (float velocity, bool allowTailOff)
{
    (void) velocity;

    if (allowTailOff)
    {
        envelope1_.noteOff();
        envelope2_.noteOff();
        envelope3_.noteOff();
        envelope4_.noteOff();
    }
    else
    {
        envelope1_.reset();
        envelope2_.reset();
        envelope3_.reset();
        envelope4_.reset();
        clearCurrentNote();
        level_ = 0.0f;
        osc1_.reset();
        osc2_.reset();
        osc3_.reset();
    }
}

void SynthVoice::pitchWheelMoved (int) {}
void SynthVoice::controllerMoved (int, int) {}

void SynthVoice::setCurrentPlaybackSampleRate (double newRate)
{
    juce::SynthesiserVoice::setCurrentPlaybackSampleRate (newRate);

    osc1_.setSampleRate (static_cast<float> (newRate));
    osc2_.setSampleRate (static_cast<float> (newRate));
    osc3_.setSampleRate (static_cast<float> (newRate));

    oscFilter_.setSampleRate (static_cast<float> (newRate));
    noiseFilter_.setSampleRate (static_cast<float> (newRate));

    envelope1_.setSampleRate (newRate);
    envelope2_.setSampleRate (newRate);
    envelope3_.setSampleRate (newRate);
    envelope4_.setSampleRate (newRate);

    refreshOscTables();
}

void SynthVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample,
                                  int numSamples)
{
    if (! isEnvelopeActive()) return;

    const bool runOsc1 = osc1On_ && osc1Level_ > 0.0f;
    const bool runOsc2 = osc2On_ && osc2Level_ > 0.0f;
    const bool runOsc3 = osc3On_ && osc3Level_ > 0.0f;
    const bool runNoise = noiseOn_ && noiseLevel_ > 0.0f;
    const bool runOscBus = runOsc1 || runOsc2 || runOsc3;

    const int numChannels = outputBuffer.getNumChannels();
    float* left = numChannels >= 1 ? outputBuffer.getWritePointer (0, startSample) : nullptr;
    float* right = numChannels >= 2 ? outputBuffer.getWritePointer (1, startSample) : nullptr;

    // All sources muted: still tick envelopes so note-off / voice lifetime stay correct.
    if (! runOscBus && ! runNoise)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            envelope1_.getNextSample();
            envelope2_.getNextSample();
            envelope3_.getNextSample();
            envelope4_.getNextSample();

            if (! isEnvelopeActive())
            {
                clearCurrentNote();
                level_ = 0.0f;
                break;
            }
        }
        return;
    }

    constexpr float kMixScale = 0.25f;

    for (int i = 0; i < numSamples; ++i)
    {
        // Always advance envelopes so re-enabling a source mid-note keeps phase with ADSR.
        const float env1 = envelope1_.getNextSample();
        const float env2 = envelope2_.getNextSample();
        const float env3 = envelope3_.getNextSample();
        const float env4 = envelope4_.getNextSample();

        float osc1L = 0.0f, osc1R = 0.0f;
        float osc2L = 0.0f, osc2R = 0.0f;
        float osc3L = 0.0f, osc3R = 0.0f;

        if (runOsc1)
        {
            const float s = osc1_.process() * osc1Level_ * env1;
            applyPan (s, osc1Pan_, osc1L, osc1R);
        }
        if (runOsc2)
        {
            const float s = osc2_.process() * osc2Level_ * env2;
            applyPan (s, osc2Pan_, osc2L, osc2R);
        }
        if (runOsc3)
        {
            const float s = osc3_.process() * osc3Level_ * env3;
            applyPan (s, osc3Pan_, osc3L, osc3R);
        }

        float oscFilteredL = 0.0f, oscFilteredR = 0.0f;
        if (runOscBus)
        {
            const float oscL = (osc1L + osc2L + osc3L) * kMixScale;
            const float oscR = (osc1R + osc2R + osc3R) * kMixScale;
            oscFilter_.process (oscL, oscR, oscFilteredL, oscFilteredR);
        }

        float noiseFilteredL = 0.0f, noiseFilteredR = 0.0f;
        if (runNoise)
        {
            float noiseL, noiseR;
            const float s = noise_.process() * noiseLevel_ * env4;
            applyPan (s, noisePan_, noiseL, noiseR);
            noiseFilter_.process (noiseL * kMixScale, noiseR * kMixScale, noiseFilteredL,
                                  noiseFilteredR);
        }

        const float sampleL = (oscFilteredL + noiseFilteredL) * level_;
        const float sampleR = (oscFilteredR + noiseFilteredR) * level_;

        if (left != nullptr && right != nullptr)
        {
            left[i] += sampleL;
            right[i] += sampleR;
        }
        else if (left != nullptr) { left[i] += (sampleL + sampleR) * 0.5f; }

        if (! isEnvelopeActive())
        {
            clearCurrentNote();
            level_ = 0.0f;
            break;
        }
    }
}
