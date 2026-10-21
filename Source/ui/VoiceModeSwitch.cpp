#include "ui/VoiceModeSwitch.h"

namespace
{
void styleSegment (juce::TextButton& b)
{
    b.setClickingTogglesState (true);
    b.setColour (juce::TextButton::buttonColourId, SynthLookAndFeel::track);
    b.setColour (juce::TextButton::buttonOnColourId, SynthLookAndFeel::accentDim);
    b.setColour (juce::TextButton::textColourOffId, SynthLookAndFeel::textMuted);
    b.setColour (juce::TextButton::textColourOnId, SynthLookAndFeel::textPrimary);
    b.setConnectedEdges (juce::Button::ConnectedOnLeft | juce::Button::ConnectedOnRight);
}
} // namespace

VoiceModeSwitch::VoiceModeSwitch()
{
    styleSegment (poly_);
    styleSegment (mono_);
    styleSegment (legato_);

    poly_.setConnectedEdges (juce::Button::ConnectedOnRight);
    legato_.setConnectedEdges (juce::Button::ConnectedOnLeft);

    addAndMakeVisible (poly_);
    addAndMakeVisible (mono_);
    addAndMakeVisible (legato_);

    poly_.onClick = [this] { setMode (0); };
    mono_.onClick = [this] { setMode (1); };
    legato_.onClick = [this] { setMode (2); };
}

VoiceModeSwitch::~VoiceModeSwitch()
{
    if (param_ != nullptr) param_->removeListener (this);
}

void VoiceModeSwitch::attach (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID)
{
    if (param_ != nullptr) param_->removeListener (this);

    param_ = dynamic_cast<juce::AudioParameterChoice*> (apvts.getParameter (paramID));
    if (param_ != nullptr)
    {
        param_->addListener (this);
        syncFromParameter();
    }
}

void VoiceModeSwitch::setMode (int index)
{
    if (param_ == nullptr) return;

    const float norm = param_->convertTo0to1 (static_cast<float> (index));
    param_->beginChangeGesture();
    param_->setValueNotifyingHost (norm);
    param_->endChangeGesture();
    syncFromParameter();
}

void VoiceModeSwitch::parameterValueChanged (int, float)
{
    juce::MessageManager::callAsync (
        [safe = juce::Component::SafePointer (this)]
        {
            if (safe != nullptr) safe->syncFromParameter();
        });
}

int VoiceModeSwitch::getSelectedIndex() const
{ return param_ != nullptr ? param_->getIndex() : 0; }

void VoiceModeSwitch::syncFromParameter()
{
    if (param_ == nullptr) return;

    const int index = param_->getIndex();
    poly_.setToggleState (index == 0, juce::dontSendNotification);
    mono_.setToggleState (index == 1, juce::dontSendNotification);
    legato_.setToggleState (index == 2, juce::dontSendNotification);
}

void VoiceModeSwitch::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced (0.5f);
    g.setColour (SynthLookAndFeel::panelBorder);
    g.drawRoundedRectangle (bounds, 6.0f, 1.0f);
}

void VoiceModeSwitch::resized()
{
    auto r = getLocalBounds().reduced (1);
    const int w = r.getWidth() / 3;
    poly_.setBounds (r.removeFromLeft (w));
    mono_.setBounds (r.removeFromLeft (w));
    legato_.setBounds (r);
}
