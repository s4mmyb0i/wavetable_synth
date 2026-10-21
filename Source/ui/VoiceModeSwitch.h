#pragma once

#include "ui/SynthLookAndFeel.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

// Segmented switch: Poly | Mono | Legato (indices match VoiceMode).
class VoiceModeSwitch final : public juce::Component, private juce::AudioProcessorParameter::Listener
{
public:
    VoiceModeSwitch();
    ~VoiceModeSwitch() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

    void attach (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID);

    int getSelectedIndex() const;

private:
    void parameterValueChanged (int parameterIndex, float newValue) override;
    void parameterGestureChanged (int, bool) override {}
    void syncFromParameter();
    void setMode (int index);

    juce::TextButton poly_ {"Poly"};
    juce::TextButton mono_ {"Mono"};
    juce::TextButton legato_ {"Legato"};
    juce::AudioParameterChoice* param_ = nullptr;
};
