#include "ui/EnvelopeEditor.h"

EnvelopeEditor::EnvelopeEditor()
{ setMouseCursor (juce::MouseCursor::NormalCursor); }

void EnvelopeEditor::forEachParam (
    const std::function<void (juce::AudioProcessorParameter*)>& fn) const
{
    for (auto* p : {static_cast<juce::AudioProcessorParameter*> (attack_),
                    static_cast<juce::AudioProcessorParameter*> (decay_),
                    static_cast<juce::AudioProcessorParameter*> (sustain_),
                    static_cast<juce::AudioProcessorParameter*> (release_),
                    static_cast<juce::AudioProcessorParameter*> (attackCurveX_),
                    static_cast<juce::AudioProcessorParameter*> (attackCurveY_),
                    static_cast<juce::AudioProcessorParameter*> (decayCurveX_),
                    static_cast<juce::AudioProcessorParameter*> (decayCurveY_),
                    static_cast<juce::AudioProcessorParameter*> (releaseCurveX_),
                    static_cast<juce::AudioProcessorParameter*> (releaseCurveY_)})
        if (p != nullptr) fn (p);
}

EnvelopeEditor::~EnvelopeEditor()
{
    forEachParam ([this] (juce::AudioProcessorParameter* p) { p->removeListener (this); });
}

void EnvelopeEditor::attach (juce::AudioProcessorValueTreeState& apvts,
                             const juce::String& attackID, const juce::String& decayID,
                             const juce::String& sustainID, const juce::String& releaseID,
                             const juce::String& attackCurveXID, const juce::String& attackCurveYID,
                             const juce::String& decayCurveXID, const juce::String& decayCurveYID,
                             const juce::String& releaseCurveXID, const juce::String& releaseCurveYID)
{
    forEachParam ([this] (juce::AudioProcessorParameter* p) { p->removeListener (this); });

    attack_ = dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter (attackID));
    decay_ = dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter (decayID));
    sustain_ = dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter (sustainID));
    release_ = dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter (releaseID));
    attackCurveX_ = dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter (attackCurveXID));
    attackCurveY_ = dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter (attackCurveYID));
    decayCurveX_ = dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter (decayCurveXID));
    decayCurveY_ = dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter (decayCurveYID));
    releaseCurveX_ = dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter (releaseCurveXID));
    releaseCurveY_ = dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter (releaseCurveYID));

    if (attack_ != nullptr) timeRange_ = attack_->getNormalisableRange();

    forEachParam ([this] (juce::AudioProcessorParameter* p) { p->addListener (this); });

    repaint();
}

EnvelopeShape::Params EnvelopeEditor::readParams() const
{
    EnvelopeShape::Params p;
    if (attack_ != nullptr) p.attack = attack_->get();
    if (decay_ != nullptr) p.decay = decay_->get();
    if (sustain_ != nullptr) p.sustain = sustain_->get();
    if (release_ != nullptr) p.release = release_->get();
    if (attackCurveX_ != nullptr) p.attackCurve.x = attackCurveX_->get();
    if (attackCurveY_ != nullptr) p.attackCurve.y = attackCurveY_->get();
    if (decayCurveX_ != nullptr) p.decayCurve.x = decayCurveX_->get();
    if (decayCurveY_ != nullptr) p.decayCurve.y = decayCurveY_->get();
    if (releaseCurveX_ != nullptr) p.releaseCurve.x = releaseCurveX_->get();
    if (releaseCurveY_ != nullptr) p.releaseCurve.y = releaseCurveY_->get();
    return p;
}

juce::Rectangle<float> EnvelopeEditor::plotBounds() const
{ return getLocalBounds().toFloat().reduced (10.0f, 8.0f); }

void EnvelopeEditor::parameterValueChanged (int, float)
{
    juce::MessageManager::callAsync (
        [safe = juce::Component::SafePointer (this)]
        {
            if (safe != nullptr) safe->repaint();
        });
}

