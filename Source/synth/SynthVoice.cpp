#include "synth/SynthVoice.h"
#include "synth/SynthSound.h"
#include "dsp/Wavetable.h"
#include "dsp/WavetableMipBank.h"
#include "dsp/Envelope.h"

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

void SynthVoice::setOscWave (int oscIndex, const Wavetable* table, const WavetableMipBank* mipBank)
{
    auto& source = sourceForIndex (oscIndex);
    source.table = table;
    source.mipBank = mipBank;

    float freq = noteFrequencyHz_;
    if (oscIndex == 1)
        freq = frequencyForCents (osc2Detune_);
    else if (oscIndex == 2)
        freq = frequencyForCents (osc3Detune_);

    applySourceToOsc (oscForIndex (oscIndex), source, freq);
}

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

void SynthVoice::setOscDetuneCents (float detuneBy, float osc2Detune, float osc3Detune)
{
    osc2Detune_ = osc2Detune + detuneBy;
    osc3Detune_ = osc3Detune + detuneBy;

    if (isVoiceActive()) applyOscFrequencies();
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

    filter_.reset();
    level_ = velocity * 0.15f;
    envelope_.noteOn();
}

void SynthVoice::stopNote (float velocity, bool allowTailOff)
{
    (void) velocity;

    if (allowTailOff)
        envelope_.noteOff();
    else
    {
        envelope_.reset();
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

    filter_.setSampleRate (static_cast<float> (newRate));
    envelope_.setSampleRate (newRate);
    refreshOscTables();
}

void SynthVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample,
                                  int numSamples)
{
    if (! envelope_.isActive()) return;

    for (int i = 0; i < numSamples; ++i)
    {
        const float mixed = (osc1_.process() * osc1Level_ + osc2_.process() * osc2Level_ +
                             osc3_.process() * osc3Level_) *
                            0.80f;
        const float filtered = filter_.process (mixed);
        const float env = envelope_.getNextSample();
        const float sample = filtered * level_ * env;

        for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
            outputBuffer.addSample (ch, startSample + i, sample);

        if (! envelope_.isActive())
        {
            clearCurrentNote();
            level_ = 0.0f;
            break;
        }
    }
}
