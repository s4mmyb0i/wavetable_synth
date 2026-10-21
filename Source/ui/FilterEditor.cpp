#include "ui/FilterEditor.h"

#include <cmath>
#include <functional>

FilterEditor::FilterEditor()
{ setMouseCursor (juce::MouseCursor::NormalCursor); }

void FilterEditor::forEachParam (const std::function<void (juce::AudioProcessorParameter*)>& fn) const
{
    for (auto* p : {static_cast<juce::AudioProcessorParameter*> (cutoff_),
                    static_cast<juce::AudioProcessorParameter*> (resonance_),
                    static_cast<juce::AudioProcessorParameter*> (type_)})
        if (p != nullptr) fn (p);
}

FilterEditor::~FilterEditor()
{
    forEachParam ([this] (juce::AudioProcessorParameter* p) { p->removeListener (this); });
}

void FilterEditor::attach (juce::AudioProcessorValueTreeState& apvts, const juce::String& cutoffID,
                           const juce::String& resonanceID, const juce::String& typeID)
{
    forEachParam ([this] (juce::AudioProcessorParameter* p) { p->removeListener (this); });

    cutoff_ = dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter (cutoffID));
    resonance_ = dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter (resonanceID));
    type_ = dynamic_cast<juce::AudioParameterChoice*> (apvts.getParameter (typeID));

    forEachParam ([this] (juce::AudioProcessorParameter* p) { p->addListener (this); });

    repaint();
}

FilterType FilterEditor::currentType() const
{
    if (type_ == nullptr) return FilterType::lowPass;
    return static_cast<FilterType> (juce::jlimit (0, 2, type_->getIndex()));
}

juce::Rectangle<float> FilterEditor::plotBounds() const
{ return getLocalBounds().toFloat().reduced (10.0f, 8.0f); }

void FilterEditor::parameterValueChanged (int, float)
{
    juce::MessageManager::callAsync (
        [safe = juce::Component::SafePointer (this)]
        {
            if (safe != nullptr) safe->repaint();
        });
}

void FilterEditor::setParamValue (juce::AudioParameterFloat* param, float denormalised)
{
    if (param == nullptr) return;
    const float clamped = param->getNormalisableRange().snapToLegalValue (denormalised);
    param->setValueNotifyingHost (param->getNormalisableRange().convertTo0to1 (clamped));
}

bool FilterEditor::hitTestHandle (juce::Point<float> pos) const
{
    if (cutoff_ == nullptr || resonance_ == nullptr) return false;

    const auto bounds = plotBounds();
    const auto n = FilterResponse::handleNorm (cutoff_->get(), resonance_->get(), currentType());
    const auto px = FilterResponse::toPixels (n.x, n.y, bounds);
    return pos.getDistanceSquaredFrom (px) <= grabRadius * grabRadius;
}

void FilterEditor::syncCursor (bool overHandle)
{
    setMouseCursor (overHandle ? juce::MouseCursor::UpDownLeftRightResizeCursor
                               : juce::MouseCursor::NormalCursor);
}

void FilterEditor::mouseMove (const juce::MouseEvent& e)
{
    if (dragging_) return;
    hover_ = hitTestHandle (e.position);
    syncCursor (hover_);
    repaint();
}

void FilterEditor::mouseExit (const juce::MouseEvent&)
{
    if (dragging_) return;
    hover_ = false;
    syncCursor (false);
    repaint();
}

void FilterEditor::mouseDown (const juce::MouseEvent& e)
{
    dragging_ = hitTestHandle (e.position);
    if (! dragging_) return;

    gestureOpen_ = true;
    if (cutoff_ != nullptr) cutoff_->beginChangeGesture();
    if (resonance_ != nullptr) resonance_->beginChangeGesture();
    applyDrag (e.position);
}

void FilterEditor::mouseDrag (const juce::MouseEvent& e)
{
    if (dragging_) applyDrag (e.position);
}

