#include "PluginEditor.h"
#include "params/ParamIDs.h"

namespace
{
constexpr int kPad = 16;
constexpr int kHeaderH = 44;
constexpr int kSectionTitleH = 22;
constexpr int kLabelH = 18;
constexpr int kTextBoxH = 18;
constexpr int kWaveBoxH = 26;
} // namespace

WavetableSynthAudioProcessorEditor::WavetableSynthAudioProcessorEditor (
    WavetableSynthAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    setLookAndFeel (&lookAndFeel_);

    titleLabel_.setText ("Wavetable Synth", juce::dontSendNotification);
    titleLabel_.setFont (juce::FontOptions (22.0f, juce::Font::bold));
    titleLabel_.setColour (juce::Label::textColourId, SynthLookAndFeel::textPrimary);
    titleLabel_.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (titleLabel_);

    setupRotary (osc1LevelSlider_, osc1LevelLabel_, "Osc 1");
    setupRotary (osc2LevelSlider_, osc2LevelLabel_, "Osc 2");
    setupRotary (osc2DetuneSlider_, osc2DetuneLabel_, "Detune 2");
    setupRotary (osc3LevelSlider_, osc3LevelLabel_, "Osc 3");
    setupRotary (osc3DetuneSlider_, osc3DetuneLabel_, "Detune 3");

    setupWaveBox (osc1WaveBox_);
    setupWaveBox (osc2WaveBox_);
    setupWaveBox (osc3WaveBox_);

    setupRotary (cutoffSlider_, cutoffLabel_, "Cutoff");
    setupRotary (resonanceSlider_, resonanceLabel_, "Resonance");

    setupVertical (attackSlider_, attackLabel_, "A");
    setupVertical (decaySlider_, decayLabel_, "D");
    setupVertical (sustainSlider_, sustainLabel_, "S");
    setupVertical (releaseSlider_, releaseLabel_, "R");

    auto& apvts = processorRef.getAPVTS();

    osc1WaveAttachment_ =
        std::make_unique<ComboBoxAttachment> (apvts, ParamIDs::osc1Wave, osc1WaveBox_);
    osc2WaveAttachment_ =
        std::make_unique<ComboBoxAttachment> (apvts, ParamIDs::osc2Wave, osc2WaveBox_);
    osc3WaveAttachment_ =
        std::make_unique<ComboBoxAttachment> (apvts, ParamIDs::osc3Wave, osc3WaveBox_);

    osc1LevelAttachment_ =
        std::make_unique<SliderAttachment> (apvts, ParamIDs::osc1Level, osc1LevelSlider_);
    osc2LevelAttachment_ =
        std::make_unique<SliderAttachment> (apvts, ParamIDs::osc2Level, osc2LevelSlider_);
    osc3LevelAttachment_ =
        std::make_unique<SliderAttachment> (apvts, ParamIDs::osc3Level, osc3LevelSlider_);
    osc2DetuneAttachment_ =
        std::make_unique<SliderAttachment> (apvts, ParamIDs::osc2Detune, osc2DetuneSlider_);
    osc3DetuneAttachment_ =
        std::make_unique<SliderAttachment> (apvts, ParamIDs::osc3Detune, osc3DetuneSlider_);

    cutoffAttachment_ = std::make_unique<SliderAttachment> (apvts, ParamIDs::cutoff, cutoffSlider_);
    resonanceAttachment_ =
        std::make_unique<SliderAttachment> (apvts, ParamIDs::resonance, resonanceSlider_);

    attackAttachment_ = std::make_unique<SliderAttachment> (apvts, ParamIDs::attack, attackSlider_);
    decayAttachment_ = std::make_unique<SliderAttachment> (apvts, ParamIDs::decay, decaySlider_);
    sustainAttachment_ =
        std::make_unique<SliderAttachment> (apvts, ParamIDs::sustain, sustainSlider_);
    releaseAttachment_ =
        std::make_unique<SliderAttachment> (apvts, ParamIDs::release, releaseSlider_);

    setSize (760, 460);
    setResizable (false, false);
}

WavetableSynthAudioProcessorEditor::~WavetableSynthAudioProcessorEditor()
{ setLookAndFeel (nullptr); }

void WavetableSynthAudioProcessorEditor::setupRotary (juce::Slider& slider, juce::Label& label,
                                                      const juce::String& name)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 64, kTextBoxH);
    addAndMakeVisible (slider);

    label.setText (name, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setColour (juce::Label::textColourId, SynthLookAndFeel::textMuted);
    addAndMakeVisible (label);
}

void WavetableSynthAudioProcessorEditor::setupVertical (juce::Slider& slider, juce::Label& label,
                                                        const juce::String& name)
{
    slider.setSliderStyle (juce::Slider::LinearVertical);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 52, kTextBoxH);
    addAndMakeVisible (slider);

    label.setText (name, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setColour (juce::Label::textColourId, SynthLookAndFeel::textMuted);
    addAndMakeVisible (label);
}

void WavetableSynthAudioProcessorEditor::setupWaveBox (juce::ComboBox& box)
{
    box.addItemList (juce::StringArray {"Sine", "Saw", "Square", "Triangle"}, 1);
    addAndMakeVisible (box);
}

