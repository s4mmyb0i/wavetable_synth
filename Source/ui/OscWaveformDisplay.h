#pragma once

#include "dsp/WarpUtils.h"
#include "ui/SynthLookAndFeel.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class WavetableSynthAudioProcessor;

// One-cycle preview of an osc: morph/warp applied, unison as dimmer ghost traces.
class OscWaveformDisplay final : public juce::Component,
                                 private juce::AudioProcessorParameter::Listener
{
public:
    OscWaveformDisplay();
    ~OscWaveformDisplay() override;

    void attach (WavetableSynthAudioProcessor& processor,
                 juce::AudioProcessorValueTreeState& apvts, const juce::String& waveID,
                 const juce::String& positionID, const juce::String& warpModeID,
                 const juce::String& warpAmountID, const juce::String& unisonCountID,
                 const juce::String& unisonDetuneID);

    void paint (juce::Graphics& g) override;

private:
    void parameterValueChanged (int parameterIndex, float newValue) override;
    void parameterGestureChanged (int, bool) override {}

    void listen (juce::AudioProcessorParameter*& slot, juce::AudioProcessorParameter* param);
    float lookupSample (float phase01) const;
    void drawVoice (juce::Graphics& g, juce::Rectangle<float> plot, int voiceIndex, int uniCount,
                    float detuneCents, float alpha, float stroke) const;

    WavetableSynthAudioProcessor* processor_ = nullptr;

    juce::AudioProcessorParameter* wave_ = nullptr;
    juce::AudioProcessorParameter* position_ = nullptr;
    juce::AudioProcessorParameter* warpMode_ = nullptr;
    juce::AudioProcessorParameter* warpAmount_ = nullptr;
    juce::AudioProcessorParameter* unisonCount_ = nullptr;
    juce::AudioProcessorParameter* unisonDetune_ = nullptr;

    static constexpr int maxUnison = 8;
    static constexpr int numPlotPoints = 128;
};
