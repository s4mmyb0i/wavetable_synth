#pragma once

#include "ui/SynthLookAndFeel.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

// Segmented switch: Low Pass | High Pass | Band Pass (matches FilterType).
class FilterTypeSwitch final : public juce::Component, private juce::AudioProcessorParameter::Listener
{
public:
    FilterTypeSwitch();
    ~FilterTypeSwitch() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

    void attach (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID);

    int getSelectedIndex() const;

private:
    void parameterValueChanged (int parameterIndex, float newValue) override;
    void parameterGestureChanged (int, bool) override {}
    void syncFromParameter();
    void setMode (int index);

    juce::TextButton lowPass_ {"LP"};
    juce::TextButton highPass_ {"HP"};
    juce::TextButton bandPass_ {"BP"};
    juce::AudioParameterChoice* param_ = nullptr;
};
