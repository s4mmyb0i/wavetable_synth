#include "PluginEditor.h"
#include "params/ParamIDs.h"

namespace
{
constexpr int kPad = 16;
constexpr int kHeaderH = 44;
constexpr int kSectionTitleH = 22;
constexpr int kLabelH = 18;
constexpr int kTextBoxH = 18;
constexpr int kSwitchH = 26;
constexpr int kGap = 10;

// Must match ParamIDs / ParameterLayout waveChoices order.
constexpr int kMorphWaveIndex = 4;
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
    setupRotary (osc1WarpAmountSlider_, osc1WarpAmountLabel_, "Warp");
    setupRotary (osc1PositionSlider_, osc1PositionLabel_, "Pos");

    setupRotary (osc2LevelSlider_, osc2LevelLabel_, "Level");
    setupRotary (osc2DetuneSlider_, osc2DetuneLabel_, "Detune");
    setupRotary (osc2UnisonCountSlider_, osc2UnisonCountLabel_, "Unison");
    setupRotary (osc2UnisonDetuneSlider_, osc2UnisonDetuneLabel_, "Spread");
    setupRotary (osc2WarpAmountSlider_, osc2WarpAmountLabel_, "Warp");
    setupRotary (osc2PositionSlider_, osc2PositionLabel_, "Pos");

    setupRotary (osc3LevelSlider_, osc3LevelLabel_, "Level");
    setupRotary (osc3DetuneSlider_, osc3DetuneLabel_, "Detune");
    setupRotary (osc3UnisonCountSlider_, osc3UnisonCountLabel_, "Unison");
    setupRotary (osc3UnisonDetuneSlider_, osc3UnisonDetuneLabel_, "Spread");
    setupRotary (osc3WarpAmountSlider_, osc3WarpAmountLabel_, "Warp");
    setupRotary (osc3PositionSlider_, osc3PositionLabel_, "Pos");

    addAndMakeVisible (osc1WaveKnob_);
    addAndMakeVisible (osc2WaveKnob_);
    addAndMakeVisible (osc3WaveKnob_);
    addAndMakeVisible (osc1WarpSwitch_);
    addAndMakeVisible (osc2WarpSwitch_);
    addAndMakeVisible (osc3WarpSwitch_);

    setupRotary (cutoffSlider_, cutoffLabel_, "Cutoff");
    setupRotary (resonanceSlider_, resonanceLabel_, "Resonance");

    setupVertical (attackSlider_, attackLabel_, "A");
    setupVertical (decaySlider_, decayLabel_, "D");
    setupVertical (sustainSlider_, sustainLabel_, "S");
    setupVertical (releaseSlider_, releaseLabel_, "R");

    auto& apvts = processorRef.getAPVTS();

    osc1WaveKnob_.attach (apvts, ParamIDs::osc1Wave);
    osc2WaveKnob_.attach (apvts, ParamIDs::osc2Wave);
    osc3WaveKnob_.attach (apvts, ParamIDs::osc3Wave);

    osc1WarpSwitch_.attach (apvts, ParamIDs::osc1WarpMode);
    osc2WarpSwitch_.attach (apvts, ParamIDs::osc2WarpMode);
    osc3WarpSwitch_.attach (apvts, ParamIDs::osc3WarpMode);

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

    osc1WarpAmountAttachment_ = std::make_unique<SliderAttachment> (
        apvts, ParamIDs::osc1WarpAmount, osc1WarpAmountSlider_);
    osc2WarpAmountAttachment_ = std::make_unique<SliderAttachment> (
        apvts, ParamIDs::osc2WarpAmount, osc2WarpAmountSlider_);
    osc3WarpAmountAttachment_ = std::make_unique<SliderAttachment> (
        apvts, ParamIDs::osc3WarpAmount, osc3WarpAmountSlider_);

    osc1PositionAttachment_ = std::make_unique<SliderAttachment> (
        apvts, ParamIDs::osc1Position, osc1PositionSlider_);
    osc2PositionAttachment_ = std::make_unique<SliderAttachment> (
        apvts, ParamIDs::osc2Position, osc2PositionSlider_);
    osc3PositionAttachment_ = std::make_unique<SliderAttachment> (
        apvts, ParamIDs::osc3Position, osc3PositionSlider_);

    cutoffAttachment_ = std::make_unique<SliderAttachment> (apvts, ParamIDs::cutoff, cutoffSlider_);
    resonanceAttachment_ =
        std::make_unique<SliderAttachment> (apvts, ParamIDs::resonance, resonanceSlider_);

    attackAttachment_ = std::make_unique<SliderAttachment> (apvts, ParamIDs::attack, attackSlider_);
    decayAttachment_ = std::make_unique<SliderAttachment> (apvts, ParamIDs::decay, decaySlider_);
    sustainAttachment_ =
        std::make_unique<SliderAttachment> (apvts, ParamIDs::sustain, sustainSlider_);
    releaseAttachment_ =
        std::make_unique<SliderAttachment> (apvts, ParamIDs::release, releaseSlider_);

