#include "params/ParameterLayout.h"
#include "params/ParamIDs.h"

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Osc mix/detune
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::osc1Level, 1}, "Osc 1 Level",
        juce::NormalisableRange<float> {0.0f, 1.0f, 0.01f}, 1.0f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::osc2Level, 1}, "Osc 2 Level",
        juce::NormalisableRange<float> {0.0f, 1.0f, 0.01f}, 0.0f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::osc3Level, 1}, "Osc 3 Level",
        juce::NormalisableRange<float> {0.0f, 1.0f, 0.01f}, 0.0f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::osc2Detune, 1}, "Osc 2 Detune",
        juce::NormalisableRange<float> {-50.0f, 50.0f, 0.1f}, 7.0f)); // cents

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::osc3Detune, 1}, "Osc 3 Detune",
        juce::NormalisableRange<float> {-50.0f, 50.0f, 0.1f}, -7.0f));

    // Wavetype
    layout.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID {ParamIDs::wavetype, 1}, "Wave",
        juce::StringArray {"Sine", "Saw", "Square", "Triangle"}, 0));

    // Filter
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::cutoff, 1}, "Cutoff",
        juce::NormalisableRange<float> {20.0f, 2000.0f, 0.1f, 0.25f}, 200.0f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::resonance, 1}, "Resonance",
        juce::NormalisableRange<float> {0.0f, 1.0f, 0.01f, 0.25f}, 0.0f));

    // ADSR
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::attack, 1}, "Attack",
        juce::NormalisableRange<float> {0.001f, 5.0f, 0.001f, 0.25f}, 0.01f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::decay, 1}, "Decay",
        juce::NormalisableRange<float> {0.001f, 5.0f, 0.001f, 0.25f}, 0.1f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::sustain, 1}, "Sustain",
        juce::NormalisableRange<float> {0.0f, 1.0f, 0.01f}, 0.8f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::release, 1}, "Release",
        juce::NormalisableRange<float> {0.001f, 5.0f, 0.001f, 0.25f}, 0.2f));

    return layout;
}
