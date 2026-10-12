#include "ui/EnvSourceSwitch.h"

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

EnvSourceSwitch::EnvSourceSwitch()
{
    styleSegment (osc1_);
    styleSegment (osc2_);
    styleSegment (osc3_);
    styleSegment (noise_);

    osc1_.setConnectedEdges (juce::Button::ConnectedOnRight);
    osc2_.setConnectedEdges (juce::Button::ConnectedOnLeft | juce::Button::ConnectedOnRight);
    osc3_.setConnectedEdges (juce::Button::ConnectedOnLeft | juce::Button::ConnectedOnRight);
    noise_.setConnectedEdges (juce::Button::ConnectedOnLeft);

    addAndMakeVisible (osc1_);
    addAndMakeVisible (osc2_);
    addAndMakeVisible (osc3_);
    addAndMakeVisible (noise_);

    osc1_.onClick = [this] { select (0); };
    osc2_.onClick = [this] { select (1); };
    osc3_.onClick = [this] { select (2); };
    noise_.onClick = [this] { select (3); };

    setSelectedIndex (0);
}

void EnvSourceSwitch::setSelectedIndex (int index)
{
    selected_ = juce::jlimit (0, 3, index);
    osc1_.setToggleState (selected_ == 0, juce::dontSendNotification);
    osc2_.setToggleState (selected_ == 1, juce::dontSendNotification);
    osc3_.setToggleState (selected_ == 2, juce::dontSendNotification);
    noise_.setToggleState (selected_ == 3, juce::dontSendNotification);
}

void EnvSourceSwitch::select (int index)
{
    if (index == selected_)
    {
        setSelectedIndex (index); // keep radio exclusivity if user re-clicks
        return;
    }

    setSelectedIndex (index);
    if (onChange != nullptr) onChange();
}

void EnvSourceSwitch::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced (0.5f);
    g.setColour (SynthLookAndFeel::panelBorder);
    g.drawRoundedRectangle (bounds, 6.0f, 1.0f);
}

void EnvSourceSwitch::resized()
{
    auto r = getLocalBounds().reduced (1);
    const int w = r.getWidth() / 4;
    osc1_.setBounds (r.removeFromLeft (w));
    osc2_.setBounds (r.removeFromLeft (w));
    osc3_.setBounds (r.removeFromLeft (w));
    noise_.setBounds (r);
}
