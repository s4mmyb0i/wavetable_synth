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
    setupRotary (osc1UnisonDetuneSlider_, osc1UnisonDetuneLabel_, "Spread", " ct");
    setupRotary (osc1WarpAmountSlider_, osc1WarpAmountLabel_, "Warp");
    setupRotary (osc1PositionSlider_, osc1PositionLabel_, "Pos");
    setupRotary (osc1PanSlider_, osc1PanLabel_, "Pan");

    setupRotary (osc2LevelSlider_, osc2LevelLabel_, "Level");
    setupRotary (osc2DetuneSlider_, osc2DetuneLabel_, "Detune", " ct");
    setupRotary (osc2UnisonCountSlider_, osc2UnisonCountLabel_, "Unison");
    setupRotary (osc2UnisonDetuneSlider_, osc2UnisonDetuneLabel_, "Spread", " ct");
    setupRotary (osc2WarpAmountSlider_, osc2WarpAmountLabel_, "Warp");
    setupRotary (osc2PositionSlider_, osc2PositionLabel_, "Pos");
    setupRotary (osc2PanSlider_, osc2PanLabel_, "Pan");

    setupRotary (osc3LevelSlider_, osc3LevelLabel_, "Level");
    setupRotary (osc3DetuneSlider_, osc3DetuneLabel_, "Detune", " ct");
    setupRotary (osc3UnisonCountSlider_, osc3UnisonCountLabel_, "Unison");
    setupRotary (osc3UnisonDetuneSlider_, osc3UnisonDetuneLabel_, "Spread", " ct");
    setupRotary (osc3WarpAmountSlider_, osc3WarpAmountLabel_, "Warp");
    setupRotary (osc3PositionSlider_, osc3PositionLabel_, "Pos");
    setupRotary (osc3PanSlider_, osc3PanLabel_, "Pan");

    addAndMakeVisible (osc1Waveform_);
    addAndMakeVisible (osc2Waveform_);
    addAndMakeVisible (osc3Waveform_);
    addAndMakeVisible (osc1WaveKnob_);
    addAndMakeVisible (osc2WaveKnob_);
    addAndMakeVisible (osc3WaveKnob_);
    addAndMakeVisible (osc1WarpSwitch_);
    addAndMakeVisible (osc2WarpSwitch_);
    addAndMakeVisible (osc3WarpSwitch_);

    setupRotary (noiseLevelSlider_, noiseLevelLabel_, "Level");
    setupRotary (noisePanSlider_, noisePanLabel_, "Pan");
    addAndMakeVisible (noiseTypeSwitch_);

    setupRotary (cutoffSlider_, cutoffLabel_, "Cutoff", " Hz");
    setupRotary (resonanceSlider_, resonanceLabel_, "Resonance");

    setupRotary (attackSlider_, attackLabel_, "A", " s");
    setupRotary (decaySlider_, decayLabel_, "D", " s");
    setupRotary (sustainSlider_, sustainLabel_, "S");
    setupRotary (releaseSlider_, releaseLabel_, "R", " s");

    auto& apvts = processorRef.getAPVTS();

    osc1WaveKnob_.attach (apvts, ParamIDs::osc1Wave);
    osc2WaveKnob_.attach (apvts, ParamIDs::osc2Wave);
    osc3WaveKnob_.attach (apvts, ParamIDs::osc3Wave);

    osc1WarpSwitch_.attach (apvts, ParamIDs::osc1WarpMode);
    osc2WarpSwitch_.attach (apvts, ParamIDs::osc2WarpMode);
    osc3WarpSwitch_.attach (apvts, ParamIDs::osc3WarpMode);

    osc1WarpSwitch_.onChange = [this] { refreshAllWarpAmountEnabled(); };
    osc2WarpSwitch_.onChange = [this] { refreshAllWarpAmountEnabled(); };
    osc3WarpSwitch_.onChange = [this] { refreshAllWarpAmountEnabled(); };
    refreshAllWarpAmountEnabled();

    osc1Waveform_.attach (processorRef, apvts, ParamIDs::osc1Wave, ParamIDs::osc1Position,
                          ParamIDs::osc1WarpMode, ParamIDs::osc1WarpAmount,
                          ParamIDs::osc1UnisonCount, ParamIDs::osc1UnisonDetune);
    osc2Waveform_.attach (processorRef, apvts, ParamIDs::osc2Wave, ParamIDs::osc2Position,
                          ParamIDs::osc2WarpMode, ParamIDs::osc2WarpAmount,
                          ParamIDs::osc2UnisonCount, ParamIDs::osc2UnisonDetune);
    osc3Waveform_.attach (processorRef, apvts, ParamIDs::osc3Wave, ParamIDs::osc3Position,
                          ParamIDs::osc3WarpMode, ParamIDs::osc3WarpAmount,
                          ParamIDs::osc3UnisonCount, ParamIDs::osc3UnisonDetune);

    noiseTypeSwitch_.attach (apvts, ParamIDs::noiseType);
    noiseLevelAttachment_ =
        std::make_unique<SliderAttachment> (apvts, ParamIDs::noiseLevel, noiseLevelSlider_);
    noisePanAttachment_ =
        std::make_unique<SliderAttachment> (apvts, ParamIDs::noisePan, noisePanSlider_);

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

    osc1PanAttachment_ =
        std::make_unique<SliderAttachment> (apvts, ParamIDs::osc1Pan, osc1PanSlider_);
    osc2PanAttachment_ =
        std::make_unique<SliderAttachment> (apvts, ParamIDs::osc2Pan, osc2PanSlider_);
    osc3PanAttachment_ =
        std::make_unique<SliderAttachment> (apvts, ParamIDs::osc3Pan, osc3PanSlider_);

    cutoffAttachment_ = std::make_unique<SliderAttachment> (apvts, ParamIDs::cutoff, cutoffSlider_);
    resonanceAttachment_ =
        std::make_unique<SliderAttachment> (apvts, ParamIDs::resonance, resonanceSlider_);

    envelopeEditor_.attach (apvts, ParamIDs::attack, ParamIDs::decay, ParamIDs::sustain,
                            ParamIDs::release);
    addAndMakeVisible (envelopeEditor_);

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

    setSize (980, 760);
    setResizable (false, false);
}