void FilterEditor::mouseUp (const juce::MouseEvent& e)
{
    if (dragging_) applyDrag (e.position);

    if (gestureOpen_)
    {
        if (cutoff_ != nullptr) cutoff_->endChangeGesture();
        if (resonance_ != nullptr) resonance_->endChangeGesture();
        gestureOpen_ = false;
    }

    dragging_ = false;
    hover_ = hitTestHandle (e.position);
    syncCursor (hover_);
    repaint();
}

void FilterEditor::applyDrag (juce::Point<float> pos)
{
    const auto n = FilterResponse::fromPixels (pos, plotBounds());

    if (cutoff_ != nullptr)
        setParamValue (cutoff_, FilterResponse::normXToFreq (n.x));

    if (resonance_ != nullptr)
    {
        // Map handle Y using the LP/HP-at-fc relation mag ≈ Q = 1/R.
        // (Band-pass peak at fc is flatter; Y still adjusts resonance usefully.)
        const float db = FilterResponse::normYToDb (n.y);
        const float mag = std::pow (10.0f, db / 20.0f);
        const float R = 1.0f / juce::jmax (mag, 0.05f);
        const float res = juce::jlimit (0.0f, 1.0f, (2.0f - R) / 1.9f);
        setParamValue (resonance_, res);
    }
}

void FilterEditor::resized() {}

void FilterEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setColour (SynthLookAndFeel::background.brighter (0.04f));
    g.fillRoundedRectangle (bounds, 6.0f);
    g.setColour (SynthLookAndFeel::panelBorder);
    g.drawRoundedRectangle (bounds.reduced (0.5f), 6.0f, 1.0f);

    const auto plot = plotBounds();
    if (plot.getWidth() < 8.0f || plot.getHeight() < 8.0f) return;

    {
        g.setColour (SynthLookAndFeel::track.withAlpha (0.45f));
        for (float hz = 100.0f; hz < FilterResponse::maxHz; hz *= 10.0f)
        {
            const float x = FilterResponse::toPixels (FilterResponse::freqToNormX (hz), 0.0f, plot).x;
            g.drawVerticalLine (juce::roundToInt (x), plot.getY(), plot.getBottom());
        }

        const float zeroY =
            FilterResponse::toPixels (0.0f, FilterResponse::dbToNormY (0.0f), plot).y;
        g.setColour (SynthLookAndFeel::track);
        g.drawHorizontalLine (juce::roundToInt (zeroY), plot.getX(), plot.getRight());
    }

    if (cutoff_ == nullptr || resonance_ == nullptr) return;

    const float cutoff = cutoff_->get();
    const float resonance = resonance_->get();
    const auto type = currentType();
    auto path = FilterResponse::buildPath (plot, cutoff, resonance, type);

    {
        juce::Path fill = path;
        const float zeroY =
            FilterResponse::toPixels (0.0f, FilterResponse::dbToNormY (0.0f), plot).y;
        fill.lineTo (plot.getRight(), zeroY);
        fill.lineTo (plot.getX(), zeroY);
        fill.closeSubPath();
        g.setColour (SynthLookAndFeel::accent.withAlpha (0.12f));
        g.fillPath (fill);
    }

    g.setColour (SynthLookAndFeel::accent);
    g.strokePath (path, juce::PathStrokeType (2.0f, juce::PathStrokeType::curved,
                                              juce::PathStrokeType::rounded));

    const auto n = FilterResponse::handleNorm (cutoff, resonance, type);
    const auto c = FilterResponse::toPixels (n.x, n.y, plot);
    const bool hot = dragging_ || hover_;
    g.setColour (hot ? SynthLookAndFeel::textPrimary : SynthLookAndFeel::accent);
    g.fillEllipse (c.x - handleRadius, c.y - handleRadius, handleRadius * 2.0f, handleRadius * 2.0f);
    g.setColour (SynthLookAndFeel::panel);
    g.drawEllipse (c.x - handleRadius, c.y - handleRadius, handleRadius * 2.0f, handleRadius * 2.0f,
                   1.2f);
}
