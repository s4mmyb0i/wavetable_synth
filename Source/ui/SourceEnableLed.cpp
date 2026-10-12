#include "ui/SourceEnableLed.h"

SourceEnableLed::SourceEnableLed()
{
    setMouseCursor (juce::MouseCursor::PointingHandCursor);
    setRepaintsOnMouseActivity (true);
}

SourceEnableLed::~SourceEnableLed()
{
    for (auto* p : {static_cast<juce::AudioProcessorParameter*> (enable_),
                    static_cast<juce::AudioProcessorParameter*> (level_)})
        if (p != nullptr) p->removeListener (this);
}

void SourceEnableLed::attach (juce::AudioProcessorValueTreeState& apvts,
                              const juce::String& enableID, const juce::String& levelID)
{
    for (auto* p : {static_cast<juce::AudioProcessorParameter*> (enable_),
                    static_cast<juce::AudioProcessorParameter*> (level_)})
        if (p != nullptr) p->removeListener (this);

    enable_ = dynamic_cast<juce::AudioParameterBool*> (apvts.getParameter (enableID));
    level_ = dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter (levelID));

    for (auto* p : {static_cast<juce::AudioProcessorParameter*> (enable_),
                    static_cast<juce::AudioProcessorParameter*> (level_)})
        if (p != nullptr) p->addListener (this);

    syncFromParameters();
}

void SourceEnableLed::parameterValueChanged (int, float)
{
    juce::MessageManager::callAsync (
        [safe = juce::Component::SafePointer (this)]
        {
            if (safe != nullptr) safe->syncFromParameters();
        });
}

void SourceEnableLed::syncFromParameters()
{
    const bool wasEnabled = enabled_;
    enabled_ = enable_ == nullptr || enable_->get();
    const float level = level_ != nullptr ? level_->get() : 0.0f;
    active_ = enabled_ && level > 0.001f;
    repaint();

    if (onChange != nullptr && enabled_ != wasEnabled) onChange();
}

void SourceEnableLed::toggle()
{
    if (enable_ == nullptr) return;

    enable_->beginChangeGesture();
    enable_->setValueNotifyingHost (enable_->get() ? 0.0f : 1.0f);
    enable_->endChangeGesture();
    syncFromParameters();
}

void SourceEnableLed::mouseDown (const juce::MouseEvent&)
{ toggle(); }

void SourceEnableLed::mouseEnter (const juce::MouseEvent&)
{
    hover_ = true;
    repaint();
}

void SourceEnableLed::mouseExit (const juce::MouseEvent&)
{
    hover_ = false;
    repaint();
}

void SourceEnableLed::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();
    const float size = juce::jmin (bounds.getWidth(), bounds.getHeight()) - 2.0f;
    const auto led = juce::Rectangle<float> (size, size).withCentre (bounds.getCentre());

    // Bezel
    g.setColour (SynthLookAndFeel::panelBorder);
    g.fillEllipse (led);

    auto inner = led.reduced (2.0f);
    if (! enabled_)
    {
        g.setColour (SynthLookAndFeel::track.darker (0.35f));
        g.fillEllipse (inner);
        return;
    }

    // On: dim when level is 0, bright when contributing.
    const auto colour = active_ ? SynthLookAndFeel::accent : SynthLookAndFeel::accentDim;
    g.setColour (colour);
    g.fillEllipse (inner);

    if (active_)
    {
        g.setColour (colour.withAlpha (0.35f));
        g.fillEllipse (led.expanded (hover_ ? 2.5f : 1.5f));
        g.setColour (colour);
        g.fillEllipse (inner);
    }

    // Specular highlight
    g.setColour (juce::Colours::white.withAlpha (enabled_ ? 0.35f : 0.12f));
    g.fillEllipse (inner.getX() + inner.getWidth() * 0.18f, inner.getY() + inner.getHeight() * 0.12f,
                   inner.getWidth() * 0.35f, inner.getHeight() * 0.28f);
}