WavetableSynthAudioProcessorEditor::~WavetableSynthAudioProcessorEditor()
{ setLookAndFeel (nullptr); }

void WavetableSynthAudioProcessorEditor::setupRotary (juce::Slider& slider, juce::Label& label,
                                                      const juce::String& name,
                                                      const juce::String& unitSuffix)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    const int textW = unitSuffix.isEmpty() ? 64 : 72;
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, textW, kTextBoxH);
    if (unitSuffix.isNotEmpty())
        slider.setTextValueSuffix (unitSuffix);
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

void WavetableSynthAudioProcessorEditor::updateWarpAmountEnabled (
    WarpModeSwitch& warpSwitch, juce::Slider& warpAmountSlider, juce::Label& warpAmountLabel)
{
    // WarpMode::off ignores amount (see warpPhase).
    const bool active = warpSwitch.getSelectedIndex() != 0;
    warpAmountSlider.setEnabled (active);
    const float alpha = active ? 1.0f : 0.35f;
    warpAmountSlider.setAlpha (alpha);
    warpAmountLabel.setAlpha (alpha);
}

void WavetableSynthAudioProcessorEditor::refreshAllWarpAmountEnabled()
{
    updateWarpAmountEnabled (osc1WarpSwitch_, osc1WarpAmountSlider_, osc1WarpAmountLabel_);
    updateWarpAmountEnabled (osc2WarpSwitch_, osc2WarpAmountSlider_, osc2WarpAmountLabel_);
    updateWarpAmountEnabled (osc3WarpSwitch_, osc3WarpAmountSlider_, osc3WarpAmountLabel_);
}

void WavetableSynthAudioProcessorEditor::layoutKnobColumn (juce::Rectangle<int> column,
                                                           juce::Slider& slider, juce::Label& label)
{
    label.setBounds (column.removeFromTop (kLabelH));
    slider.setBounds (column.reduced (4, 2));
}

