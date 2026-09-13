#include "PluginProcessor.h"
#include "PluginEditor.h"

WavetableSynthAudioProcessor::WavetableSynthAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      wavetable_ (Wavetable::sine()),
      osc_ (wavetable_)
{
}

WavetableSynthAudioProcessor::~WavetableSynthAudioProcessor() = default;

const juce::String WavetableSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool WavetableSynthAudioProcessor::acceptsMidi() const { return true; }
bool WavetableSynthAudioProcessor::producesMidi() const { return false; }
bool WavetableSynthAudioProcessor::isMidiEffect() const { return false; }

double WavetableSynthAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int WavetableSynthAudioProcessor::getNumPrograms() { return 1; }
int WavetableSynthAudioProcessor::getCurrentProgram() { return 0; }

void WavetableSynthAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String WavetableSynthAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void WavetableSynthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

void WavetableSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (samplesPerBlock);
    juce::ignoreUnused (sampleRate);
    osc_.setSampleRate(static_cast<float>(sampleRate));
}

void WavetableSynthAudioProcessor::releaseResources() {}

bool WavetableSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& output = layouts.getMainOutputChannelSet();
    return output == juce::AudioChannelSet::mono()
        || output == juce::AudioChannelSet::stereo();
}

void WavetableSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                                 juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    for (const auto metadata : midiMessages)
    {
        const auto msg = metadata.getMessage();
        if (msg.isNoteOn())
        {
            noteOn_ = true;
            osc_.reset();
            osc_.setFrequency ((float) msg.getMidiNoteInHertz(msg.getNoteNumber()));
            // or: juce::MidiMessage::getMidiNoteInHertz (msg.getNoteNumber())
        }
        else if (msg.isNoteOff())
        {
            noteOn_ = false;
        }
    }

    float value;
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        if (noteOn_) value = static_cast<float>(osc_.process() * 0.2);
        else value = 0;
        
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            buffer.setSample(ch, sample, static_cast<float>(value));
    }

}

bool WavetableSynthAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* WavetableSynthAudioProcessor::createEditor()
{
    return new WavetableSynthAudioProcessorEditor (*this);
}

void WavetableSynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::ignoreUnused (destData);
}

void WavetableSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    juce::ignoreUnused (data, sizeInBytes);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WavetableSynthAudioProcessor();
}
