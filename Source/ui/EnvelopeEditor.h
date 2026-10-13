#pragma once

#include "ui/EnvelopeShape.h"
#include "ui/SynthLookAndFeel.h"
#include <functional>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

// Interactive ADSR plot. Listens to APVTS params (Option A); knobs stay separate.
class EnvelopeEditor final : public juce::Component, private juce::AudioProcessorParameter::Listener
{
public:
    EnvelopeEditor();
    ~EnvelopeEditor() override;

    void attach (juce::AudioProcessorValueTreeState& apvts, const juce::String& attackID,
                 const juce::String& decayID, const juce::String& sustainID,
                 const juce::String& releaseID, const juce::String& attackCurveXID,
                 const juce::String& attackCurveYID, const juce::String& decayCurveXID,
                 const juce::String& decayCurveYID, const juce::String& releaseCurveXID,
                 const juce::String& releaseCurveYID);

    void paint (juce::Graphics& g) override;
    void resized() override;

    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseUp (const juce::MouseEvent& e) override;
    void mouseMove (const juce::MouseEvent& e) override;
    void mouseExit (const juce::MouseEvent& e) override;

private:
    void parameterValueChanged (int parameterIndex, float newValue) override;
    void parameterGestureChanged (int, bool) override {}

    EnvelopeShape::Params readParams() const;
    juce::Rectangle<float> plotBounds() const;
    int hitTestHandle (juce::Point<float> pos) const;
    void applyDrag (juce::Point<float> pos);
    void setParamValue (juce::AudioParameterFloat* param, float denormalised);
    void setCurveHandle (juce::AudioParameterFloat* xParam, juce::AudioParameterFloat* yParam,
                         EnvelopeCurve::Handle h);
    void syncCursor (int handle);
    void forEachParam (const std::function<void (juce::AudioProcessorParameter*)>& fn) const;

    juce::AudioParameterFloat* attack_ = nullptr;
    juce::AudioParameterFloat* decay_ = nullptr;
    juce::AudioParameterFloat* sustain_ = nullptr;
    juce::AudioParameterFloat* release_ = nullptr;
    juce::AudioParameterFloat* attackCurveX_ = nullptr;
    juce::AudioParameterFloat* attackCurveY_ = nullptr;
    juce::AudioParameterFloat* decayCurveX_ = nullptr;
    juce::AudioParameterFloat* decayCurveY_ = nullptr;
    juce::AudioParameterFloat* releaseCurveX_ = nullptr;
    juce::AudioParameterFloat* releaseCurveY_ = nullptr;

    juce::NormalisableRange<float> timeRange_ {0.001f, 5.0f, 0.001f};

    int activeHandle_ = -1;
    int hoverHandle_ = -1;
    bool gestureOpen_ = false;

    static constexpr float handleRadius = 5.0f;
    static constexpr float curveHandleRadius = 4.0f;
    static constexpr float grabRadius = 10.0f;
};