void WavetableSynthAudioProcessorEditor::layoutKnobColumn (juce::Rectangle<int> column,
                                                           juce::Slider& slider, juce::Label& label)
{
    label.setBounds (column.removeFromTop (kLabelH));
    slider.setBounds (column.reduced (4, 2));
}

void WavetableSynthAudioProcessorEditor::layoutVerticalColumn (juce::Rectangle<int> column,
                                                               juce::Slider& slider,
                                                               juce::Label& label)
{
    label.setBounds (column.removeFromTop (kLabelH));
    slider.setBounds (column.reduced (column.getWidth() / 4, 2));
}

void WavetableSynthAudioProcessorEditor::paintSection (juce::Graphics& g,
                                                       juce::Rectangle<int> bounds,
                                                       const juce::String& title)
{
    g.setColour (SynthLookAndFeel::panel);
    g.fillRoundedRectangle (bounds.toFloat(), 10.0f);
    g.setColour (SynthLookAndFeel::panelBorder);
    g.drawRoundedRectangle (bounds.toFloat().reduced (0.5f), 10.0f, 1.0f);

    g.setColour (SynthLookAndFeel::accent);
    g.setFont (juce::FontOptions (12.0f, juce::Font::bold));
    g.drawText (title, bounds.removeFromTop (kSectionTitleH).reduced (12, 2),
                juce::Justification::centredLeft, false);
}

void WavetableSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (SynthLookAndFeel::background);

    g.setColour (SynthLookAndFeel::accent.withAlpha (0.35f));
    g.fillRect (0, 0, getWidth(), 2);

    paintSection (g, oscSectionBounds_, "OSCILLATORS");
    paintSection (g, filterSectionBounds_, "FILTER");
    paintSection (g, envSectionBounds_, "ENVELOPE");
}

void WavetableSynthAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced (kPad);

    auto header = bounds.removeFromTop (kHeaderH);
    titleLabel_.setBounds (header);

    bounds.removeFromTop (8);

    auto bottom = bounds.removeFromBottom (bounds.getHeight() / 2 - 6);
    bounds.removeFromBottom (12);
    auto top = bounds;

    oscSectionBounds_ = top;
    filterSectionBounds_ =
        bottom.removeFromLeft (juce::roundToInt (static_cast<float> (bottom.getWidth()) * 0.34f));
    bottom.removeFromLeft (12);
    envSectionBounds_ = bottom;

    // Oscillators: knobs on top row, wave combos under Osc 1 / 2 / 3
    {
        auto inner = oscSectionBounds_.reduced (10);
        inner.removeFromTop (kSectionTitleH);

        auto waveRow = inner.removeFromBottom (kWaveBoxH + 4);
        const int n = 5;
        const int colW = inner.getWidth() / n;

        auto c1 = inner.removeFromLeft (colW);
        auto c2 = inner.removeFromLeft (colW);
        auto c3 = inner.removeFromLeft (colW); // detune 2
        auto c4 = inner.removeFromLeft (colW);
        auto c5 = inner; // detune 3

        layoutKnobColumn (c1, osc1LevelSlider_, osc1LevelLabel_);
        layoutKnobColumn (c2, osc2LevelSlider_, osc2LevelLabel_);
        layoutKnobColumn (c3, osc2DetuneSlider_, osc2DetuneLabel_);
        layoutKnobColumn (c4, osc3LevelSlider_, osc3LevelLabel_);
        layoutKnobColumn (c5, osc3DetuneSlider_, osc3DetuneLabel_);

        // Wave boxes under the three level columns (skip detune columns).
        const int waveW = juce::jmin (colW - 8, 110);
        osc1WaveBox_.setBounds (
            waveRow.removeFromLeft (colW).withSizeKeepingCentre (waveW, kWaveBoxH));
        osc2WaveBox_.setBounds (
            waveRow.removeFromLeft (colW).withSizeKeepingCentre (waveW, kWaveBoxH));
        waveRow.removeFromLeft (colW); // under detune 2
        osc3WaveBox_.setBounds (
            waveRow.removeFromLeft (colW).withSizeKeepingCentre (waveW, kWaveBoxH));
    }

    {
        auto inner = filterSectionBounds_.reduced (10);
        inner.removeFromTop (kSectionTitleH);
        const int colW = inner.getWidth() / 2;
        layoutKnobColumn (inner.removeFromLeft (colW), cutoffSlider_, cutoffLabel_);
        layoutKnobColumn (inner, resonanceSlider_, resonanceLabel_);
    }

    {
        auto inner = envSectionBounds_.reduced (10);
        inner.removeFromTop (kSectionTitleH);
        const int colW = inner.getWidth() / 4;
        layoutVerticalColumn (inner.removeFromLeft (colW), attackSlider_, attackLabel_);
        layoutVerticalColumn (inner.removeFromLeft (colW), decaySlider_, decayLabel_);
        layoutVerticalColumn (inner.removeFromLeft (colW), sustainSlider_, sustainLabel_);
        layoutVerticalColumn (inner, releaseSlider_, releaseLabel_);
    }
}
