#include "ui/CpuMeter.h"

CpuMeter::CpuMeter (WavetableSynthAudioProcessor& processor) : processor_ (processor)
{
    startTimerHz (12);
}

CpuMeter::~CpuMeter()
{ stopTimer(); }

void CpuMeter::timerCallback()
{
    currentLoad_ = static_cast<float> (processor_.getCpuLoad());
    history_[static_cast<size_t> (historyWrite_)] = currentLoad_;
    historyWrite_ = (historyWrite_ + 1) % historySize;
    historyCount_ = juce::jmin (historyCount_ + 1, historySize);
    repaint();
}

void CpuMeter::resized() {}

void CpuMeter::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setColour (SynthLookAndFeel::panel);
    g.fillRoundedRectangle (bounds, 6.0f);
    g.setColour (SynthLookAndFeel::panelBorder);
    g.drawRoundedRectangle (bounds.reduced (0.5f), 6.0f, 1.0f);

    auto inner = bounds.reduced (8.0f, 6.0f);

    auto labelRow = inner.removeFromTop (14.0f);
    g.setColour (SynthLookAndFeel::textMuted);
    g.setFont (juce::FontOptions (11.0f, juce::Font::bold));
    g.drawText ("CPU", labelRow.removeFromLeft (28.0f), juce::Justification::centredLeft, false);

    const int pct = juce::roundToInt (currentLoad_ * 100.0f);
    g.setColour (currentLoad_ > 0.85f ? juce::Colour (0xffe07070) : SynthLookAndFeel::textPrimary);
    g.drawText (juce::String (pct) + " %", labelRow, juce::Justification::centredRight, false);

    inner.removeFromTop (3.0f);

    // Instant bar
    auto barArea = inner.removeFromBottom (6.0f);
    g.setColour (SynthLookAndFeel::track);
    g.fillRoundedRectangle (barArea, 2.0f);
    auto fill = barArea.withWidth (barArea.getWidth() * juce::jlimit (0.0f, 1.0f, currentLoad_));
    g.setColour (currentLoad_ > 0.85f ? juce::Colour (0xffe07070) : SynthLookAndFeel::accent);
    g.fillRoundedRectangle (fill, 2.0f);

    inner.removeFromBottom (4.0f);

    // History sparkline (oldest → newest left → right)
    if (historyCount_ < 2 || inner.getHeight() < 4.0f) return;

    juce::Path path;
    const float w = inner.getWidth();
    const float h = inner.getHeight();
    const int n = historyCount_;

    auto sampleAt = [this, n] (int i) -> float
    {
        // i = 0 is oldest among the filled slots
        const int idx = (historyWrite_ - n + i + historySize * 2) % historySize;
        return history_[static_cast<size_t> (idx)];
    };

    for (int i = 0; i < n; ++i)
    {
        const float x = inner.getX() + (n == 1 ? 0.0f : (w * static_cast<float> (i) / static_cast<float> (n - 1)));
        const float y = inner.getBottom() - sampleAt (i) * h;
        if (i == 0)
            path.startNewSubPath (x, y);
        else
            path.lineTo (x, y);
    }

    juce::Path fillPath = path;
    fillPath.lineTo (inner.getRight(), inner.getBottom());
    fillPath.lineTo (inner.getX(), inner.getBottom());
    fillPath.closeSubPath();

    g.setColour (SynthLookAndFeel::accent.withAlpha (0.18f));
    g.fillPath (fillPath);
    g.setColour (SynthLookAndFeel::accent);
    g.strokePath (path, juce::PathStrokeType (1.4f, juce::PathStrokeType::curved,
                                              juce::PathStrokeType::rounded));
}
