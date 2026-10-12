#pragma once

#include "ui/SynthLookAndFeel.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

// UI-only picker for the filter section: Osc 1-3 (shared bus) | Noise
class FilterSourceSwitch final : public juce::Component
{
public:
    FilterSourceSwitch();

    void paint (juce::Graphics& g) override;
    void resized() override;

    int getSelectedIndex() const { return selected_; }
    void setSelectedIndex (int index);

    std::function<void()> onChange;

private:
    void select (int index);

    juce::TextButton osc_ {"Osc 1-3"};
    juce::TextButton noise_ {"Noise"};
    int selected_ = 0;
};
