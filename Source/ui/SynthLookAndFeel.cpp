#include "ui/SynthLookAndFeel.h"

const juce::Colour SynthLookAndFeel::background {0xff141618};
const juce::Colour SynthLookAndFeel::panel {0xff1c2024};
const juce::Colour SynthLookAndFeel::panelBorder {0xff2a3036};
const juce::Colour SynthLookAndFeel::accent {0xffd4a574};
const juce::Colour SynthLookAndFeel::accentDim {0xff8a6b4a};
const juce::Colour SynthLookAndFeel::textPrimary {0xffe8eaed};
const juce::Colour SynthLookAndFeel::textMuted {0xff8b929a};
const juce::Colour SynthLookAndFeel::track {0xff3a424a};

SynthLookAndFeel::SynthLookAndFeel()
{
    setColour (juce::ResizableWindow::backgroundColourId, background);
    setColour (juce::Slider::textBoxTextColourId, textPrimary);
    setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    setColour (juce::ComboBox::backgroundColourId, panel);
    setColour (juce::ComboBox::outlineColourId, panelBorder);
    setColour (juce::ComboBox::textColourId, textPrimary);
    setColour (juce::ComboBox::arrowColourId, accent);
    setColour (juce::PopupMenu::backgroundColourId, panel);
    setColour (juce::PopupMenu::textColourId, textPrimary);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, accentDim);
    setColour (juce::PopupMenu::highlightedTextColourId, textPrimary);
    setColour (juce::Label::textColourId, textMuted);
    setColour (juce::CaretComponent::caretColourId, accent);
}

void SynthLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                         float sliderPos, float rotaryStartAngle,
                                         float rotaryEndAngle, juce::Slider& slider)
{
    juce::ignoreUnused (slider);

    const auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (6.0f);
    const auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
    const auto centre = bounds.getCentre();
    const auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    const auto lineW = juce::jmax (2.0f, radius * 0.12f);
    const auto arcRadius = radius - lineW * 0.5f;

    juce::Path backgroundArc;
    backgroundArc.addCentredArc (centre.x, centre.y, arcRadius, arcRadius, 0.0f, rotaryStartAngle,
                                 rotaryEndAngle, true);
    g.setColour (track);
    g.strokePath (backgroundArc, juce::PathStrokeType (lineW, juce::PathStrokeType::curved,
                                                       juce::PathStrokeType::rounded));

    if (slider.isEnabled())
    {
        juce::Path valueArc;
        valueArc.addCentredArc (centre.x, centre.y, arcRadius, arcRadius, 0.0f, rotaryStartAngle,
                                toAngle, true);
        g.setColour (accent);
        g.strokePath (valueArc, juce::PathStrokeType (lineW, juce::PathStrokeType::curved,
                                                      juce::PathStrokeType::rounded));
    }

    // Thumb
    const auto thumbRadius = lineW * 1.1f;
    const auto thumbPoint = centre.getPointOnCircumference (arcRadius, toAngle);
    g.setColour (textPrimary);
    g.fillEllipse (
        juce::Rectangle<float> (thumbRadius * 2.0f, thumbRadius * 2.0f).withCentre (thumbPoint));
}

void SynthLookAndFeel::drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                                         float sliderPos, float minSliderPos, float maxSliderPos,
                                         juce::Slider::SliderStyle style, juce::Slider& slider)
{
    if (! slider.isVertical() ||
        (style != juce::Slider::LinearVertical && style != juce::Slider::LinearBarVertical))
    {
        LookAndFeel_V4::drawLinearSlider (g, x, y, width, height, sliderPos, minSliderPos,
                                          maxSliderPos, style, slider);
        return;
    }

    juce::ignoreUnused (minSliderPos, maxSliderPos);

    auto bounds = juce::Rectangle<float> (static_cast<float> (x), static_cast<float> (y),
                                          static_cast<float> (width), static_cast<float> (height))
                      .reduced (static_cast<float> (width) * 0.35f, 4.0f);

    const float trackW = juce::jmax (4.0f, bounds.getWidth());
    auto trackBounds = bounds.withSizeKeepingCentre (trackW, bounds.getHeight());

    g.setColour (SynthLookAndFeel::track);
    g.fillRoundedRectangle (trackBounds, trackW * 0.5f);

    // Fill from bottom up to the thumb (JUCE vertical: higher value → smaller Y).
    const float thumbY = sliderPos;
    auto fill =
        trackBounds.withTop (juce::jlimit (trackBounds.getY(), trackBounds.getBottom(), thumbY));
    g.setColour (accent);
    g.fillRoundedRectangle (fill, trackW * 0.5f);

    const float thumbH = 10.0f;
    const float thumbW = trackW + 10.0f;
    auto thumb =
        juce::Rectangle<float> (thumbW, thumbH).withCentre ({trackBounds.getCentreX(), thumbY});
    g.setColour (textPrimary);
    g.fillRoundedRectangle (thumb, 3.0f);
}

void SynthLookAndFeel::drawComboBox (juce::Graphics& g, int width, int height, bool, int, int, int,
                                     int, juce::ComboBox& box)
{
    auto bounds = juce::Rectangle<int> (0, 0, width, height).toFloat().reduced (0.5f);
    g.setColour (box.findColour (juce::ComboBox::backgroundColourId));
    g.fillRoundedRectangle (bounds, 6.0f);
    g.setColour (box.findColour (juce::ComboBox::outlineColourId));
    g.drawRoundedRectangle (bounds, 6.0f, 1.0f);

    const auto arrowZone = bounds.removeFromRight (24.0f).reduced (6.0f, 10.0f);
    juce::Path arrow;
    arrow.addTriangle (arrowZone.getX(), arrowZone.getY(), arrowZone.getRight(), arrowZone.getY(),
                       arrowZone.getCentreX(), arrowZone.getBottom());
    g.setColour (box.findColour (juce::ComboBox::arrowColourId));
    g.fillPath (arrow);
}

void SynthLookAndFeel::drawPopupMenuBackground (juce::Graphics& g, int width, int height)
{
    g.fillAll (panel);
    g.setColour (panelBorder);
    g.drawRect (0, 0, width, height, 1);
}

juce::Font SynthLookAndFeel::getComboBoxFont (juce::ComboBox&)
{ return juce::FontOptions (14.0f); }

juce::Font SynthLookAndFeel::getPopupMenuFont()
{ return juce::FontOptions (14.0f); }

juce::Font SynthLookAndFeel::getLabelFont (juce::Label&)
{ return juce::FontOptions (12.0f); }
