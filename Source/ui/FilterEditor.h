#pragma once

#include "ui/FilterResponse.h"
#include "ui/SynthLookAndFeel.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

// Interactive filter frequency-response plot. Cutoff ↔ X, resonance ↔ peak height.
class FilterEditor final : public juce::Component, private juce::AudioProcessorParameter::Listener
{
public:
    FilterEditor();
    ~FilterEditor() override;

    void attach (juce::AudioProcessorValueTreeState& apvts, const juce::String& cutoffID,
                 const juce::String& resonanceID);

    void paint (juce::Graphics& g) override;
    void resized() override;

    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseUp (const juce::MouseEvent& e) override;
    void mouseMove (const juce::MouseEvent& e) override;
    void mouseExit (const juce::MouseEvent& e) override;

private:
    void parameterValueChanged (int parameterIndex, float newValue) override;
    void parameterGestureChanged (int, bool) override {}

    juce::Rectangle<float> plotBounds() const;
    bool hitTestHandle (juce::Point<float> pos) const;
    void applyDrag (juce::Point<float> pos);
    void setParamValue (juce::AudioParameterFloat* param, float denormalised);
    void syncCursor (bool overHandle);

    juce::AudioParameterFloat* cutoff_ = nullptr;
    juce::AudioParameterFloat* resonance_ = nullptr;

    bool dragging_ = false;
    bool hover_ = false;
    bool gestureOpen_ = false;

    static constexpr float handleRadius = 5.0f;
    static constexpr float grabRadius = 12.0f;
};
