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

    for (int i = 0; i < numSamples; ++i)
    {
        float osc1Sample = osc1_.process() * osc1Level_ * (osc1On_ ? 1.0f : 0.0f);
        float osc2Sample = osc2_.process() * osc2Level_ * (osc2On_ ? 1.0f : 0.0f);
        float osc3Sample = osc3_.process() * osc3Level_ * (osc3On_ ? 1.0f : 0.0f);
        float noiseSample = noise_.process() * noiseLevel_ * (noiseOn_ ? 1.0f : 0.0f);

        osc1Sample = osc1Sample * envelope1_.getNextSample();
        osc2Sample = osc2Sample * envelope2_.getNextSample();
        osc3Sample = osc3Sample * envelope3_.getNextSample();
        noiseSample = noiseSample * envelope4_.getNextSample();

        float osc1L, osc1R;
        float osc2L, osc2R;
        float osc3L, osc3R;
        float noiseL, noiseR;

        applyPan (osc1Sample, osc1Pan_, osc1L, osc1R);
        applyPan (osc2Sample, osc2Pan_, osc2L, osc2R);
        applyPan (osc3Sample, osc3Pan_, osc3L, osc3R);
        applyPan (noiseSample, noisePan_, noiseL, noiseR);

        // Four unit-gain sources summed; scale so full levels stay near ±1 peak.
        constexpr float kMixScale = 0.25f;
        const float oscL = (osc1L + osc2L + osc3L) * kMixScale;
        const float oscR = (osc1R + osc2R + osc3R) * kMixScale;
        noiseL = noiseL * kMixScale;
        noiseR = noiseR * kMixScale;

        float oscFilteredL, oscFilteredR;
        float noiseFilteredL, noiseFilteredR;

        oscFilter_.process (oscL, oscR, oscFilteredL, oscFilteredR);
        noiseFilter_.process (noiseL, noiseR, noiseFilteredL, noiseFilteredR);

        const float sampleL = (oscFilteredL + noiseFilteredL) * level_;
        const float sampleR = (oscFilteredR + noiseFilteredR) * level_;

        const int numChannels = outputBuffer.getNumChannels();

        if (numChannels >= 2)
        {
            outputBuffer.addSample (0, startSample + i, sampleL);
            outputBuffer.addSample (1, startSample + i, sampleR);
        }
        else if (numChannels >= 1)
            outputBuffer.addSample (0, startSample + i, (sampleL + sampleR) * 0.5f);

        if (! isEnvelopeActive())
        {
            clearCurrentNote();
            level_ = 0.0f;
            break;
        }
    }
}
