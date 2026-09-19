#include "params/ParameterLayout.h"
#include "params/ParamIDs.h"

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID {ParamIDs::wavetype, 1},
        "Wave",
        juce::StringArray { "Sine", "Saw", "Square" },
        0
    ));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::attack, 1},
        "Attack",
        juce::NormalisableRange<float> { 0.001f, 5.0f, 0.001f, 0.25f} ,
        0.01f
    ));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::decay, 1},
        "Decay",
        juce::NormalisableRange<float> { 0.001f, 5.0f, 0.001f, 0.25f },
        0.1f
    ));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::sustain, 1},
        "Sustain",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f },
        0.8f
    ));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::release, 1},
        "Release",
        juce::NormalisableRange<float> { 0.001f, 5.0f, 0.001f, 0.25f },
        0.2f
    ));

    return layout;
}