#pragma once

#include "ui/SynthLookAndFeel.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

// Clickable power LED for an osc / noise source. Lit when enabled; brighter when level > 0.
class SourceEnableLed final : public juce::Component, private juce::AudioProcessorParameter::Listener
{
public:
    SourceEnableLed();
    ~SourceEnableLed() override;

    void attach (juce::AudioProcessorValueTreeState& apvts, const juce::String& enableID,
                 const juce::String& levelID);

    bool isSourceOn() const { return enabled_; }

    std::function<void()> onChange;

    void paint (juce::Graphics& g) override;
    void mouseDown (const juce::MouseEvent& e) override;
    void mouseEnter (const juce::MouseEvent&) override;
    void mouseExit (const juce::MouseEvent&) override;

private:
    void parameterValueChanged (int parameterIndex, float newValue) override;
    void parameterGestureChanged (int, bool) override {}
    void syncFromParameters();
    void toggle();

    juce::AudioParameterBool* enable_ = nullptr;
    juce::AudioParameterFloat* level_ = nullptr;
    bool enabled_ = true;
    bool active_ = false; // enabled && level > 0
    bool hover_ = false;
};