void EnvelopeEditor::setParamValue (juce::AudioParameterFloat* param, float denormalised)
{
    if (param == nullptr) return;
    const float clamped = param->getNormalisableRange().snapToLegalValue (denormalised);
    param->setValueNotifyingHost (param->getNormalisableRange().convertTo0to1 (clamped));
}

void EnvelopeEditor::setCurveHandle (juce::AudioParameterFloat* xParam,
                                     juce::AudioParameterFloat* yParam, EnvelopeCurve::Handle h)
{
    h = EnvelopeCurve::clampHandle (h);
    setParamValue (xParam, h.x);
    setParamValue (yParam, h.y);
}

int EnvelopeEditor::hitTestHandle (juce::Point<float> pos) const
{
    const auto bounds = plotBounds();
    const auto params = readParams();
    const auto pts = EnvelopeShape::buildPoints (params, timeRange_);
    const auto curves = EnvelopeShape::buildCurveHandles (params, timeRange_);
    float bestDist = grabRadius * grabRadius;
    int best = -1;

    auto consider = [&] (int handle, juce::Point<float> px)
    {
        const float d = pos.getDistanceSquaredFrom (px);
        if (d <= bestDist)
        {
            bestDist = d;
            best = handle;
        }
    };

    for (int i = EnvelopeShape::attackPeak; i <= EnvelopeShape::releaseEnd; ++i)
        consider (i, EnvelopeShape::toPixels (pts[static_cast<size_t> (i)], bounds));

    consider (EnvelopeShape::attackCurve, EnvelopeShape::toPixels (curves[0], bounds));
    consider (EnvelopeShape::decayCurve, EnvelopeShape::toPixels (curves[1], bounds));
    consider (EnvelopeShape::releaseCurve, EnvelopeShape::toPixels (curves[2], bounds));

    return best;
}

void EnvelopeEditor::syncCursor (int handle)
{
    if (handle == EnvelopeShape::decaySustain)
        setMouseCursor (juce::MouseCursor::UpDownLeftRightResizeCursor);
    else if (handle == EnvelopeShape::sustainEnd)
        setMouseCursor (juce::MouseCursor::UpDownResizeCursor);
    else if (handle == EnvelopeShape::attackPeak || handle == EnvelopeShape::releaseEnd)
        setMouseCursor (juce::MouseCursor::LeftRightResizeCursor);
    else if (handle == EnvelopeShape::attackCurve || handle == EnvelopeShape::decayCurve
             || handle == EnvelopeShape::releaseCurve)
        setMouseCursor (juce::MouseCursor::UpDownLeftRightResizeCursor);
    else
        setMouseCursor (juce::MouseCursor::NormalCursor);
}

void EnvelopeEditor::mouseMove (const juce::MouseEvent& e)
{
    if (activeHandle_ >= 0) return;
    hoverHandle_ = hitTestHandle (e.position);
    syncCursor (hoverHandle_);
    repaint();
}

void EnvelopeEditor::mouseExit (const juce::MouseEvent&)
{
    if (activeHandle_ >= 0) return;
    hoverHandle_ = -1;
    syncCursor (-1);
    repaint();
}

void EnvelopeEditor::mouseDown (const juce::MouseEvent& e)
{
    activeHandle_ = hitTestHandle (e.position);
    if (activeHandle_ < 0) return;

    gestureOpen_ = true;
    auto begin = [] (juce::AudioProcessorParameter* p)
    {
        if (p != nullptr) p->beginChangeGesture();
    };
    switch (activeHandle_)
    {
    case EnvelopeShape::attackPeak: begin (attack_); break;
    case EnvelopeShape::decaySustain:
        begin (decay_);
        begin (sustain_);
        break;
    case EnvelopeShape::sustainEnd: begin (sustain_); break;
    case EnvelopeShape::releaseEnd: begin (release_); break;
    case EnvelopeShape::attackCurve:
        begin (attackCurveX_);
        begin (attackCurveY_);
        break;
    case EnvelopeShape::decayCurve:
        begin (decayCurveX_);
        begin (decayCurveY_);
        break;
    case EnvelopeShape::releaseCurve:
        begin (releaseCurveX_);
        begin (releaseCurveY_);
        break;
    default: break;
    }

    applyDrag (e.position);
}

