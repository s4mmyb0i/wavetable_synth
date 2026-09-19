#pragma once

#include "PluginProcessor.h"

class WavetableSynthAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit WavetableSynthAudioProcessorEditor (WavetableSynthAudioProcessor&);
    ~WavetableSynthAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    WavetableSynthAudioProcessor& processorRef;

    juce::ComboBox wavetypeBox;
    juce::Label wavetypeLabel;
    std::unique_ptr<ComboBoxAttachment> wavetypeAttachment;

    juce::Slider attackSlider;
    juce::Label attackLabel;
    std::unique_ptr<SliderAttachment> attackAttachment;
    
    juce::Slider decaySlider;
    juce::Label decayLabel;
    std::unique_ptr<SliderAttachment> decayAttachment;
    
    juce::Slider sustainSlider;
    juce::Label sustainLabel;
    std::unique_ptr<SliderAttachment> sustainAttachment;
    
    juce::Slider releaseSlider;
    juce::Label releaseLabel;
    std::unique_ptr<SliderAttachment> releaseAttachment;
    
    void setupSlider (juce::Slider& slider, juce::Label& label, const juce::String& name);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavetableSynthAudioProcessorEditor)
};
