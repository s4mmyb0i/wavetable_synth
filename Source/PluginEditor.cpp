#include "PluginProcessor.h"
#include "PluginEditor.h"

WavetableSynthAudioProcessorEditor::WavetableSynthAudioProcessorEditor (WavetableSynthAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);
    setSize (400, 240);
}

WavetableSynthAudioProcessorEditor::~WavetableSynthAudioProcessorEditor() = default;

void WavetableSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour (juce::Colours::white);
    g.setFont (16.0f);
    g.drawFittedText ("Wavetable Synth (silent shell)",
                      getLocalBounds(), juce::Justification::centred, 1);
}

void WavetableSynthAudioProcessorEditor::resized() {}