void WavetableSynthAudioProcessorEditor::layoutOscPanel (
    juce::Rectangle<int> bounds, OscWaveformDisplay& waveform, WaveSelectKnob& waveKnob,
    WarpModeSwitch& warpSwitch, juce::Slider& level, juce::Label& levelLabel, juce::Slider* detune,
    juce::Label* detuneLabel, juce::Slider& uniCount, juce::Label& uniCountLabel,
    juce::Slider& uniDetune, juce::Label& uniDetuneLabel, juce::Slider& warpAmount,
    juce::Label& warpAmountLabel, juce::Slider& position, juce::Label& positionLabel,
    juce::Slider& pan, juce::Label& panLabel)
{
    auto inner = bounds.reduced (10);
    inner.removeFromTop (kSectionTitleH);

    auto warpRow = inner.removeFromTop (kSwitchH + 6);
    const int warpW = juce::jmin (warpRow.getWidth() - 8, 160);
    warpSwitch.setBounds (warpRow.withSizeKeepingCentre (warpW, kSwitchH));

    constexpr int kWaveformH = 44;
    waveform.setBounds (inner.removeFromTop (kWaveformH));
    inner.removeFromTop (4);

    // Slightly larger than one rotary in this panel's top knob row.
    const int topCols = (detune != nullptr) ? 3 : 2;
    constexpr int kGapAfterWave = 8;
    constexpr float kWaveScale = 1.15f;
    const float avail = static_cast<float> (inner.getWidth() - kGapAfterWave);
    const int waveSize = juce::jmin (
        inner.getHeight(),
        juce::roundToInt (kWaveScale * avail
                          / (static_cast<float> (topCols) + kWaveScale)));
    auto waveArea = inner.removeFromLeft (waveSize);
    inner.removeFromLeft (kGapAfterWave);
    waveKnob.setBounds (waveArea.withSizeKeepingCentre (waveSize, waveSize));

    auto topRow = inner.removeFromTop (inner.getHeight() / 2);
    auto bottomRow = inner;

    const int topW = topRow.getWidth() / topCols;

    layoutKnobColumn (topRow.removeFromLeft (topW), level, levelLabel);
    if (detune != nullptr && detuneLabel != nullptr)
        layoutKnobColumn (topRow.removeFromLeft (topW), *detune, *detuneLabel);
    layoutKnobColumn (topRow, warpAmount, warpAmountLabel);

    const int botW = bottomRow.getWidth() / 4;
    layoutKnobColumn (bottomRow.removeFromLeft (botW), uniCount, uniCountLabel);
    layoutKnobColumn (bottomRow.removeFromLeft (botW), uniDetune, uniDetuneLabel);
    layoutKnobColumn (bottomRow.removeFromLeft (botW), position, positionLabel);
    layoutKnobColumn (bottomRow, pan, panLabel);
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
    paintSection (g, noiseSectionBounds_, "NOISE");
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

    // Give OSC panels more vertical room than the bottom filter/env row.
    auto bottom = bounds.removeFromBottom (juce::roundToInt (static_cast<float> (bounds.getHeight()) * 0.40f));
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

    noiseSectionBounds_ =
        bottom.removeFromLeft (juce::roundToInt (static_cast<float> (bottom.getWidth()) * 0.18f));
    bottom.removeFromLeft (kGap);
    filterSectionBounds_ =
        bottom.removeFromLeft (juce::roundToInt (static_cast<float> (bottom.getWidth()) * 0.32f));
    bottom.removeFromLeft (kGap);
    envSectionBounds_ = bottom;

    layoutOscPanel (osc1SectionBounds_, osc1Waveform_, osc1WaveKnob_, osc1WarpSwitch_,
                    osc1LevelSlider_, osc1LevelLabel_, nullptr, nullptr, osc1UnisonCountSlider_,
                    osc1UnisonCountLabel_, osc1UnisonDetuneSlider_, osc1UnisonDetuneLabel_,
                    osc1WarpAmountSlider_, osc1WarpAmountLabel_, osc1PositionSlider_,
                    osc1PositionLabel_, osc1PanSlider_, osc1PanLabel_);
    layoutOscPanel (osc2SectionBounds_, osc2Waveform_, osc2WaveKnob_, osc2WarpSwitch_,
                    osc2LevelSlider_, osc2LevelLabel_, &osc2DetuneSlider_, &osc2DetuneLabel_,
                    osc2UnisonCountSlider_, osc2UnisonCountLabel_, osc2UnisonDetuneSlider_,
                    osc2UnisonDetuneLabel_, osc2WarpAmountSlider_, osc2WarpAmountLabel_,
                    osc2PositionSlider_, osc2PositionLabel_, osc2PanSlider_, osc2PanLabel_);
    layoutOscPanel (osc3SectionBounds_, osc3Waveform_, osc3WaveKnob_, osc3WarpSwitch_,
                    osc3LevelSlider_, osc3LevelLabel_, &osc3DetuneSlider_, &osc3DetuneLabel_,
                    osc3UnisonCountSlider_, osc3UnisonCountLabel_, osc3UnisonDetuneSlider_,
                    osc3UnisonDetuneLabel_, osc3WarpAmountSlider_, osc3WarpAmountLabel_,
                    osc3PositionSlider_, osc3PositionLabel_, osc3PanSlider_, osc3PanLabel_);

    {
        auto inner = noiseSectionBounds_.reduced (10);
        inner.removeFromTop (kSectionTitleH);
        auto typeRow = inner.removeFromTop (kSwitchH + 6);
        const int typeW = juce::jmin (typeRow.getWidth() - 4, 120);
        noiseTypeSwitch_.setBounds (typeRow.withSizeKeepingCentre (typeW, kSwitchH));
        const int colW = inner.getWidth() / 2;
        layoutKnobColumn (inner.removeFromLeft (colW), noiseLevelSlider_, noiseLevelLabel_);
        layoutKnobColumn (inner, noisePanSlider_, noisePanLabel_);
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

        constexpr int kKnobRowH = 100;
        auto knobRow = inner.removeFromBottom (kKnobRowH);
        inner.removeFromBottom (6);
        envelopeEditor_.setBounds (inner);

        const int colW = knobRow.getWidth() / 4;
        layoutKnobColumn (knobRow.removeFromLeft (colW), attackSlider_, attackLabel_);
        layoutKnobColumn (knobRow.removeFromLeft (colW), decaySlider_, decayLabel_);
        layoutKnobColumn (knobRow.removeFromLeft (colW), sustainSlider_, sustainLabel_);
        layoutKnobColumn (knobRow, releaseSlider_, releaseLabel_);
    }
}
