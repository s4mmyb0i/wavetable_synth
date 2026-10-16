#pragma once

#include "PluginProcessor.h"
#include "ui/SynthLookAndFeel.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <array>

// Header CPU readout: % label, fill bar, and a short history sparkline.
class CpuMeter final : public juce::Component, private juce::Timer
{
public:
    explicit CpuMeter (WavetableSynthAudioProcessor& processor);
    ~CpuMeter() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;

    WavetableSynthAudioProcessor& processor_;

    static constexpr int historySize = 64;
    std::array<float, historySize> history_ {};
    int historyWrite_ = 0;
    int historyCount_ = 0;
    float currentLoad_ = 0.0f; // 0..1
};
