#pragma once

#include "ui/SynthLookAndFeel.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

// Three-way segmented switch for warp mode: Off | Bend+ | Sync
class WarpModeSwitch final : public juce::Component, private juce::AudioProcessorParameter::Listener
{
public:
    WarpModeSwitch();
    ~WarpModeSwitch() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

    void attach (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID);

    int getSelectedIndex() const;
    std::function<void()> onChange;

private:
    void parameterValueChanged (int parameterIndex, float newValue) override;
    void parameterGestureChanged (int, bool) override {}
    void syncFromParameter();
    void setMode (int index);

    juce::TextButton off_ {"Off"};
    juce::TextButton bend_ {"Bend+"};
    juce::TextButton sync_ {"Sync"};
    juce::AudioParameterChoice* param_ = nullptr;
};
