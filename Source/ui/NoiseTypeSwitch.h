#pragma once

#include "ui/SynthLookAndFeel.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

// Two-way switch for noise color: White | Pink
class NoiseTypeSwitch final : public juce::Component,
                              private juce::AudioProcessorParameter::Listener
{
public:
    NoiseTypeSwitch();
    ~NoiseTypeSwitch() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

    void attach (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID);

private:
    void parameterValueChanged (int parameterIndex, float newValue) override;
    void parameterGestureChanged (int, bool) override {}
    void syncFromParameter();
    void setMode (int index);

    juce::TextButton white_ {"White"};
    juce::TextButton pink_ {"Pink"};
    juce::AudioParameterChoice* param_ = nullptr;
};
