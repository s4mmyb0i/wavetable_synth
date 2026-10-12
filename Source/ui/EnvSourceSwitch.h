#pragma once

#include "ui/SynthLookAndFeel.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

// UI-only source picker for the envelope section: Osc 1 | Osc 2 | Osc 3 | Noise
class EnvSourceSwitch final : public juce::Component
{
public:
    EnvSourceSwitch();

    void paint (juce::Graphics& g) override;
    void resized() override;

    int getSelectedIndex() const { return selected_; }
    void setSelectedIndex (int index);

    std::function<void()> onChange;

private:
    void select (int index);

    juce::TextButton osc1_ {"Osc 1"};
    juce::TextButton osc2_ {"Osc 2"};
    juce::TextButton osc3_ {"Osc 3"};
    juce::TextButton noise_ {"Noise"};
    int selected_ = 0;
};
