#include "ui/WarpModeSwitch.h"

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

WarpModeSwitch::WarpModeSwitch()
{
    styleSegment (off_);
    styleSegment (bend_);
    styleSegment (sync_);

    off_.setConnectedEdges (juce::Button::ConnectedOnRight);
    sync_.setConnectedEdges (juce::Button::ConnectedOnLeft);

    addAndMakeVisible (off_);
    addAndMakeVisible (bend_);
    addAndMakeVisible (sync_);

    off_.onClick = [this] { setMode (0); };
    bend_.onClick = [this] { setMode (1); };
    sync_.onClick = [this] { setMode (2); };
}

WarpModeSwitch::~WarpModeSwitch()
{
    if (param_ != nullptr) param_->removeListener (this);
}

void WarpModeSwitch::attach (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID)
{
    if (param_ != nullptr) param_->removeListener (this);

    param_ = dynamic_cast<juce::AudioParameterChoice*> (apvts.getParameter (paramID));
    if (param_ != nullptr)
    {
        param_->addListener (this);
        syncFromParameter();
    }
}

void WarpModeSwitch::setMode (int index)
{
    if (param_ == nullptr) return;

    const float norm = param_->convertTo0to1 (static_cast<float> (index));
    param_->beginChangeGesture();
    param_->setValueNotifyingHost (norm);
    param_->endChangeGesture();
    syncFromParameter();
}

void WarpModeSwitch::parameterValueChanged (int, float)
{
    juce::MessageManager::callAsync (
        [safe = juce::Component::SafePointer (this)]
        {
            if (safe != nullptr) safe->syncFromParameter();
        });
}

void WarpModeSwitch::syncFromParameter()
{
    if (param_ == nullptr) return;

    const int index = param_->getIndex();
    off_.setToggleState (index == 0, juce::dontSendNotification);
    bend_.setToggleState (index == 1, juce::dontSendNotification);
    sync_.setToggleState (index == 2, juce::dontSendNotification);
}

void WarpModeSwitch::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced (0.5f);
    g.setColour (SynthLookAndFeel::panelBorder);
    g.drawRoundedRectangle (bounds, 6.0f, 1.0f);
}

void WarpModeSwitch::resized()
{
    auto r = getLocalBounds().reduced (1);
    const int w = r.getWidth() / 3;
    off_.setBounds (r.removeFromLeft (w));
    bend_.setBounds (r.removeFromLeft (w));
    sync_.setBounds (r);
}
