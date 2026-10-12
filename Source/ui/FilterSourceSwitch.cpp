#include "ui/FilterSourceSwitch.h"

namespace
{
void styleSegment (juce::TextButton& b)
{
    b.setClickingTogglesState (true);
    b.setColour (juce::TextButton::buttonColourId, SynthLookAndFeel::track);
    b.setColour (juce::TextButton::buttonOnColourId, SynthLookAndFeel::accentDim);
    b.setColour (juce::TextButton::textColourOffId, SynthLookAndFeel::textMuted);
    b.setColour (juce::TextButton::textColourOnId, SynthLookAndFeel::textPrimary);
}
} // namespace

FilterSourceSwitch::FilterSourceSwitch()
{
    styleSegment (osc_);
    styleSegment (noise_);

    osc_.setConnectedEdges (juce::Button::ConnectedOnRight);
    noise_.setConnectedEdges (juce::Button::ConnectedOnLeft);

    addAndMakeVisible (osc_);
    addAndMakeVisible (noise_);

    osc_.onClick = [this] { select (0); };
    noise_.onClick = [this] { select (1); };

    setSelectedIndex (0);
}

void FilterSourceSwitch::setSelectedIndex (int index)
{
    selected_ = juce::jlimit (0, 1, index);
    osc_.setToggleState (selected_ == 0, juce::dontSendNotification);
    noise_.setToggleState (selected_ == 1, juce::dontSendNotification);
}

void FilterSourceSwitch::select (int index)
{
    if (index == selected_)
    {
        setSelectedIndex (index);
        return;
    }

    setSelectedIndex (index);
    if (onChange != nullptr) onChange();
}

void FilterSourceSwitch::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced (0.5f);
    g.setColour (SynthLookAndFeel::panelBorder);
    g.drawRoundedRectangle (bounds, 6.0f, 1.0f);
}

void FilterSourceSwitch::resized()
{
    auto r = getLocalBounds().reduced (1);
    const int w = r.getWidth() / 2;
    osc_.setBounds (r.removeFromLeft (w));
    noise_.setBounds (r);
}