    osc1WaveKnob_.getSlider().onValueChange = [this]
    {
        osc1WaveKnob_.repaint();
        updatePositionEnabled (osc1WaveKnob_.getSlider(), osc1PositionSlider_, osc1PositionLabel_);
    };
    osc2WaveKnob_.getSlider().onValueChange = [this]
    {
        osc2WaveKnob_.repaint();
        updatePositionEnabled (osc2WaveKnob_.getSlider(), osc2PositionSlider_, osc2PositionLabel_);
    };
    osc3WaveKnob_.getSlider().onValueChange = [this]
    {
        osc3WaveKnob_.repaint();
        updatePositionEnabled (osc3WaveKnob_.getSlider(), osc3PositionSlider_, osc3PositionLabel_);
    };
    refreshAllPositionEnabled();

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

    setSize (980, 680);
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

void WavetableSynthAudioProcessorEditor::updatePositionEnabled (juce::Slider& waveSlider,
                                                                juce::Slider& positionSlider,
                                                                juce::Label& positionLabel)
{
    const bool morph = juce::roundToInt (waveSlider.getValue()) == kMorphWaveIndex;
    positionSlider.setEnabled (morph);
    const float alpha = morph ? 1.0f : 0.35f;
    positionSlider.setAlpha (alpha);
    positionLabel.setAlpha (alpha);
}

void WavetableSynthAudioProcessorEditor::refreshAllPositionEnabled()
{
    updatePositionEnabled (osc1WaveKnob_.getSlider(), osc1PositionSlider_, osc1PositionLabel_);
    updatePositionEnabled (osc2WaveKnob_.getSlider(), osc2PositionSlider_, osc2PositionLabel_);
    updatePositionEnabled (osc3WaveKnob_.getSlider(), osc3PositionSlider_, osc3PositionLabel_);
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
    juce::Rectangle<int> bounds, WaveSelectKnob& waveKnob, WarpModeSwitch& warpSwitch,
    juce::Slider& level, juce::Label& levelLabel, juce::Slider* detune, juce::Label* detuneLabel,
    juce::Slider& uniCount, juce::Label& uniCountLabel, juce::Slider& uniDetune,
    juce::Label& uniDetuneLabel, juce::Slider& warpAmount, juce::Label& warpAmountLabel,
    juce::Slider& position, juce::Label& positionLabel)
{
    auto inner = bounds.reduced (10);
    inner.removeFromTop (kSectionTitleH);

    auto warpRow = inner.removeFromTop (kSwitchH + 6);
    const int warpW = juce::jmin (warpRow.getWidth() - 8, 160);
    warpSwitch.setBounds (warpRow.withSizeKeepingCentre (warpW, kSwitchH));

    // Wave knob gets a large square cell on the left; remaining knobs stack in two rows.
    const int waveSize = juce::jmin (inner.getHeight(), juce::roundToInt (inner.getWidth() * 0.42f));
    auto waveArea = inner.removeFromLeft (waveSize);
    inner.removeFromLeft (6);
    waveKnob.setBounds (waveArea.withSizeKeepingCentre (waveSize, waveSize));

    auto topRow = inner.removeFromTop (inner.getHeight() / 2);
    auto bottomRow = inner;

    const int topCols = (detune != nullptr) ? 3 : 2;
    const int topW = topRow.getWidth() / topCols;

    layoutKnobColumn (topRow.removeFromLeft (topW), level, levelLabel);
    if (detune != nullptr && detuneLabel != nullptr)
        layoutKnobColumn (topRow.removeFromLeft (topW), *detune, *detuneLabel);
    layoutKnobColumn (topRow, warpAmount, warpAmountLabel);

    const int botW = bottomRow.getWidth() / 3;
    layoutKnobColumn (bottomRow.removeFromLeft (botW), uniCount, uniCountLabel);
    layoutKnobColumn (bottomRow.removeFromLeft (botW), uniDetune, uniDetuneLabel);
    layoutKnobColumn (bottomRow, position, positionLabel);
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

    layoutOscPanel (osc1SectionBounds_, osc1WaveKnob_, osc1WarpSwitch_, osc1LevelSlider_,
                    osc1LevelLabel_, nullptr, nullptr, osc1UnisonCountSlider_,
                    osc1UnisonCountLabel_, osc1UnisonDetuneSlider_, osc1UnisonDetuneLabel_,
                    osc1WarpAmountSlider_, osc1WarpAmountLabel_, osc1PositionSlider_,
                    osc1PositionLabel_);
    layoutOscPanel (osc2SectionBounds_, osc2WaveKnob_, osc2WarpSwitch_, osc2LevelSlider_,
                    osc2LevelLabel_, &osc2DetuneSlider_, &osc2DetuneLabel_, osc2UnisonCountSlider_,
                    osc2UnisonCountLabel_, osc2UnisonDetuneSlider_, osc2UnisonDetuneLabel_,
                    osc2WarpAmountSlider_, osc2WarpAmountLabel_, osc2PositionSlider_,
                    osc2PositionLabel_);
    layoutOscPanel (osc3SectionBounds_, osc3WaveKnob_, osc3WarpSwitch_, osc3LevelSlider_,
                    osc3LevelLabel_, &osc3DetuneSlider_, &osc3DetuneLabel_, osc3UnisonCountSlider_,
                    osc3UnisonCountLabel_, osc3UnisonDetuneSlider_, osc3UnisonDetuneLabel_,
                    osc3WarpAmountSlider_, osc3WarpAmountLabel_, osc3PositionSlider_,
                    osc3PositionLabel_);

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
