#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "params/ParamIDs.h"

WavetableSynthAudioProcessorEditor::WavetableSynthAudioProcessorEditor (WavetableSynthAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);

    wavetypeBox.addItemList (juce::StringArray { "Sine", "Saw", "Square", "Triangle"}, 1);

    wavetypeLabel.setText ("Wave", juce::dontSendNotification);
    wavetypeLabel.attachToComponent (&wavetypeBox, true);
    addAndMakeVisible (wavetypeBox);
    addAndMakeVisible (wavetypeLabel);
    
    setupSlider (attackSlider   , attackLabel   , "Attack");
    setupSlider (decaySlider    , decayLabel    , "Decay");
    setupSlider (sustainSlider  , sustainLabel  , "Sustain");
    setupSlider (releaseSlider  , releaseLabel  , "Release");
    
    auto& apvts = processorRef.getAPVTS();
    
    wavetypeAttachment = std::make_unique<ComboBoxAttachment> (
        apvts, ParamIDs::wavetype, wavetypeBox);
    
    attackAttachment  = std::make_unique<SliderAttachment> (apvts, ParamIDs::attack,  attackSlider);
    decayAttachment   = std::make_unique<SliderAttachment> (apvts, ParamIDs::decay,   decaySlider);
    sustainAttachment = std::make_unique<SliderAttachment> (apvts, ParamIDs::sustain, sustainSlider);
    releaseAttachment = std::make_unique<SliderAttachment> (apvts, ParamIDs::release, releaseSlider);

    setSize (400, 240);
}

WavetableSynthAudioProcessorEditor::~WavetableSynthAudioProcessorEditor() = default;


void WavetableSynthAudioProcessorEditor::setupSlider (juce::Slider& slider, juce::Label& label,
                                                      const juce::String& name)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 18);
    addAndMakeVisible (slider);

    label.setText (name, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.attachToComponent (&slider, false);
    addAndMakeVisible (label);
}


void WavetableSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour (juce::Colours::white);
    g.setFont (16.0f);
}

void WavetableSynthAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (20);

    auto top = area.removeFromTop (32);
    wavetypeBox.setBounds (top.removeFromLeft (180));

    area.removeFromTop (24); // space for rotary labels

    const int w = area.getWidth() / 4;
    attackSlider.setBounds  (area.removeFromLeft (w).reduced (8));
    decaySlider.setBounds   (area.removeFromLeft (w).reduced (8));
    sustainSlider.setBounds (area.removeFromLeft (w).reduced (8));
    releaseSlider.setBounds (area.reduced (8));
}