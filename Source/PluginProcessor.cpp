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

    // Per-source enable
    const bool osc1On = apvts_.getRawParameterValue (ParamIDs::osc1On)->load() > 0.5f;
    const bool osc2On = apvts_.getRawParameterValue (ParamIDs::osc2On)->load() > 0.5f;
    const bool osc3On = apvts_.getRawParameterValue (ParamIDs::osc3On)->load() > 0.5f;
    const bool noiseOn = apvts_.getRawParameterValue (ParamIDs::noiseOn)->load() > 0.5f;

    // Osc mix/detune
    const float osc1Level = apvts_.getRawParameterValue (ParamIDs::osc1Level)->load();
    const float osc2Level = apvts_.getRawParameterValue (ParamIDs::osc2Level)->load();
    const float osc3Level = apvts_.getRawParameterValue (ParamIDs::osc3Level)->load();
    const float osc2Detune = apvts_.getRawParameterValue (ParamIDs::osc2Detune)->load();
    const float osc3Detune = apvts_.getRawParameterValue (ParamIDs::osc3Detune)->load();

    const int osc1UnisonCount =
        static_cast<int> (apvts_.getRawParameterValue (ParamIDs::osc1UnisonCount)->load());
    const float osc1UnisonDetune = apvts_.getRawParameterValue (ParamIDs::osc1UnisonDetune)->load();
    const int osc2UnisonCount =
        static_cast<int> (apvts_.getRawParameterValue (ParamIDs::osc2UnisonCount)->load());
    const float osc2UnisonDetune = apvts_.getRawParameterValue (ParamIDs::osc2UnisonDetune)->load();
    const int osc3UnisonCount =
        static_cast<int> (apvts_.getRawParameterValue (ParamIDs::osc3UnisonCount)->load());
    const float osc3UnisonDetune = apvts_.getRawParameterValue (ParamIDs::osc3UnisonDetune)->load();

    // Resolve wave choice → exactly one of: table, mip bank, or morph bank.
    const auto resolveWave = [this] (int waveIndex, const Wavetable*& table,
                                     const WavetableMipBank*& mipBank, const WavetableBank*& bank)
    {
        table = &sineTable_;
        mipBank = nullptr;
        bank = nullptr;

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
        case 4:
            table = nullptr;
            bank = &morphBank_;
            break;
        default: break; // sine
        }
    };

    // Per osc warp
    const int osc1WarpModeIdx =
        static_cast<int> (apvts_.getRawParameterValue (ParamIDs::osc1WarpMode)->load());
    const float osc1WarpAmount = apvts_.getRawParameterValue (ParamIDs::osc1WarpAmount)->load();
    const int osc2WarpModeIdx =
        static_cast<int> (apvts_.getRawParameterValue (ParamIDs::osc2WarpMode)->load());
    const float osc2WarpAmount = apvts_.getRawParameterValue (ParamIDs::osc2WarpAmount)->load();
    const int osc3WarpModeIdx =
        static_cast<int> (apvts_.getRawParameterValue (ParamIDs::osc3WarpMode)->load());
    const float osc3WarpAmount = apvts_.getRawParameterValue (ParamIDs::osc3WarpAmount)->load();

    const float osc1Position = apvts_.getRawParameterValue (ParamIDs::osc1Position)->load();
    const float osc2Position = apvts_.getRawParameterValue (ParamIDs::osc2Position)->load();
    const float osc3Position = apvts_.getRawParameterValue (ParamIDs::osc3Position)->load();

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
    const WavetableBank* osc1Bank = nullptr;
    const WavetableBank* osc2Bank = nullptr;
    const WavetableBank* osc3Bank = nullptr;

    resolveWave (osc1Wave, osc1Table, osc1Mip, osc1Bank);
    resolveWave (osc2Wave, osc2Table, osc2Mip, osc2Bank);
    resolveWave (osc3Wave, osc3Table, osc3Mip, osc3Bank);

    const float osc1Pan = apvts_.getRawParameterValue (ParamIDs::osc1Pan)->load();
    const float osc2Pan = apvts_.getRawParameterValue (ParamIDs::osc2Pan)->load();
    const float osc3Pan = apvts_.getRawParameterValue (ParamIDs::osc3Pan)->load();

    // Noise
    const int noiseTypeIdx =
        static_cast<int> (apvts_.getRawParameterValue (ParamIDs::noiseType)->load());
    const float noiseLevel = apvts_.getRawParameterValue (ParamIDs::noiseLevel)->load();
    const auto noiseType =
        (noiseTypeIdx == 1) ? NoiseGenerator::Type::pink : NoiseGenerator::Type::white;
    const float noisePan = apvts_.getRawParameterValue (ParamIDs::noisePan)->load();

    // Filters
    const float oscCutoff = apvts_.getRawParameterValue (ParamIDs::cutoff)->load();
    const float oscResonance = apvts_.getRawParameterValue (ParamIDs::resonance)->load();
    const float noiseCutoff = apvts_.getRawParameterValue (ParamIDs::noiseCutoff)->load();
    const float noiseResonance = apvts_.getRawParameterValue (ParamIDs::noiseResonance)->load();

    // Per-source ADSR
    auto readEnv = [this] (const char* attackID, const char* decayID, const char* sustainID,
                           const char* releaseID) -> Envelope::Parameters
    {
        Envelope::Parameters p;
        p.attackSeconds = apvts_.getRawParameterValue (attackID)->load();
        p.decaySeconds = apvts_.getRawParameterValue (decayID)->load();
        p.sustainLevel = apvts_.getRawParameterValue (sustainID)->load();
        p.releaseSeconds = apvts_.getRawParameterValue (releaseID)->load();
        return p;
    };

    const auto env1 = readEnv (ParamIDs::osc1Attack, ParamIDs::osc1Decay, ParamIDs::osc1Sustain,
                               ParamIDs::osc1Release);
    const auto env2 = readEnv (ParamIDs::osc2Attack, ParamIDs::osc2Decay, ParamIDs::osc2Sustain,
                               ParamIDs::osc2Release);
    const auto env3 = readEnv (ParamIDs::osc3Attack, ParamIDs::osc3Decay, ParamIDs::osc3Sustain,
                               ParamIDs::osc3Release);
    const auto envNoise = readEnv (ParamIDs::noiseAttack, ParamIDs::noiseDecay,
                                   ParamIDs::noiseSustain, ParamIDs::noiseRelease);

    for (int i = 0; i < synth_.getNumVoices(); ++i)
        if (SynthVoice* voice = dynamic_cast<SynthVoice*> (synth_.getVoice (i)))
        {
            voice->setOscWave (0, osc1Table, osc1Mip, osc1Bank);
            voice->setOscWave (1, osc2Table, osc2Mip, osc2Bank);
            voice->setOscWave (2, osc3Table, osc3Mip, osc3Bank);

            voice->setOscPosition (0, osc1Position);
            voice->setOscPosition (1, osc2Position);
            voice->setOscPosition (2, osc3Position);

            voice->setOscWarp (0, static_cast<WarpMode> (osc1WarpModeIdx), osc1WarpAmount);
            voice->setOscWarp (1, static_cast<WarpMode> (osc2WarpModeIdx), osc2WarpAmount);
            voice->setOscWarp (2, static_cast<WarpMode> (osc3WarpModeIdx), osc3WarpAmount);

            voice->setOscUnison (0, osc1UnisonCount, osc1UnisonDetune);
            voice->setOscUnison (1, osc2UnisonCount, osc2UnisonDetune);
            voice->setOscUnison (2, osc3UnisonCount, osc3UnisonDetune);

            voice->setOscFilterCutoff (oscCutoff);
            voice->setOscFilterResonance (oscResonance);
            voice->setNoiseFilterCutoff (noiseCutoff);
            voice->setNoiseFilterResonance (noiseResonance);
            voice->setEnvelopeParameters (env1, env2, env3, envNoise);

            voice->setOscLevels (osc1Level, osc2Level, osc3Level);
            voice->setOscDetuneCents (0.0f, osc2Detune, osc3Detune);
            voice->setNoise (noiseType, noiseLevel);
            voice->setSourceEnabled (osc1On, osc2On, osc3On, noiseOn);

            voice->setOscPan (osc1Pan, osc2Pan, osc3Pan);
            voice->setNoisePan (noisePan);
        }

    keyboardState_.processNextMidiBuffer (midiMessages, 0, buffer.getNumSamples(), true);
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
