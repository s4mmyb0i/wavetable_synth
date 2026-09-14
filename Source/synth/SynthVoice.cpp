#include "synth/SynthVoice.h"
#include "synth/SynthSound.h"
#include "dsp/Wavetable.h"
#include "dsp/Envelope.h"

SynthVoice::SynthVoice (const Wavetable& table)
    : osc_ (table)
{
}

bool SynthVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*>(sound) != nullptr;
}

void SynthVoice::startNote (int midiNoteNumber, float velocity,
                            juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    (void) sound;
    (void) currentPitchWheelPosition;

    float frequency = static_cast<float>(juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber));
    osc_.setFrequency(frequency);
    osc_.reset();
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
        osc_.reset();
    }
}

void SynthVoice::pitchWheelMoved (int) {}
void SynthVoice::controllerMoved (int, int) {}

void SynthVoice::setCurrentPlaybackSampleRate (double newRate)
{
    juce::SynthesiserVoice::setCurrentPlaybackSampleRate (newRate);
    osc_.setSampleRate (static_cast<float> (newRate));
    envelope_.setSampleRate (newRate);
}

void SynthVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer,
                                  int startSample, int numSamples)
{
    if (! envelope_.isActive())
        return;

    for (int i = 0; i < numSamples; ++i)
    {
        const float env = envelope_.getNextSample();
        const float sample = osc_.process() * level_ * env;

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