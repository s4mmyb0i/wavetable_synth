#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "constants.h"
#include "synth/SynthSound.h"
#include "synth/SynthVoice.h"
#include "params/ParameterLayout.h"
#include "params/ParamIDs.h"

WavetableSynthAudioProcessor::WavetableSynthAudioProcessor()
    : AudioProcessor (
          BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      sineTable_ (Wavetable::sine()), apvts_ (*this, nullptr, "PARAMS", createParameterLayout())
{
    sawMipBank_.build (WavetableMipBank::Shape::saw);
    squareMipBank_.build (WavetableMipBank::Shape::square);

    for (int voice = 0; voice < NUM_VOICES; ++voice) synth_.addVoice (new SynthVoice (sineTable_));

    synth_.addSound (new SynthSound());
}

WavetableSynthAudioProcessor::~WavetableSynthAudioProcessor() = default;

const juce::String WavetableSynthAudioProcessor::getName() const
{ return JucePlugin_Name; }

bool WavetableSynthAudioProcessor::acceptsMidi() const
{ return true; }
bool WavetableSynthAudioProcessor::producesMidi() const
{ return false; }
bool WavetableSynthAudioProcessor::isMidiEffect() const
{ return false; }

double WavetableSynthAudioProcessor::getTailLengthSeconds() const
{ return 0.0; }

int WavetableSynthAudioProcessor::getNumPrograms()
{ return 1; }
int WavetableSynthAudioProcessor::getCurrentProgram()
{ return 0; }

void WavetableSynthAudioProcessor::setCurrentProgram (int index)
{ juce::ignoreUnused (index); }

const juce::String WavetableSynthAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void WavetableSynthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{ juce::ignoreUnused (index, newName); }

void WavetableSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (samplesPerBlock);
    synth_.setCurrentPlaybackSampleRate (sampleRate);
}

void WavetableSynthAudioProcessor::releaseResources() {}

bool WavetableSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& output = layouts.getMainOutputChannelSet();
    return output == juce::AudioChannelSet::mono() || output == juce::AudioChannelSet::stereo();
}

void WavetableSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                                 juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    // Osc mix/detune
    const float osc1Level = apvts_.getRawParameterValue (ParamIDs::osc1Level)->load();
    const float osc2Level = apvts_.getRawParameterValue (ParamIDs::osc2Level)->load();
    const float osc3Level = apvts_.getRawParameterValue (ParamIDs::osc3Level)->load();
    const float osc2Detune = apvts_.getRawParameterValue (ParamIDs::osc2Detune)->load();
    const float osc3Detune = apvts_.getRawParameterValue (ParamIDs::osc3Detune)->load();

    // Resolve each osc's wave choice → fixed table and/or mip bank (exactly one used).
    const auto resolveWave =
        [this] (int waveIndex, const Wavetable*& table, const WavetableMipBank*& mipBank)
    {
        table = &sineTable_;
        mipBank = nullptr;

        switch (waveIndex)
        {
        case 1:
            table = nullptr;
            mipBank = &sawMipBank_;
            break;
        case 2:
            table = nullptr;
            mipBank = &squareMipBank_;
            break;
        case 3: table = &triangleTable_; break;
        default: break; // sine
        }
    };

    const int osc1Wave =
        static_cast<int> (apvts_.getRawParameterValue (ParamIDs::osc1Wave)->load());
    const int osc2Wave =
        static_cast<int> (apvts_.getRawParameterValue (ParamIDs::osc2Wave)->load());
    const int osc3Wave =
        static_cast<int> (apvts_.getRawParameterValue (ParamIDs::osc3Wave)->load());

    const Wavetable* osc1Table = nullptr;
    const Wavetable* osc2Table = nullptr;
    const Wavetable* osc3Table = nullptr;
    const WavetableMipBank* osc1Mip = nullptr;
    const WavetableMipBank* osc2Mip = nullptr;
    const WavetableMipBank* osc3Mip = nullptr;

    resolveWave (osc1Wave, osc1Table, osc1Mip);
    resolveWave (osc2Wave, osc2Table, osc2Mip);
    resolveWave (osc3Wave, osc3Table, osc3Mip);

    // Filter
    const float cutoff = apvts_.getRawParameterValue (ParamIDs::cutoff)->load();
    const float resonance = apvts_.getRawParameterValue (ParamIDs::resonance)->load();

    // ADSR
    Envelope::Parameters envParams;
    envParams.attackSeconds = apvts_.getRawParameterValue (ParamIDs::attack)->load();
    envParams.decaySeconds = apvts_.getRawParameterValue (ParamIDs::decay)->load();
    envParams.sustainLevel = apvts_.getRawParameterValue (ParamIDs::sustain)->load();
    envParams.releaseSeconds = apvts_.getRawParameterValue (ParamIDs::release)->load();

    for (int i = 0; i < synth_.getNumVoices(); ++i)
        if (SynthVoice* voice = dynamic_cast<SynthVoice*> (synth_.getVoice (i)))
        {
            voice->setOscWave (0, osc1Table, osc1Mip);
            voice->setOscWave (1, osc2Table, osc2Mip);
            voice->setOscWave (2, osc3Table, osc3Mip);

            voice->setFilterCutoff (cutoff);
            voice->setFilterResonance (resonance);
            voice->setEnvelopeParameters (envParams);

            voice->setOscLevels (osc1Level, osc2Level, osc3Level);
            voice->setOscDetuneCents (0.0f, osc2Detune, osc3Detune);
        }

    synth_.renderNextBlock (buffer, midiMessages, 0, buffer.getNumSamples());
}

bool WavetableSynthAudioProcessor::hasEditor() const
{ return true; }

juce::AudioProcessorEditor* WavetableSynthAudioProcessor::createEditor()
{ return new WavetableSynthAudioProcessorEditor (*this); }

void WavetableSynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = apvts_.copyState().createXml()) copyXmlToBinary (*xml, destData);
}

void WavetableSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    juce::ignoreUnused (data, sizeInBytes);

    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (apvts_.state.getType()))
            apvts_.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{ return new WavetableSynthAudioProcessor(); }