void EnvelopeEditor::mouseDrag (const juce::MouseEvent& e)
{
    if (activeHandle_ >= 0) applyDrag (e.position);
}

void EnvelopeEditor::mouseUp (const juce::MouseEvent& e)
{
    if (activeHandle_ >= 0) applyDrag (e.position);

    if (gestureOpen_)
    {
        auto end = [] (juce::AudioProcessorParameter* p)
        {
            if (p != nullptr) p->endChangeGesture();
        };
        switch (activeHandle_)
        {
        case EnvelopeShape::attackPeak: end (attack_); break;
        case EnvelopeShape::decaySustain:
            end (decay_);
            end (sustain_);
            break;
        case EnvelopeShape::sustainEnd: end (sustain_); break;
        case EnvelopeShape::releaseEnd: end (release_); break;
        case EnvelopeShape::attackCurve:
            end (attackCurveX_);
            end (attackCurveY_);
            break;
        case EnvelopeShape::decayCurve:
            end (decayCurveX_);
            end (decayCurveY_);
            break;
        case EnvelopeShape::releaseCurve:
            end (releaseCurveX_);
            end (releaseCurveY_);
            break;
        default: break;
        }
        gestureOpen_ = false;
    }

    activeHandle_ = -1;
    hoverHandle_ = hitTestHandle (e.position);
    syncCursor (hoverHandle_);
    repaint();
}

void EnvelopeEditor::applyDrag (juce::Point<float> pos)
{
    const auto bounds = plotBounds();
    auto n = EnvelopeShape::fromPixels (pos, bounds);
    n.x = juce::jlimit (0.0f, 1.0f, n.x);
    n.y = juce::jlimit (0.0f, 1.0f, n.y);

    const float adr = 1.0f - EnvelopeShape::sustainFraction;
    const float band = adr / 3.0f;
    const float a0 = 0.0f;
    const float d0 = band;
    const float r0 = 2.0f * band + EnvelopeShape::sustainFraction;

    auto normInBand = [band] (float x, float bandStart) -> float
    { return juce::jlimit (0.0f, 1.0f, (x - bandStart) / band); };

    const auto corners = EnvelopeShape::buildPoints (readParams(), timeRange_);

    switch (activeHandle_)
    {
    case EnvelopeShape::attackPeak:
        setParamValue (attack_, timeRange_.convertFrom0to1 (normInBand (n.x, a0)));
        break;

    case EnvelopeShape::decaySustain:
        setParamValue (decay_, timeRange_.convertFrom0to1 (normInBand (n.x, d0)));
        setParamValue (sustain_, n.y);
        break;

    case EnvelopeShape::sustainEnd: setParamValue (sustain_, n.y); break;

    case EnvelopeShape::releaseEnd:
        setParamValue (release_, timeRange_.convertFrom0to1 (normInBand (n.x, r0)));
        break;

    case EnvelopeShape::attackCurve:
        setCurveHandle (attackCurveX_, attackCurveY_,
                        EnvelopeShape::handleFromSegmentPoint (
                            corners[EnvelopeShape::start], corners[EnvelopeShape::attackPeak], n));
        break;

    case EnvelopeShape::decayCurve:
        setCurveHandle (decayCurveX_, decayCurveY_,
                        EnvelopeShape::handleFromSegmentPoint (corners[EnvelopeShape::attackPeak],
                                                               corners[EnvelopeShape::decaySustain],
                                                               n));
        break;

    case EnvelopeShape::releaseCurve:
        setCurveHandle (releaseCurveX_, releaseCurveY_,
                        EnvelopeShape::handleFromSegmentPoint (corners[EnvelopeShape::sustainEnd],
                                                               corners[EnvelopeShape::releaseEnd],
                                                               n));
        break;

    default: break;
    }
}

