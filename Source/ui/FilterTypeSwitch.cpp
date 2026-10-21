#include "ui/FilterTypeSwitch.h"

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

FilterTypeSwitch::FilterTypeSwitch()
{
    styleSegment (lowPass_);
    styleSegment (highPass_);
    styleSegment (bandPass_);

    lowPass_.setConnectedEdges (juce::Button::ConnectedOnRight);
    bandPass_.setConnectedEdges (juce::Button::ConnectedOnLeft);

    addAndMakeVisible (lowPass_);
    addAndMakeVisible (highPass_);
    addAndMakeVisible (bandPass_);

    lowPass_.onClick = [this] { setMode (0); };
    highPass_.onClick = [this] { setMode (1); };
    bandPass_.onClick = [this] { setMode (2); };
}

FilterTypeSwitch::~FilterTypeSwitch()
{
    if (param_ != nullptr) param_->removeListener (this);
}

void FilterTypeSwitch::attach (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID)
{
    if (param_ != nullptr) param_->removeListener (this);

    param_ = dynamic_cast<juce::AudioParameterChoice*> (apvts.getParameter (paramID));
    if (param_ != nullptr)
    {
        param_->addListener (this);
        syncFromParameter();
    }
}

void FilterTypeSwitch::setMode (int index)
{
    if (param_ == nullptr) return;

    const float norm = param_->convertTo0to1 (static_cast<float> (index));
    param_->beginChangeGesture();
    param_->setValueNotifyingHost (norm);
    param_->endChangeGesture();
    syncFromParameter();
}

void FilterTypeSwitch::parameterValueChanged (int, float)
{
    juce::MessageManager::callAsync (
        [safe = juce::Component::SafePointer (this)]
        {
            if (safe != nullptr) safe->syncFromParameter();
        });
}

int FilterTypeSwitch::getSelectedIndex() const
{ return param_ != nullptr ? param_->getIndex() : 0; }

void FilterTypeSwitch::syncFromParameter()
{
    if (param_ == nullptr) return;

    const int index = param_->getIndex();
    lowPass_.setToggleState (index == 0, juce::dontSendNotification);
    highPass_.setToggleState (index == 1, juce::dontSendNotification);
    bandPass_.setToggleState (index == 2, juce::dontSendNotification);
}

void FilterTypeSwitch::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced (0.5f);
    g.setColour (SynthLookAndFeel::panelBorder);
    g.drawRoundedRectangle (bounds, 6.0f, 1.0f);
}

void FilterTypeSwitch::resized()
{
    auto r = getLocalBounds().reduced (1);
    const int w = r.getWidth() / 3;
    lowPass_.setBounds (r.removeFromLeft (w));
    highPass_.setBounds (r.removeFromLeft (w));
    bandPass_.setBounds (r);
}
