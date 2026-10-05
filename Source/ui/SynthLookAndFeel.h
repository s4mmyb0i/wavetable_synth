#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

// Compact dark instrument look: charcoal panel, warm copper accent.
class SynthLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    SynthLookAndFeel();

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle,
                           float rotaryEndAngle, juce::Slider& slider) override;

    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           float minSliderPos, float maxSliderPos, juce::Slider::SliderStyle style,
                           juce::Slider& slider) override;

    void drawComboBox (juce::Graphics& g, int width, int height, bool isButtonDown, int buttonX,
                       int buttonY, int buttonW, int buttonH, juce::ComboBox& box) override;

    void drawPopupMenuBackground (juce::Graphics& g, int width, int height) override;

    juce::Font getComboBoxFont (juce::ComboBox&) override;
    juce::Font getPopupMenuFont() override;
    juce::Font getLabelFont (juce::Label&) override;

    static const juce::Colour background;
    static const juce::Colour panel;
    static const juce::Colour panelBorder;
    static const juce::Colour accent;
    static const juce::Colour accentDim;
    static const juce::Colour textPrimary;
    static const juce::Colour textMuted;
    static const juce::Colour track;
};