void EnvelopeEditor::resized() {}

void EnvelopeEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setColour (SynthLookAndFeel::background.brighter (0.04f));
    g.fillRoundedRectangle (bounds, 6.0f);
    g.setColour (SynthLookAndFeel::panelBorder);
    g.drawRoundedRectangle (bounds.reduced (0.5f), 6.0f, 1.0f);

    const auto plot = plotBounds();
    if (plot.getWidth() < 8.0f || plot.getHeight() < 8.0f) return;

    {
        const float adr = 1.0f - EnvelopeShape::sustainFraction;
        const float band = adr / 3.0f;
        const float xs[] = {0.0f, band, 2.0f * band, 2.0f * band + EnvelopeShape::sustainFraction,
                            1.0f};
        g.setColour (SynthLookAndFeel::track.withAlpha (0.45f));
        for (int i = 1; i < 4; ++i)
        {
            const float x = plot.getX() + xs[i] * plot.getWidth();
            g.drawVerticalLine (juce::roundToInt (x), plot.getY(), plot.getBottom());
        }
    }

    g.setColour (SynthLookAndFeel::track);
    g.drawHorizontalLine (juce::roundToInt (plot.getBottom()), plot.getX(), plot.getRight());

    const auto params = readParams();
    auto path = EnvelopeShape::buildPath (plot, params, timeRange_);

    {
        juce::Path fill = path;
        fill.lineTo (EnvelopeShape::toPixels ({1.0f, 0.0f}, plot));
        fill.lineTo (EnvelopeShape::toPixels ({0.0f, 0.0f}, plot));
        fill.closeSubPath();
        g.setColour (SynthLookAndFeel::accent.withAlpha (0.12f));
        g.fillPath (fill);
    }

    g.setColour (SynthLookAndFeel::accent);
    g.strokePath (path, juce::PathStrokeType (2.0f, juce::PathStrokeType::curved,
                                              juce::PathStrokeType::rounded));

    const auto pts = EnvelopeShape::buildPoints (params, timeRange_);
    const auto curves = EnvelopeShape::buildCurveHandles (params, timeRange_);
    const int lit = activeHandle_ >= 0 ? activeHandle_ : hoverHandle_;

    auto drawHandle = [&] (int handle, juce::Point<float> c, float radius, bool diamond)
    {
        const bool hot = (handle == lit);
        g.setColour (hot ? SynthLookAndFeel::textPrimary : SynthLookAndFeel::accent);
        if (diamond)
        {
            juce::Path d;
            d.addQuadrilateral (c.x, c.y - radius, c.x + radius, c.y, c.x, c.y + radius,
                                c.x - radius, c.y);
            g.fillPath (d);
            g.setColour (SynthLookAndFeel::panel);
            g.strokePath (d, juce::PathStrokeType (1.2f));
        }
        else
        {
            g.fillEllipse (c.x - radius, c.y - radius, radius * 2.0f, radius * 2.0f);
            g.setColour (SynthLookAndFeel::panel);
            g.drawEllipse (c.x - radius, c.y - radius, radius * 2.0f, radius * 2.0f, 1.2f);
        }
    };

    for (int i = EnvelopeShape::attackPeak; i <= EnvelopeShape::releaseEnd; ++i)
        drawHandle (i, EnvelopeShape::toPixels (pts[static_cast<size_t> (i)], plot), handleRadius,
                    false);

    drawHandle (EnvelopeShape::attackCurve, EnvelopeShape::toPixels (curves[0], plot),
                curveHandleRadius, true);
    drawHandle (EnvelopeShape::decayCurve, EnvelopeShape::toPixels (curves[1], plot),
                curveHandleRadius, true);
    drawHandle (EnvelopeShape::releaseCurve, EnvelopeShape::toPixels (curves[2], plot),
                curveHandleRadius, true);
}
