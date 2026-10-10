#pragma once

#include "PluginProcessor.h"
#include "juce_audio_utils/juce_audio_utils.h"
#include "ui/EnvelopeEditor.h"
#include "ui/NoiseTypeSwitch.h"
#include "ui/OscWaveformDisplay.h"
#include "ui/SynthLookAndFeel.h"
#include "ui/WaveSelectKnob.h"
#include "ui/WarpModeSwitch.h"

class WavetableSynthAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit WavetableSynthAudioProcessorEditor (WavetableSynthAudioProcessor&);
    ~WavetableSynthAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    void setupRotary (juce::Slider& slider, juce::Label& label, const juce::String& name,
                      const juce::String& unitSuffix = {});
    void layoutKnobColumn (juce::Rectangle<int> column, juce::Slider& slider, juce::Label& label);
    void layoutOscPanel (juce::Rectangle<int> bounds, OscWaveformDisplay& waveform,
                         WaveSelectKnob& waveKnob, WarpModeSwitch& warpSwitch, juce::Slider& level,
                         juce::Label& levelLabel, juce::Slider* detune, juce::Label* detuneLabel,
                         juce::Slider& uniCount, juce::Label& uniCountLabel, juce::Slider& uniDetune,
                         juce::Label& uniDetuneLabel, juce::Slider& warpAmount,
                         juce::Label& warpAmountLabel, juce::Slider& position,
                         juce::Label& positionLabel);
    void paintSection (juce::Graphics& g, juce::Rectangle<int> bounds, const juce::String& title);
    void updatePositionEnabled (juce::Slider& waveSlider, juce::Slider& positionSlider,
                                juce::Label& positionLabel);
    void refreshAllPositionEnabled();

    WavetableSynthAudioProcessor& processorRef;
    SynthLookAndFeel lookAndFeel_;

    juce::Label titleLabel_;

    // Osc 1
    OscWaveformDisplay osc1Waveform_;
    juce::Slider osc1LevelSlider_;
    juce::Label osc1LevelLabel_;
    WaveSelectKnob osc1WaveKnob_;
    WarpModeSwitch osc1WarpSwitch_;
    juce::Slider osc1UnisonCountSlider_, osc1UnisonDetuneSlider_, osc1WarpAmountSlider_;
    juce::Slider osc1PositionSlider_;
    juce::Label osc1UnisonCountLabel_, osc1UnisonDetuneLabel_, osc1WarpAmountLabel_;
    juce::Label osc1PositionLabel_;

    // Osc 2
    OscWaveformDisplay osc2Waveform_;
    juce::Slider osc2LevelSlider_, osc2DetuneSlider_;
    juce::Label osc2LevelLabel_, osc2DetuneLabel_;
    WaveSelectKnob osc2WaveKnob_;
    WarpModeSwitch osc2WarpSwitch_;
    juce::Slider osc2UnisonCountSlider_, osc2UnisonDetuneSlider_, osc2WarpAmountSlider_;
    juce::Slider osc2PositionSlider_;
    juce::Label osc2UnisonCountLabel_, osc2UnisonDetuneLabel_, osc2WarpAmountLabel_;
    juce::Label osc2PositionLabel_;

    // Osc 3
    OscWaveformDisplay osc3Waveform_;
    juce::Slider osc3LevelSlider_, osc3DetuneSlider_;
    juce::Label osc3LevelLabel_, osc3DetuneLabel_;
    WaveSelectKnob osc3WaveKnob_;
    WarpModeSwitch osc3WarpSwitch_;
    juce::Slider osc3UnisonCountSlider_, osc3UnisonDetuneSlider_, osc3WarpAmountSlider_;
    juce::Slider osc3PositionSlider_;
    juce::Label osc3UnisonCountLabel_, osc3UnisonDetuneLabel_, osc3WarpAmountLabel_;
    juce::Label osc3PositionLabel_;

    std::unique_ptr<SliderAttachment> osc1LevelAttachment_;
    std::unique_ptr<SliderAttachment> osc2LevelAttachment_;
    std::unique_ptr<SliderAttachment> osc3LevelAttachment_;
    std::unique_ptr<SliderAttachment> osc2DetuneAttachment_;
    std::unique_ptr<SliderAttachment> osc3DetuneAttachment_;
    std::unique_ptr<SliderAttachment> osc1UnisonCountAttachment_;
    std::unique_ptr<SliderAttachment> osc1UnisonDetuneAttachment_;
    std::unique_ptr<SliderAttachment> osc2UnisonCountAttachment_;
    std::unique_ptr<SliderAttachment> osc2UnisonDetuneAttachment_;
    std::unique_ptr<SliderAttachment> osc3UnisonCountAttachment_;
    std::unique_ptr<SliderAttachment> osc3UnisonDetuneAttachment_;
    std::unique_ptr<SliderAttachment> osc1WarpAmountAttachment_;
    std::unique_ptr<SliderAttachment> osc2WarpAmountAttachment_;
    std::unique_ptr<SliderAttachment> osc3WarpAmountAttachment_;
    std::unique_ptr<SliderAttachment> osc1PositionAttachment_;
    std::unique_ptr<SliderAttachment> osc2PositionAttachment_;
    std::unique_ptr<SliderAttachment> osc3PositionAttachment_;

    // Noise
    NoiseTypeSwitch noiseTypeSwitch_;
    juce::Slider noiseLevelSlider_;
    juce::Label noiseLevelLabel_;
    std::unique_ptr<SliderAttachment> noiseLevelAttachment_;

    // Filter
    juce::Slider cutoffSlider_, resonanceSlider_;
    juce::Label cutoffLabel_, resonanceLabel_;
    std::unique_ptr<SliderAttachment> cutoffAttachment_;
    std::unique_ptr<SliderAttachment> resonanceAttachment_;

    // ADSR
    EnvelopeEditor envelopeEditor_;
    juce::Slider attackSlider_, decaySlider_, sustainSlider_, releaseSlider_;
    juce::Label attackLabel_, decayLabel_, sustainLabel_, releaseLabel_;
    std::unique_ptr<SliderAttachment> attackAttachment_;
    std::unique_ptr<SliderAttachment> decayAttachment_;
    std::unique_ptr<SliderAttachment> sustainAttachment_;
    std::unique_ptr<SliderAttachment> releaseAttachment_;

    juce::Rectangle<int> osc1SectionBounds_;
    juce::Rectangle<int> osc2SectionBounds_;
    juce::Rectangle<int> osc3SectionBounds_;
    juce::Rectangle<int> noiseSectionBounds_;
    juce::Rectangle<int> filterSectionBounds_;
    juce::Rectangle<int> envSectionBounds_;

    juce::MidiKeyboardComponent keyboard_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavetableSynthAudioProcessorEditor)
};
