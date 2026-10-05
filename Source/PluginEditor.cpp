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
constexpr int kGap = 10;
} // namespace

WavetableSynthAudioProcessorEditor::WavetableSynthAudioProcessorEditor (
    WavetableSynthAudioProcessor& p)
    : AudioProcessorEditor (&p),
      processorRef (p),
      keyboard_ (p.getKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard)
{
    setLookAndFeel (&lookAndFeel_);

    titleLabel_.setText ("Wavetable Synth", juce::dontSendNotification);
    titleLabel_.setFont (juce::FontOptions (22.0f, juce::Font::bold));
    titleLabel_.setColour (juce::Label::textColourId, SynthLookAndFeel::textPrimary);
    titleLabel_.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (titleLabel_);

    setupRotary (osc1LevelSlider_, osc1LevelLabel_, "Level");
    setupRotary (osc1UnisonCountSlider_, osc1UnisonCountLabel_, "Unison");
    setupRotary (osc1UnisonDetuneSlider_, osc1UnisonDetuneLabel_, "Spread");

    setupRotary (osc2LevelSlider_, osc2LevelLabel_, "Level");
    setupRotary (osc2DetuneSlider_, osc2DetuneLabel_, "Detune");
    setupRotary (osc2UnisonCountSlider_, osc2UnisonCountLabel_, "Unison");
    setupRotary (osc2UnisonDetuneSlider_, osc2UnisonDetuneLabel_, "Spread");

    setupRotary (osc3LevelSlider_, osc3LevelLabel_, "Level");
    setupRotary (osc3DetuneSlider_, osc3DetuneLabel_, "Detune");
    setupRotary (osc3UnisonCountSlider_, osc3UnisonCountLabel_, "Unison");
    setupRotary (osc3UnisonDetuneSlider_, osc3UnisonDetuneLabel_, "Spread");

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

    osc1UnisonCountAttachment_ = std::make_unique<SliderAttachment> (
        apvts, ParamIDs::osc1UnisonCount, osc1UnisonCountSlider_);
    osc1UnisonDetuneAttachment_ = std::make_unique<SliderAttachment> (
        apvts, ParamIDs::osc1UnisonDetune, osc1UnisonDetuneSlider_);
    osc2UnisonCountAttachment_ = std::make_unique<SliderAttachment> (
        apvts, ParamIDs::osc2UnisonCount, osc2UnisonCountSlider_);
    osc2UnisonDetuneAttachment_ = std::make_unique<SliderAttachment> (
        apvts, ParamIDs::osc2UnisonDetune, osc2UnisonDetuneSlider_);
    osc3UnisonCountAttachment_ = std::make_unique<SliderAttachment> (
        apvts, ParamIDs::osc3UnisonCount, osc3UnisonCountSlider_);
    osc3UnisonDetuneAttachment_ = std::make_unique<SliderAttachment> (
        apvts, ParamIDs::osc3UnisonDetune, osc3UnisonDetuneSlider_);

    cutoffAttachment_ = std::make_unique<SliderAttachment> (apvts, ParamIDs::cutoff, cutoffSlider_);
    resonanceAttachment_ =
        std::make_unique<SliderAttachment> (apvts, ParamIDs::resonance, resonanceSlider_);

    attackAttachment_ = std::make_unique<SliderAttachment> (apvts, ParamIDs::attack, attackSlider_);
    decayAttachment_ = std::make_unique<SliderAttachment> (apvts, ParamIDs::decay, decaySlider_);
    sustainAttachment_ =
        std::make_unique<SliderAttachment> (apvts, ParamIDs::sustain, sustainSlider_);
    releaseAttachment_ =
        std::make_unique<SliderAttachment> (apvts, ParamIDs::release, releaseSlider_);

    keyboard_.setAvailableRange (36, 96); // C2–C7
    keyboard_.setOctaveForMiddleC (4);
    keyboard_.setScrollButtonsVisible (false);
    keyboard_.setColour (juce::MidiKeyboardComponent::whiteNoteColourId,
                         juce::Colour (0xff2a3036));
    keyboard_.setColour (juce::MidiKeyboardComponent::blackNoteColourId,
                         juce::Colour (0xff141618));
    keyboard_.setColour (juce::MidiKeyboardComponent::keySeparatorLineColourId,
                         SynthLookAndFeel::panelBorder);
    keyboard_.setColour (juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId,
                         SynthLookAndFeel::accent.withAlpha (0.25f));
    keyboard_.setColour (juce::MidiKeyboardComponent::keyDownOverlayColourId,
                         SynthLookAndFeel::accent.withAlpha (0.55f));
    keyboard_.setColour (juce::MidiKeyboardComponent::textLabelColourId,
                         SynthLookAndFeel::textMuted);
    addAndMakeVisible (keyboard_);

    setSize (960, 640);
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

void WavetableSynthAudioProcessorEditor::layoutOscPanel (
    juce::Rectangle<int> bounds, juce::ComboBox& waveBox, juce::Slider& level,
    juce::Label& levelLabel, juce::Slider* detune, juce::Label* detuneLabel, juce::Slider& uniCount,
    juce::Label& uniCountLabel, juce::Slider& uniDetune, juce::Label& uniDetuneLabel)
{
    auto inner = bounds.reduced (10);
    inner.removeFromTop (kSectionTitleH);

    auto waveRow = inner.removeFromTop (kWaveBoxH + 6);
    const int waveW = juce::jmin (waveRow.getWidth() - 8, 140);
    waveBox.setBounds (waveRow.withSizeKeepingCentre (waveW, kWaveBoxH));

    // 2×2 knob grid. Osc 1 has no pitch detune — leave that cell empty for alignment.
    auto topRow = inner.removeFromTop (inner.getHeight() / 2);
    auto bottomRow = inner;
    const int halfW = topRow.getWidth() / 2;

    layoutKnobColumn (topRow.removeFromLeft (halfW), level, levelLabel);
    if (detune != nullptr && detuneLabel != nullptr)
        layoutKnobColumn (topRow, *detune, *detuneLabel);

    layoutKnobColumn (bottomRow.removeFromLeft (halfW), uniCount, uniCountLabel);
    layoutKnobColumn (bottomRow, uniDetune, uniDetuneLabel);
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

    paintSection (g, osc1SectionBounds_, "OSC 1");
    paintSection (g, osc2SectionBounds_, "OSC 2");
    paintSection (g, osc3SectionBounds_, "OSC 3");
    paintSection (g, filterSectionBounds_, "FILTER");
    paintSection (g, envSectionBounds_, "ENVELOPE");
}

void WavetableSynthAudioProcessorEditor::resized()
{
    auto full = getLocalBounds();

    constexpr int kKeyboardH = 88;
    auto keyboardArea = full.removeFromBottom (kKeyboardH);
    keyboard_.setBounds (keyboardArea);

    const float totalW = keyboard_.getTotalKeyboardWidth();
    if (totalW > 1.0f)
        keyboard_.setKeyWidth (keyboard_.getKeyWidth() * static_cast<float> (keyboardArea.getWidth())
                               / totalW);

    auto bounds = full.reduced (kPad);
    bounds.removeFromBottom (12);

    auto header = bounds.removeFromTop (kHeaderH);
    titleLabel_.setBounds (header);

    bounds.removeFromTop (8);

    auto bottom = bounds.removeFromBottom (bounds.getHeight() / 2 - 6);
    bounds.removeFromBottom (kGap);
    auto top = bounds;

    // Three separate oscillator panels across the top.
    {
        const int panelW = (top.getWidth() - 2 * kGap) / 3;
        osc1SectionBounds_ = top.removeFromLeft (panelW);
        top.removeFromLeft (kGap);
        osc2SectionBounds_ = top.removeFromLeft (panelW);
        top.removeFromLeft (kGap);
        osc3SectionBounds_ = top;
    }

    filterSectionBounds_ =
        bottom.removeFromLeft (juce::roundToInt (static_cast<float> (bottom.getWidth()) * 0.34f));
    bottom.removeFromLeft (kGap);
    envSectionBounds_ = bottom;

    layoutOscPanel (osc1SectionBounds_, osc1WaveBox_, osc1LevelSlider_, osc1LevelLabel_, nullptr,
                    nullptr, osc1UnisonCountSlider_, osc1UnisonCountLabel_, osc1UnisonDetuneSlider_,
                    osc1UnisonDetuneLabel_);
    layoutOscPanel (osc2SectionBounds_, osc2WaveBox_, osc2LevelSlider_, osc2LevelLabel_,
                    &osc2DetuneSlider_, &osc2DetuneLabel_, osc2UnisonCountSlider_,
                    osc2UnisonCountLabel_, osc2UnisonDetuneSlider_, osc2UnisonDetuneLabel_);
    layoutOscPanel (osc3SectionBounds_, osc3WaveBox_, osc3LevelSlider_, osc3LevelLabel_,
                    &osc3DetuneSlider_, &osc3DetuneLabel_, osc3UnisonCountSlider_,
                    osc3UnisonCountLabel_, osc3UnisonDetuneSlider_, osc3UnisonDetuneLabel_);

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
