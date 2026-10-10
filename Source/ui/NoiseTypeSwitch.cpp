#include "ui/NoiseTypeSwitch.h"

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

NoiseTypeSwitch::NoiseTypeSwitch()
{
    styleSegment (white_);
    styleSegment (pink_);

    white_.setConnectedEdges (juce::Button::ConnectedOnRight);
    pink_.setConnectedEdges (juce::Button::ConnectedOnLeft);

    addAndMakeVisible (white_);
    addAndMakeVisible (pink_);

    white_.onClick = [this] { setMode (0); };
    pink_.onClick = [this] { setMode (1); };
}

NoiseTypeSwitch::~NoiseTypeSwitch()
{
    if (param_ != nullptr)
        param_->removeListener (this);
}

void NoiseTypeSwitch::attach (juce::AudioProcessorValueTreeState& apvts,
                              const juce::String& paramID)
{
    if (param_ != nullptr)
        param_->removeListener (this);

    param_ = dynamic_cast<juce::AudioParameterChoice*> (apvts.getParameter (paramID));
    if (param_ != nullptr)
    {
        param_->addListener (this);
        syncFromParameter();
    }
}

void NoiseTypeSwitch::setMode (int index)
{
    if (param_ == nullptr) return;

    const float norm = param_->convertTo0to1 (static_cast<float> (index));
    param_->beginChangeGesture();
    param_->setValueNotifyingHost (norm);
    param_->endChangeGesture();
    syncFromParameter();
}

void NoiseTypeSwitch::parameterValueChanged (int, float)
{
    juce::MessageManager::callAsync ([safe = juce::Component::SafePointer (this)]
                                     {
                                         if (safe != nullptr)
                                             safe->syncFromParameter();
                                     });
}

void NoiseTypeSwitch::syncFromParameter()
{
    if (param_ == nullptr) return;

    const int index = param_->getIndex();
    white_.setToggleState (index == 0, juce::dontSendNotification);
    pink_.setToggleState (index == 1, juce::dontSendNotification);
}

void NoiseTypeSwitch::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced (0.5f);
    g.setColour (SynthLookAndFeel::panelBorder);
    g.drawRoundedRectangle (bounds, 6.0f, 1.0f);
}

void NoiseTypeSwitch::resized()
{
    auto r = getLocalBounds().reduced (1);
    const int w = r.getWidth() / 2;
    white_.setBounds (r.removeFromLeft (w));
    pink_.setBounds (r);
}
