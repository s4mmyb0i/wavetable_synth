#include "synth/SynthVoice.h"
#include "synth/SynthSound.h"
#include "dsp/Wavetable.h"

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

    osc_.reset();
    float frequency = static_cast<float>(juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber));
    osc_.setFrequency(frequency);
    level_ = velocity * 0.15f;
}

void SynthVoice::stopNote (float velocity, bool allowTailOff)
{
    (void) velocity;
    (void) allowTailOff;
    
    clearCurrentNote();
    level_ = 0.0f;
    osc_.reset();
}

void SynthVoice::pitchWheelMoved (int) {}
void SynthVoice::controllerMoved (int, int) {}

void SynthVoice::setCurrentPlaybackSampleRate (double newRate)
{
    SynthesiserVoice::setCurrentPlaybackSampleRate(newRate);
    osc_.setSampleRate(static_cast<float>(newRate));
}

void SynthVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer,
                                  int startSample, int numSamples)
{
    if (level_ == 0.0f) return;

    for (int i = 0; i < numSamples; ++i)
    {
        float sample = osc_.process() * level_;
        
        int chs = outputBuffer.getNumChannels();
        for (int ch = 0; ch < chs; ++ch)
        {
            outputBuffer.addSample(ch, startSample + i, sample);
        }
    }

}