#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "constants.h"
#include "synth/SynthSound.h"
#include "synth/SynthVoice.h"

WavetableSynthAudioProcessor::WavetableSynthAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
    wavetable_ (Wavetable::sine())
{
    for (int voice = 0; voice < NUM_VOICES; ++voice)
    {
        synth_.addVoice(new SynthVoice(wavetable_));
    }
    synth_.addSound(new SynthSound());
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
    synth_.setCurrentPlaybackSampleRate(sampleRate);
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
    synth_.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
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
