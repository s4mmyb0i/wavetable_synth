#include "synth/SynthVoice.h"
#include "synth/SynthSound.h"
#include "dsp/Wavetable.h"
#include "dsp/WavetableMipBank.h"
#include "dsp/Envelope.h"

#include <cmath>

SynthVoice::SynthVoice (const Wavetable& table) : osc1_ (table), osc2_ (table), osc3_ (table) {}

bool SynthVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*> (sound) != nullptr;
}

void SynthVoice::setWavetable (const Wavetable& table)
{
    // Fixed table: don't use mip selection for this voice right now.
    mipBank_ = nullptr;
    osc1_.setTable (table);
    osc2_.setTable (table);
    osc3_.setTable (table);
}

void SynthVoice::setMipBank (const WavetableMipBank* bank)
{
    mipBank_ = bank;
    refreshMipTable();
}

float SynthVoice::frequencyForCents (float cents) const
{
    return noteFrequencyHz_ * std::pow (2.0f, cents / 1200.0f);
}

void SynthVoice::applyOscFrequencies()
{
    if (noteFrequencyHz_ <= 0.0f)
        return;

    osc1_.setFrequency (noteFrequencyHz_);
    osc2_.setFrequency (frequencyForCents (osc2Detune_));
    osc3_.setFrequency (frequencyForCents (osc3Detune_));

    refreshMipTable();
}

void SynthVoice::refreshMipTable()
{
    if (mipBank_ == nullptr)
        return;

    const double sr = getSampleRate();
    if (sr <= 0.0 || noteFrequencyHz_ <= 0.0f)
        return;

    const float srF = static_cast<float> (sr);

    osc1_.setTable (mipBank_->select (noteFrequencyHz_, srF));
    osc2_.setTable (mipBank_->select (frequencyForCents (osc2Detune_), srF));
    osc3_.setTable (mipBank_->select (frequencyForCents (osc3Detune_), srF));
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

    if (isVoiceActive())
        applyOscFrequencies();
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
    refreshMipTable();
}

void SynthVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample,
                                  int numSamples)
{
    if (! envelope_.isActive())
        return;

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
