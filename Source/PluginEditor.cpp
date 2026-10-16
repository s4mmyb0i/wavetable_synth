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
      cpuMeter_ (p),
      keyboard_ (p.getKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard)
{
    setLookAndFeel (&lookAndFeel_);

    titleLabel_.setText ("Wavetable Synth", juce::dontSendNotification);
    titleLabel_.setFont (juce::FontOptions (22.0f, juce::Font::bold));
    titleLabel_.setColour (juce::Label::textColourId, SynthLookAndFeel::textPrimary);
    titleLabel_.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (titleLabel_);
    addAndMakeVisible (cpuMeter_);

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

    osc1EnableLed_.attach (apvts, ParamIDs::osc1On, ParamIDs::osc1Level);
    osc2EnableLed_.attach (apvts, ParamIDs::osc2On, ParamIDs::osc2Level);
    osc3EnableLed_.attach (apvts, ParamIDs::osc3On, ParamIDs::osc3Level);
    noiseEnableLed_.attach (apvts, ParamIDs::noiseOn, ParamIDs::noiseLevel);
    osc1EnableLed_.onChange = [this] { refreshSourcePanelDims(); };
    osc2EnableLed_.onChange = [this] { refreshSourcePanelDims(); };
    osc3EnableLed_.onChange = [this] { refreshSourcePanelDims(); };
    noiseEnableLed_.onChange = [this] { refreshSourcePanelDims(); };
    addAndMakeVisible (osc1EnableLed_);
    addAndMakeVisible (osc2EnableLed_);
    addAndMakeVisible (osc3EnableLed_);
    addAndMakeVisible (noiseEnableLed_);
    refreshSourcePanelDims();

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

    filterHintLabel_.setJustificationType (juce::Justification::centred);
    filterHintLabel_.setColour (juce::Label::textColourId, SynthLookAndFeel::textMuted);
    filterHintLabel_.setFont (juce::FontOptions (11.0f));
    addAndMakeVisible (filterHintLabel_);

    addAndMakeVisible (filterSourceSwitch_);
    addAndMakeVisible (filterEditor_);
    filterSourceSwitch_.onChange = [this]
    {
        const int idx = filterSourceSwitch_.getSelectedIndex();
        attachFilterSource (idx);
        updateFilterHint (idx);
    };
    attachFilterSource (0);
    updateFilterHint (0);

    addAndMakeVisible (envSourceSwitch_);
    addAndMakeVisible (envelopeEditor_);
    envSourceSwitch_.onChange = [this]
    { attachEnvelopeSource (envSourceSwitch_.getSelectedIndex()); };
    attachEnvelopeSource (0);

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

    setSize (980, 880);
    setResizable (true, true);
    setResizeLimits (760, 640, 1600, 1400);
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

void WavetableSynthAudioProcessorEditor::attachEnvelopeSource (int sourceIndex)
{
    struct Ids
    {
        const char* attack;
        const char* decay;
        const char* sustain;
        const char* release;
        const char* attackCurveX;
        const char* attackCurveY;
        const char* decayCurveX;
        const char* decayCurveY;
        const char* releaseCurveX;
        const char* releaseCurveY;
    };

    static constexpr Ids kIds[] = {
        {ParamIDs::osc1Attack, ParamIDs::osc1Decay, ParamIDs::osc1Sustain, ParamIDs::osc1Release,
         ParamIDs::osc1AttackCurveX, ParamIDs::osc1AttackCurveY, ParamIDs::osc1DecayCurveX,
         ParamIDs::osc1DecayCurveY, ParamIDs::osc1ReleaseCurveX, ParamIDs::osc1ReleaseCurveY},
        {ParamIDs::osc2Attack, ParamIDs::osc2Decay, ParamIDs::osc2Sustain, ParamIDs::osc2Release,
         ParamIDs::osc2AttackCurveX, ParamIDs::osc2AttackCurveY, ParamIDs::osc2DecayCurveX,
         ParamIDs::osc2DecayCurveY, ParamIDs::osc2ReleaseCurveX, ParamIDs::osc2ReleaseCurveY},
        {ParamIDs::osc3Attack, ParamIDs::osc3Decay, ParamIDs::osc3Sustain, ParamIDs::osc3Release,
         ParamIDs::osc3AttackCurveX, ParamIDs::osc3AttackCurveY, ParamIDs::osc3DecayCurveX,
         ParamIDs::osc3DecayCurveY, ParamIDs::osc3ReleaseCurveX, ParamIDs::osc3ReleaseCurveY},
        {ParamIDs::noiseAttack, ParamIDs::noiseDecay, ParamIDs::noiseSustain,
         ParamIDs::noiseRelease, ParamIDs::noiseAttackCurveX, ParamIDs::noiseAttackCurveY,
         ParamIDs::noiseDecayCurveX, ParamIDs::noiseDecayCurveY, ParamIDs::noiseReleaseCurveX,
         ParamIDs::noiseReleaseCurveY},
    };

    const auto& ids = kIds[juce::jlimit (0, 3, sourceIndex)];
    auto& apvts = processorRef.getAPVTS();

    // Drop old attachments before rebinding so we don't briefly write to the wrong params.
    attackAttachment_.reset();
    decayAttachment_.reset();
    sustainAttachment_.reset();
    releaseAttachment_.reset();

    envelopeEditor_.attach (apvts, ids.attack, ids.decay, ids.sustain, ids.release,
                            ids.attackCurveX, ids.attackCurveY, ids.decayCurveX, ids.decayCurveY,
                            ids.releaseCurveX, ids.releaseCurveY);
    attackAttachment_ = std::make_unique<SliderAttachment> (apvts, ids.attack, attackSlider_);
    decayAttachment_ = std::make_unique<SliderAttachment> (apvts, ids.decay, decaySlider_);
    sustainAttachment_ = std::make_unique<SliderAttachment> (apvts, ids.sustain, sustainSlider_);
    releaseAttachment_ = std::make_unique<SliderAttachment> (apvts, ids.release, releaseSlider_);
}

void WavetableSynthAudioProcessorEditor::attachFilterSource (int sourceIndex)
{
    struct Ids
    {
        const char* cutoff;
        const char* resonance;
    };

    static constexpr Ids kIds[] = {
        {ParamIDs::cutoff, ParamIDs::resonance},
        {ParamIDs::noiseCutoff, ParamIDs::noiseResonance},
    };

    const auto& ids = kIds[juce::jlimit (0, 1, sourceIndex)];
    auto& apvts = processorRef.getAPVTS();

    cutoffAttachment_.reset();
    resonanceAttachment_.reset();

    filterEditor_.attach (apvts, ids.cutoff, ids.resonance);
    cutoffAttachment_ = std::make_unique<SliderAttachment> (apvts, ids.cutoff, cutoffSlider_);
    resonanceAttachment_ =
        std::make_unique<SliderAttachment> (apvts, ids.resonance, resonanceSlider_);
}

void WavetableSynthAudioProcessorEditor::updateFilterHint (int sourceIndex)
{
    filterHintLabel_.setText (sourceIndex == 0 ? "Shared low-pass on Osc 1 + 2 + 3 mix"
                                               : "Low-pass on noise only",
                              juce::dontSendNotification);
}

void WavetableSynthAudioProcessorEditor::layoutKnobColumn (juce::Rectangle<int> column,
                                                           juce::Slider& slider, juce::Label& label,
                                                           int fixedSide)
{
    label.setBounds (column.removeFromTop (kLabelH));
    const int side = fixedSide > 0
                         ? juce::jmin (fixedSide, column.getWidth(), column.getHeight())
                         : juce::jmin (column.getWidth(), column.getHeight());
    slider.setBounds (column.withSizeKeepingCentre (side, side).reduced (2));
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

    // Bottom row uses the full panel width so 4 knobs can match the top-row size.
    constexpr int kTopCols = 3;
    constexpr int kBotCols = 4;
    constexpr int kGapAfterWave = 8;
    constexpr float kWaveScale = 1.15f;

    const int bottomH = inner.getHeight() / 2;
    auto bottomRow = inner.removeFromBottom (bottomH);
    auto topArea = inner;

    const float avail = static_cast<float> (topArea.getWidth() - kGapAfterWave);
    const int waveSize = juce::jmin (
        topArea.getHeight(),
        juce::roundToInt (kWaveScale * avail / (static_cast<float> (kTopCols) + kWaveScale)));
    auto waveArea = topArea.removeFromLeft (waveSize);
    topArea.removeFromLeft (kGapAfterWave);
    waveKnob.setBounds (waveArea.withSizeKeepingCentre (waveSize, waveSize));

    const int topW = topArea.getWidth() / kTopCols;
    const int botW = bottomRow.getWidth() / kBotCols;
    const int knobSide = juce::jmin (topW, botW, topArea.getHeight() - kLabelH,
                                     bottomRow.getHeight() - kLabelH);

    layoutKnobColumn (topArea.removeFromLeft (topW), level, levelLabel, knobSide);
    auto detuneCol = topArea.removeFromLeft (topW);
    if (detune != nullptr && detuneLabel != nullptr)
        layoutKnobColumn (detuneCol, *detune, *detuneLabel, knobSide);
    layoutKnobColumn (topArea, warpAmount, warpAmountLabel, knobSide);

    layoutKnobColumn (bottomRow.removeFromLeft (botW), uniCount, uniCountLabel, knobSide);
    layoutKnobColumn (bottomRow.removeFromLeft (botW), uniDetune, uniDetuneLabel, knobSide);
    layoutKnobColumn (bottomRow.removeFromLeft (botW), position, positionLabel, knobSide);
    layoutKnobColumn (bottomRow, pan, panLabel, knobSide);
}

void WavetableSynthAudioProcessorEditor::paintSection (juce::Graphics& g,
                                                       juce::Rectangle<int> bounds,
                                                       const juce::String& title, bool dimmed)
{
    g.setColour (dimmed ? SynthLookAndFeel::panel.darker (0.35f) : SynthLookAndFeel::panel);
    g.fillRoundedRectangle (bounds.toFloat(), 10.0f);
    g.setColour (SynthLookAndFeel::panelBorder.withMultipliedAlpha (dimmed ? 0.55f : 1.0f));
    g.drawRoundedRectangle (bounds.toFloat().reduced (0.5f), 10.0f, 1.0f);

    g.setColour (dimmed ? SynthLookAndFeel::textMuted : SynthLookAndFeel::accent);
    g.setFont (juce::FontOptions (12.0f, juce::Font::bold));
    // Leave room on the right for an optional enable LED.
    auto titleArea = bounds.removeFromTop (kSectionTitleH).reduced (12, 2);
    titleArea.removeFromRight (22);
    g.drawText (title, titleArea, juce::Justification::centredLeft, false);
}

void WavetableSynthAudioProcessorEditor::setComponentsDimmed (
    bool dimmed, std::initializer_list<juce::Component*> components)
{
    const float alpha = dimmed ? 0.38f : 1.0f;
    for (auto* c : components)
        if (c != nullptr) c->setAlpha (alpha);
}

void WavetableSynthAudioProcessorEditor::refreshSourcePanelDims()
{
    setComponentsDimmed (! osc1EnableLed_.isSourceOn(),
                         {&osc1Waveform_, &osc1LevelSlider_, &osc1LevelLabel_, &osc1WaveKnob_,
                          &osc1WarpSwitch_, &osc1UnisonCountSlider_, &osc1UnisonDetuneSlider_,
                          &osc1WarpAmountSlider_, &osc1PositionSlider_, &osc1PanSlider_,
                          &osc1UnisonCountLabel_, &osc1UnisonDetuneLabel_, &osc1WarpAmountLabel_,
                          &osc1PositionLabel_, &osc1PanLabel_});

    setComponentsDimmed (! osc2EnableLed_.isSourceOn(),
                         {&osc2Waveform_, &osc2LevelSlider_, &osc2LevelLabel_, &osc2DetuneSlider_,
                          &osc2DetuneLabel_, &osc2WaveKnob_, &osc2WarpSwitch_,
                          &osc2UnisonCountSlider_, &osc2UnisonDetuneSlider_, &osc2WarpAmountSlider_,
                          &osc2PositionSlider_, &osc2PanSlider_, &osc2UnisonCountLabel_,
                          &osc2UnisonDetuneLabel_, &osc2WarpAmountLabel_, &osc2PositionLabel_,
                          &osc2PanLabel_});

    setComponentsDimmed (! osc3EnableLed_.isSourceOn(),
                         {&osc3Waveform_, &osc3LevelSlider_, &osc3LevelLabel_, &osc3DetuneSlider_,
                          &osc3DetuneLabel_, &osc3WaveKnob_, &osc3WarpSwitch_,
                          &osc3UnisonCountSlider_, &osc3UnisonDetuneSlider_, &osc3WarpAmountSlider_,
                          &osc3PositionSlider_, &osc3PanSlider_, &osc3UnisonCountLabel_,
                          &osc3UnisonDetuneLabel_, &osc3WarpAmountLabel_, &osc3PositionLabel_,
                          &osc3PanLabel_});

    setComponentsDimmed (! noiseEnableLed_.isSourceOn(),
                         {&noiseTypeSwitch_, &noiseLevelSlider_, &noisePanSlider_,
                          &noiseLevelLabel_, &noisePanLabel_});

    repaint();
}

namespace
{
void layoutSectionLed (juce::Rectangle<int> sectionBounds, SourceEnableLed& led)
{
    auto titleRow = sectionBounds.removeFromTop (kSectionTitleH).reduced (10, 0);
    constexpr int kLed = 14;
    led.setBounds (titleRow.removeFromRight (kLed + 4)
                       .withSizeKeepingCentre (kLed, kLed)
                       .translated (0, 2));
}
} // namespace

void WavetableSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (SynthLookAndFeel::background);

    g.setColour (SynthLookAndFeel::accent.withAlpha (0.35f));
    g.fillRect (0, 0, getWidth(), 2);

    paintSection (g, osc1SectionBounds_, "OSC 1", ! osc1EnableLed_.isSourceOn());
    paintSection (g, osc2SectionBounds_, "OSC 2", ! osc2EnableLed_.isSourceOn());
    paintSection (g, osc3SectionBounds_, "OSC 3", ! osc3EnableLed_.isSourceOn());
    paintSection (g, noiseSectionBounds_, "NOISE", ! noiseEnableLed_.isSourceOn());
    paintSection (g, filterSectionBounds_, "FILTERS");
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
    constexpr int kCpuW = 140;
    cpuMeter_.setBounds (header.removeFromRight (kCpuW).reduced (0, 2));
    header.removeFromRight (10);
    titleLabel_.setBounds (header);

    bounds.removeFromTop (8);

    // Bottom row needs room for filter + envelope graphs (env also has source switch).
    auto bottom = bounds.removeFromBottom (
        juce::roundToInt (static_cast<float> (bounds.getHeight()) * 0.48f));
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
        bottom.removeFromLeft (juce::roundToInt (static_cast<float> (bottom.getWidth()) * 0.16f));
    bottom.removeFromLeft (kGap);
    filterSectionBounds_ =
        bottom.removeFromLeft (juce::roundToInt (static_cast<float> (bottom.getWidth()) * 0.34f));
    bottom.removeFromLeft (kGap);
    envSectionBounds_ = bottom;

    layoutSectionLed (osc1SectionBounds_, osc1EnableLed_);
    layoutSectionLed (osc2SectionBounds_, osc2EnableLed_);
    layoutSectionLed (osc3SectionBounds_, osc3EnableLed_);
    layoutSectionLed (noiseSectionBounds_, noiseEnableLed_);

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

        auto sourceRow = inner.removeFromTop (kSwitchH + 2);
        const int sourceW = juce::jmin (sourceRow.getWidth() - 4, 200);
        filterSourceSwitch_.setBounds (sourceRow.withSizeKeepingCentre (sourceW, kSwitchH));

        constexpr int kHintH = 16;
        filterHintLabel_.setBounds (inner.removeFromTop (kHintH));
        inner.removeFromTop (2);

        constexpr int kKnobRowH = 96;
        auto knobRow = inner.removeFromBottom (kKnobRowH);
        inner.removeFromBottom (6);
        filterEditor_.setBounds (inner);

        const int colW = knobRow.getWidth() / 2;
        layoutKnobColumn (knobRow.removeFromLeft (colW), cutoffSlider_, cutoffLabel_);
        layoutKnobColumn (knobRow, resonanceSlider_, resonanceLabel_);
    }

    {
        auto inner = envSectionBounds_.reduced (10);
        inner.removeFromTop (kSectionTitleH);

        auto sourceRow = inner.removeFromTop (kSwitchH + 4);
        const int sourceW = juce::jmin (sourceRow.getWidth() - 4, 280);
        envSourceSwitch_.setBounds (sourceRow.withSizeKeepingCentre (sourceW, kSwitchH));

        constexpr int kKnobRowH = 96;
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
