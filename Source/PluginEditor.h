#pragma once

#include "PluginProcessor.h"
#include "ui/SynthLookAndFeel.h"

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

    void setupRotary (juce::Slider& slider, juce::Label& label, const juce::String& name);
    void setupVertical (juce::Slider& slider, juce::Label& label, const juce::String& name);
    void setupWaveBox (juce::ComboBox& box);
    void layoutKnobColumn (juce::Rectangle<int> column, juce::Slider& slider, juce::Label& label);
    void layoutVerticalColumn (juce::Rectangle<int> column, juce::Slider& slider,
                               juce::Label& label);
    void paintSection (juce::Graphics& g, juce::Rectangle<int> bounds, const juce::String& title);

    WavetableSynthAudioProcessor& processorRef;
    SynthLookAndFeel lookAndFeel_;

    juce::Label titleLabel_;

    // Oscillators
    juce::Slider osc1LevelSlider_, osc2LevelSlider_, osc3LevelSlider_;
    juce::Label osc1LevelLabel_, osc2LevelLabel_, osc3LevelLabel_;
    juce::Slider osc2DetuneSlider_, osc3DetuneSlider_;
    juce::Label osc2DetuneLabel_, osc3DetuneLabel_;

    juce::ComboBox osc1WaveBox_, osc2WaveBox_, osc3WaveBox_;
    std::unique_ptr<ComboBoxAttachment> osc1WaveAttachment_;
    std::unique_ptr<ComboBoxAttachment> osc2WaveAttachment_;
    std::unique_ptr<ComboBoxAttachment> osc3WaveAttachment_;

    std::unique_ptr<SliderAttachment> osc1LevelAttachment_;
    std::unique_ptr<SliderAttachment> osc2LevelAttachment_;
    std::unique_ptr<SliderAttachment> osc3LevelAttachment_;
    std::unique_ptr<SliderAttachment> osc2DetuneAttachment_;
    std::unique_ptr<SliderAttachment> osc3DetuneAttachment_;

    // Filter
    juce::Slider cutoffSlider_, resonanceSlider_;
    juce::Label cutoffLabel_, resonanceLabel_;
    std::unique_ptr<SliderAttachment> cutoffAttachment_;
    std::unique_ptr<SliderAttachment> resonanceAttachment_;

    // ADSR
    juce::Slider attackSlider_, decaySlider_, sustainSlider_, releaseSlider_;
    juce::Label attackLabel_, decayLabel_, sustainLabel_, releaseLabel_;
    std::unique_ptr<SliderAttachment> attackAttachment_;
    std::unique_ptr<SliderAttachment> decayAttachment_;
    std::unique_ptr<SliderAttachment> sustainAttachment_;
    std::unique_ptr<SliderAttachment> releaseAttachment_;

    juce::Rectangle<int> oscSectionBounds_;
    juce::Rectangle<int> filterSectionBounds_;
    juce::Rectangle<int> envSectionBounds_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavetableSynthAudioProcessorEditor)
};
