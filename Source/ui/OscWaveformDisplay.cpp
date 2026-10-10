#include "ui/OscWaveformDisplay.h"
#include "PluginProcessor.h"
#include "dsp/Wavetable.h"
#include "dsp/WavetableBank.h"
#include "dsp/WavetableMipBank.h"

namespace
{
float paramValue (juce::AudioProcessorParameter* p, float fallback)
{
    if (p == nullptr) return fallback;
    if (auto* pf = dynamic_cast<juce::AudioParameterFloat*> (p)) return pf->get();
    if (auto* pi = dynamic_cast<juce::AudioParameterInt*> (p))
        return static_cast<float> (pi->get());
    if (auto* pc = dynamic_cast<juce::AudioParameterChoice*> (p))
        return static_cast<float> (pc->getIndex());
    if (auto* ranged = dynamic_cast<juce::RangedAudioParameter*> (p))
        return ranged->convertFrom0to1 (ranged->getValue());
    return fallback;
}
} // namespace

OscWaveformDisplay::OscWaveformDisplay()
{ setInterceptsMouseClicks (false, false); }

OscWaveformDisplay::~OscWaveformDisplay()
{
    for (auto** slot : {&wave_, &position_, &warpMode_, &warpAmount_, &unisonCount_, &unisonDetune_})
        if (*slot != nullptr)
            (*slot)->removeListener (this);
}

void OscWaveformDisplay::listen (juce::AudioProcessorParameter*& slot,
                                 juce::AudioProcessorParameter* param)
{
    if (slot != nullptr)
        slot->removeListener (this);
    slot = param;
    if (slot != nullptr)
        slot->addListener (this);
}

void OscWaveformDisplay::attach (WavetableSynthAudioProcessor& processor,
                                 juce::AudioProcessorValueTreeState& apvts,
                                 const juce::String& waveID, const juce::String& positionID,
                                 const juce::String& warpModeID, const juce::String& warpAmountID,
                                 const juce::String& unisonCountID,
                                 const juce::String& unisonDetuneID)
{
    processor_ = &processor;
    listen (wave_, apvts.getParameter (waveID));
    listen (position_, apvts.getParameter (positionID));
    listen (warpMode_, apvts.getParameter (warpModeID));
    listen (warpAmount_, apvts.getParameter (warpAmountID));
    listen (unisonCount_, apvts.getParameter (unisonCountID));
    listen (unisonDetune_, apvts.getParameter (unisonDetuneID));
    repaint();
}

void OscWaveformDisplay::parameterValueChanged (int, float)
{
    juce::MessageManager::callAsync ([safe = juce::Component::SafePointer (this)]
                                     {
                                         if (safe != nullptr)
                                             safe->repaint();
                                     });
}

float OscWaveformDisplay::lookupSample (float phase01) const
{
    if (processor_ == nullptr) return 0.0f;

    const int waveIndex = juce::roundToInt (paramValue (wave_, 0.0f));
    const float position = paramValue (position_, 0.0f);
    const auto mode = static_cast<WarpMode> (juce::roundToInt (paramValue (warpMode_, 0.0f)));
    const float amount = paramValue (warpAmount_, 0.0f);
    const float warped = warpPhase (phase01, amount, mode);

    constexpr float kPreviewHz = 440.0f;
    constexpr float kPreviewSr = 44100.0f;

    switch (waveIndex)
    {
    case 1: return processor_->getSawMipBank().select (kPreviewHz, kPreviewSr).lookup (warped);
    case 2: return processor_->getSquareMipBank().select (kPreviewHz, kPreviewSr).lookup (warped);
    case 3: return processor_->getTriangleTable().lookup (warped);
    case 4: return processor_->getMorphBank().lookup (position, warped);
    default: return processor_->getSineTable().lookup (warped);
    }
}

void OscWaveformDisplay::drawVoice (juce::Graphics& g, juce::Rectangle<float> plot, int voiceIndex,
                                    int uniCount, float detuneCents, float alpha, float stroke) const
{
    // Horizontal skew hints detune without changing the cycle shape.
    constexpr float kPxPerCent = 0.22f;
    const float xShift = unisonSpread (voiceIndex, uniCount) * detuneCents * kPxPerCent;

    juce::Path path;
    for (int i = 0; i < numPlotPoints; ++i)
    {
        const float t = static_cast<float> (i) / static_cast<float> (numPlotPoints - 1);
        const float s = lookupSample (t);
        const float x = plot.getX() + t * plot.getWidth() + xShift;
        const float y = plot.getCentreY() - s * plot.getHeight() * 0.42f;
        if (i == 0)
            path.startNewSubPath (x, y);
        else
            path.lineTo (x, y);
    }

    g.setColour (SynthLookAndFeel::accent.withAlpha (alpha));
    g.strokePath (path, juce::PathStrokeType (stroke, juce::PathStrokeType::curved,
                                              juce::PathStrokeType::rounded));
}

void OscWaveformDisplay::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setColour (SynthLookAndFeel::background.brighter (0.04f));
    g.fillRoundedRectangle (bounds, 6.0f);
    g.setColour (SynthLookAndFeel::panelBorder);
    g.drawRoundedRectangle (bounds.reduced (0.5f), 6.0f, 1.0f);

    auto plot = bounds.reduced (8.0f, 6.0f);
    if (plot.getWidth() < 8.0f || plot.getHeight() < 8.0f) return;

    g.setColour (SynthLookAndFeel::track.withAlpha (0.5f));
    g.drawHorizontalLine (juce::roundToInt (plot.getCentreY()), plot.getX(), plot.getRight());

    const int uniCount =
        juce::jlimit (1, maxUnison, juce::roundToInt (paramValue (unisonCount_, 1.0f)));
    const float detuneCents = paramValue (unisonDetune_, 0.0f);

    // Dimmer ghosts first; fundamental (spread ≈ 0) on top.
    const int center = uniCount / 2;
    for (int i = 0; i < uniCount; ++i)
    {
        if (i == center) continue;
        const float dist = std::abs (unisonSpread (i, uniCount));
        const float alpha = juce::jmap (dist, 0.0f, 1.0f, 0.35f, 0.18f);
        drawVoice (g, plot, i, uniCount, detuneCents, alpha, 1.2f);
    }
    drawVoice (g, plot, center, uniCount, detuneCents, 0.95f, 1.8f);
}
