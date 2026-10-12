#include "params/ParameterLayout.h"
#include "params/ParamIDs.h"

namespace
{
juce::StringArray waveChoices()
{ return juce::StringArray {"Sine", "Saw", "Square", "Triangle", "Morph"}; }

juce::StringArray warpChoices()
{ return juce::StringArray {"Off", "Bend+", "Sync"}; } // matches WarpMode order

juce::StringArray noiseChoices()
{ return juce::StringArray {"White", "Pink"}; } // matches NoiseGenerator::Type
} // namespace

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

    const auto centsAttrs = juce::AudioParameterFloatAttributes().withLabel ("ct");

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::osc2Detune, 1}, "Osc 2 Detune",
        juce::NormalisableRange<float> {-50.0f, 50.0f, 0.1f}, 7.0f, centsAttrs));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::osc3Detune, 1}, "Osc 3 Detune",
        juce::NormalisableRange<float> {-50.0f, 50.0f, 0.1f}, -7.0f, centsAttrs));

    // Per-osc unison
    layout.add (std::make_unique<juce::AudioParameterInt> (
        juce::ParameterID {ParamIDs::osc1UnisonCount, 1}, "Osc 1 Unison", 1, 8, 1));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::osc1UnisonDetune, 1}, "Osc 1 Uni Detune",
        juce::NormalisableRange<float> {0.0f, 50.0f, 0.1f}, 7.0f, centsAttrs));

    layout.add (std::make_unique<juce::AudioParameterInt> (
        juce::ParameterID {ParamIDs::osc2UnisonCount, 1}, "Osc 2 Unison", 1, 8, 1));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::osc2UnisonDetune, 1}, "Osc 2 Uni Detune",
        juce::NormalisableRange<float> {0.0f, 50.0f, 0.1f}, 7.0f, centsAttrs));

    layout.add (std::make_unique<juce::AudioParameterInt> (
        juce::ParameterID {ParamIDs::osc3UnisonCount, 1}, "Osc 3 Unison", 1, 8, 1));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::osc3UnisonDetune, 1}, "Osc 3 Uni Detune",
        juce::NormalisableRange<float> {0.0f, 50.0f, 0.1f}, 7.0f, centsAttrs));

    // Per-osc wavetype (4 = Morph bank)
    layout.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID {ParamIDs::osc1Wave, 1}, "Osc 1 Wave", waveChoices(), 4)); // Morph
    layout.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID {ParamIDs::osc2Wave, 1}, "Osc 2 Wave", waveChoices(), 2)); // Square
    layout.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID {ParamIDs::osc3Wave, 1}, "Osc 3 Wave", waveChoices(), 0)); // Sine

    // Per-osc wavetable position
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::osc1Position, 1}, "Osc 1 Position",
        juce::NormalisableRange<float> {0.0f, 1.0f, 0.01f}, 0.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::osc2Position, 1}, "Osc 2 Position",
        juce::NormalisableRange<float> {0.0f, 1.0f, 0.01f}, 0.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::osc3Position, 1}, "Osc 3 Position",
        juce::NormalisableRange<float> {0.0f, 1.0f, 0.01f}, 0.0f));

    // Per-osc warp
    layout.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID {ParamIDs::osc1WarpMode, 1}, "Osc 1 Warp", warpChoices(), 0));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::osc1WarpAmount, 1}, "Osc 1 Warp Amt",
        juce::NormalisableRange<float> {0.0f, 1.0f, 0.01f}, 0.0f));

    layout.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID {ParamIDs::osc2WarpMode, 1}, "Osc 2 Warp", warpChoices(), 0));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::osc2WarpAmount, 1}, "Osc 2 Warp Amt",
        juce::NormalisableRange<float> {0.0f, 1.0f, 0.01f}, 0.0f));

    layout.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID {ParamIDs::osc3WarpMode, 1}, "Osc 3 Warp", warpChoices(), 0));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::osc3WarpAmount, 1}, "Osc 3 Warp Amt",
        juce::NormalisableRange<float> {0.0f, 1.0f, 0.01f}, 0.0f));

    // Per-osc / noise pan (−1 = left, 0 = centre, +1 = right)
    const auto panAttrs = juce::AudioParameterFloatAttributes().withLabel ("L/R");
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::osc1Pan, 1}, "Osc 1 Pan",
        juce::NormalisableRange<float> {-1.0f, 1.0f, 0.01f}, 0.0f, panAttrs));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::osc2Pan, 1}, "Osc 2 Pan",
        juce::NormalisableRange<float> {-1.0f, 1.0f, 0.01f}, 0.0f, panAttrs));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::osc3Pan, 1}, "Osc 3 Pan",
        juce::NormalisableRange<float> {-1.0f, 1.0f, 0.01f}, 0.0f, panAttrs));

    // Noise generator
    layout.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID {ParamIDs::noiseType, 1}, "Noise Type", noiseChoices(), 0));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::noiseLevel, 1}, "Noise Level",
        juce::NormalisableRange<float> {0.0f, 1.0f, 0.01f}, 0.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::noisePan, 1}, "Noise Pan",
        juce::NormalisableRange<float> {-1.0f, 1.0f, 0.01f}, 0.0f, panAttrs));

    // Filter
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::cutoff, 1}, "Cutoff",
        juce::NormalisableRange<float> {20.0f, 2000.0f, 0.1f, 0.25f}, 200.0f,
        juce::AudioParameterFloatAttributes().withLabel ("Hz")));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {ParamIDs::resonance, 1}, "Resonance",
        juce::NormalisableRange<float> {0.0f, 1.0f, 0.01f, 0.25f}, 0.0f));

    // Per-source ADSR
    const auto secondsAttrs = juce::AudioParameterFloatAttributes().withLabel ("s");
    const auto timeRange = juce::NormalisableRange<float> {0.001f, 5.0f, 0.001f, 0.25f};
    const auto sustainRange = juce::NormalisableRange<float> {0.0f, 1.0f, 0.01f};

    auto addAdsr = [&] (const char* attackID, const char* decayID, const char* sustainID,
                        const char* releaseID, const juce::String& prefix)
    {
        layout.add (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID {attackID, 1}, prefix + " Attack", timeRange, 0.01f, secondsAttrs));
        layout.add (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID {decayID, 1}, prefix + " Decay", timeRange, 0.1f, secondsAttrs));
        layout.add (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID {sustainID, 1}, prefix + " Sustain", sustainRange, 0.8f));
        layout.add (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID {releaseID, 1}, prefix + " Release", timeRange, 0.2f, secondsAttrs));
    };

    addAdsr (ParamIDs::osc1Attack, ParamIDs::osc1Decay, ParamIDs::osc1Sustain, ParamIDs::osc1Release,
             "Osc 1");
    addAdsr (ParamIDs::osc2Attack, ParamIDs::osc2Decay, ParamIDs::osc2Sustain, ParamIDs::osc2Release,
             "Osc 2");
    addAdsr (ParamIDs::osc3Attack, ParamIDs::osc3Decay, ParamIDs::osc3Sustain, ParamIDs::osc3Release,
             "Osc 3");
    addAdsr (ParamIDs::noiseAttack, ParamIDs::noiseDecay, ParamIDs::noiseSustain,
             ParamIDs::noiseRelease, "Noise");

    return layout;
}
