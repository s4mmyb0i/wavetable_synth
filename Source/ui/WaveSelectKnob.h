#pragma once

#include "ui/SynthLookAndFeel.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

// Full-circle discrete wave selector with waveform icons around the ring.
// Drag follows cursor angle and wraps indefinitely in either direction.
class WaveSelectKnob final : public juce::Component
{
public:
    WaveSelectKnob();
    ~WaveSelectKnob() override = default;

    void paint (juce::Graphics& g) override;
    void paintOverChildren (juce::Graphics& g) override;
    void resized() override;

    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseUp (const juce::MouseEvent& e) override;

    juce::Slider& getSlider() { return slider_; }

    void attach (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID);

private:
    void drawWaveIcon (juce::Graphics& g, juce::Rectangle<float> area, int waveIndex,
                       bool selected) const;
    void paintIcons (juce::Graphics& g);
    void setWaveFromMouse (juce::Point<float> pos);

    juce::Slider slider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment_;
    juce::AudioProcessorParameter* param_ = nullptr;
    bool dragging_ = false;

    static constexpr int numWaves = 5;
};
