#include "ui/WaveSelectKnob.h"

#include <cmath>

namespace
{
// Five stops evenly around the full circle. Needle travel is (N-1) steps so Morph
// and Sine stay distinct; wrapping is handled in mouse code, not by JUCE stopAtEnd.
constexpr int kNumWaves = 5;
constexpr float kStartAngle = -juce::MathConstants<float>::halfPi;
constexpr float kStep = juce::MathConstants<float>::twoPi / static_cast<float> (kNumWaves);
constexpr float kEndAngle = kStartAngle + kStep * static_cast<float> (kNumWaves - 1);

int waveIndexFromAngle (float angle)
{
    float rel = angle - kStartAngle;
    while (rel < 0.0f) rel += juce::MathConstants<float>::twoPi;
    while (rel >= juce::MathConstants<float>::twoPi) rel -= juce::MathConstants<float>::twoPi;

    int index = juce::roundToInt (rel / kStep);
    if (index >= kNumWaves) index = 0;
    return index;
}
} // namespace

WaveSelectKnob::WaveSelectKnob()
{
    // Drawn as a rotary; mouse is handled here so rotation can wrap forever.
    slider_.setSliderStyle (juce::Slider::Rotary);
    slider_.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    slider_.setRotaryParameters (kStartAngle, kEndAngle, true);
    slider_.getProperties().set ("waveKnob", true);
    slider_.setNumDecimalPlacesToDisplay (0);
    slider_.setInterceptsMouseClicks (false, false);
    slider_.onValueChange = [this] { repaint(); };
    addAndMakeVisible (slider_);
}

void WaveSelectKnob::attach (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID)
{
    attachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        apvts, paramID, slider_);
    param_ = apvts.getParameter (paramID);
}

void WaveSelectKnob::resized() { slider_.setBounds (getLocalBounds()); }

void WaveSelectKnob::setWaveFromMouse (juce::Point<float> pos)
{
    const auto centre = getLocalBounds().getCentre().toFloat();
    const float dx = pos.x - centre.x;
    const float dy = pos.y - centre.y;

    // Ignore jitter right on the centre.
    if (dx * dx + dy * dy < 16.0f) return;

    const float angle = std::atan2 (dx, -dy);
    const double value = static_cast<double> (waveIndexFromAngle (angle));

    if (std::abs (slider_.getValue() - value) > 0.001)
        slider_.setValue (value, juce::sendNotificationSync);
}

void WaveSelectKnob::mouseDown (const juce::MouseEvent& e)
{
    dragging_ = true;
    if (param_ != nullptr)
        param_->beginChangeGesture();
    setWaveFromMouse (e.position);
}

void WaveSelectKnob::mouseDrag (const juce::MouseEvent& e)
{
    if (dragging_)
        setWaveFromMouse (e.position);
}

void WaveSelectKnob::mouseUp (const juce::MouseEvent& e)
{
    if (! dragging_) return;
    dragging_ = false;
    setWaveFromMouse (e.position);
    if (param_ != nullptr)
        param_->endChangeGesture();
}

void WaveSelectKnob::drawWaveIcon (juce::Graphics& g, juce::Rectangle<float> area, int waveIndex,
                                   bool selected) const
{
    auto r = area.reduced (1.0f);
    if (r.getWidth() < 6.0f || r.getHeight() < 6.0f) return;

    const float x0 = r.getX();
    const float x1 = r.getRight();
    const float midY = r.getCentreY();
    const float amp = r.getHeight() * 0.40f;
    const float xm = r.getCentreX();

    juce::Path path;

    switch (waveIndex)
    {
    case 0: // sine
    {
        constexpr int steps = 32;
        path.startNewSubPath (x0, midY);
        for (int i = 1; i <= steps; ++i)
        {
            const float t = static_cast<float> (i) / static_cast<float> (steps);
            path.lineTo (x0 + t * (x1 - x0),
                         midY - amp * std::sin (t * juce::MathConstants<float>::twoPi));
        }
        break;
    }
    case 1: // saw — ramp up, hard drop
        path.startNewSubPath (x0, midY + amp);
        path.lineTo (x1, midY - amp);
        path.lineTo (x1, midY + amp);
        break;
    case 2: // square — flat highs/lows with vertical edges
        path.startNewSubPath (x0, midY - amp);
        path.lineTo (xm, midY - amp);
        path.lineTo (xm, midY + amp);
        path.lineTo (x1, midY + amp);
        break;
    case 3: // triangle
        path.startNewSubPath (x0, midY + amp);
        path.lineTo (xm, midY - amp);
        path.lineTo (x1, midY + amp);
        break;
    case 4: // morph — denser multi-harmonic scribble
    {
        constexpr int steps = 40;
        path.startNewSubPath (x0, midY);
        for (int i = 1; i <= steps; ++i)
        {
            const float t = static_cast<float> (i) / static_cast<float> (steps);
            const float a = std::sin (t * juce::MathConstants<float>::twoPi);
            const float b = std::sin (t * juce::MathConstants<float>::twoPi * 3.0f);
            const float c = std::sin (t * juce::MathConstants<float>::twoPi * 5.0f);
            path.lineTo (x0 + t * (x1 - x0), midY - amp * (0.55f * a + 0.30f * b + 0.15f * c));
        }
        break;
    }
    default: return;
    }

    const bool curved = (waveIndex == 0 || waveIndex == 4);
    g.setColour (selected ? SynthLookAndFeel::accent : SynthLookAndFeel::textMuted);
    g.strokePath (path,
                  juce::PathStrokeType (selected ? 2.0f : 1.5f,
                                        curved ? juce::PathStrokeType::curved
                                               : juce::PathStrokeType::mitered,
                                        juce::PathStrokeType::rounded));
}

void WaveSelectKnob::paint (juce::Graphics&) {}

void WaveSelectKnob::paintOverChildren (juce::Graphics& g) { paintIcons (g); }

void WaveSelectKnob::paintIcons (juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();
    const auto centre = bounds.getCentre();
    const float shortSide = juce::jmin (bounds.getWidth(), bounds.getHeight());
    const float iconSize = juce::jlimit (14.0f, 22.0f, shortSide * 0.22f);
    const float iconR = shortSide * 0.5f - iconSize * 0.55f;

    const int selected = juce::roundToInt (slider_.getValue());

    for (int i = 0; i < numWaves; ++i)
    {
        const float angle = kStartAngle + static_cast<float> (i) * kStep;
        const auto p = centre.getPointOnCircumference (iconR, angle);
        drawWaveIcon (g, juce::Rectangle<float> (iconSize, iconSize).withCentre (p), i,
                      i == selected);
    }
}
