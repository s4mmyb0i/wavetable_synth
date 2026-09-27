#include "synth/SynthVoice.h"
#include "synth/SynthSound.h"
#include "dsp/Wavetable.h"
#include "dsp/WavetableMipBank.h"
#include "dsp/Envelope.h"

SynthVoice::SynthVoice (const Wavetable& table)
    : osc_ (table)
{
}

bool SynthVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*> (sound) != nullptr;
}

void SynthVoice::setWavetable (const Wavetable& table)
{
    // Fixed table: don't use mip selection for this voice right now.
    mipBank_ = nullptr;
    osc_.setTable (table);
}

void SynthVoice::setMipBank (const WavetableMipBank* bank)
{
    mipBank_ = bank;
    refreshMipTable();
}

void SynthVoice::refreshMipTable()
{
    if (mipBank_ == nullptr)
        return;

    const double sr = getSampleRate();
    if (sr <= 0.0 || noteFrequencyHz_ <= 0.0f)
        return;

    osc_.setTable (mipBank_->select (noteFrequencyHz_, static_cast<float> (sr)));
}

void SynthVoice::startNote (int midiNoteNumber, float velocity,
                            juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    (void) sound;
    (void) currentPitchWheelPosition;

    noteFrequencyHz_ = static_cast<float> (juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber));
    osc_.setFrequency (noteFrequencyHz_);

    // Pick band-limited mip for this pitch (no-op if mipBank_ is null).
    refreshMipTable();

    osc_.reset();
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
        osc_.reset();
    }
}

void SynthVoice::pitchWheelMoved (int) {}
void SynthVoice::controllerMoved (int, int) {}

void SynthVoice::setCurrentPlaybackSampleRate (double newRate)
{
    juce::SynthesiserVoice::setCurrentPlaybackSampleRate (newRate);
    osc_.setSampleRate (static_cast<float> (newRate));
    filter_.setSampleRate (static_cast<float> (newRate));
    envelope_.setSampleRate (newRate);
    refreshMipTable();
}

void SynthVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer,
                                  int startSample, int numSamples)
{
    if (! envelope_.isActive())
        return;

    for (int i = 0; i < numSamples; ++i)
    {
        const float filtered = filter_.process (osc_.process());
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
