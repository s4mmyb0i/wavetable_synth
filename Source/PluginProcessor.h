#pragma once

#include "dsp/Wavetable.h"
#include "dsp/WavetableBank.h"
#include "dsp/WavetableMipBank.h"
#include "juce_audio_basics/juce_audio_basics.h"

#include <juce_audio_processors/juce_audio_processors.h>

class WavetableSynthAudioProcessor final : public juce::AudioProcessor
{
public:
    WavetableSynthAudioProcessor();
    ~WavetableSynthAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts_; }
    juce::MidiKeyboardState& getKeyboardState() { return keyboardState_; }

    // 0..1 proportion of the audio callback budget used by processBlock.
    double getCpuLoad() const { return loadMeasurer_.getLoadAsProportion(); }

    const Wavetable& getSineTable() const { return sineTable_; }
    const Wavetable& getTriangleTable() const { return triangleTable_; }
    const WavetableMipBank& getSawMipBank() const { return sawMipBank_; }
    const WavetableMipBank& getSquareMipBank() const { return squareMipBank_; }
    const WavetableBank& getMorphBank() const { return morphBank_; }

private:
    Wavetable sineTable_ = Wavetable::sine();
    Wavetable triangleTable_ = Wavetable::triangle();

    WavetableMipBank sawMipBank_;
    WavetableMipBank squareMipBank_;

    WavetableBank morphBank_ = WavetableBank::makeBasicMorph();

    juce::AudioProcessorValueTreeState apvts_;
    juce::Synthesiser synth_;

    juce::MidiKeyboardState keyboardState_;
    juce::AudioProcessLoadMeasurer loadMeasurer_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavetableSynthAudioProcessor)
};
