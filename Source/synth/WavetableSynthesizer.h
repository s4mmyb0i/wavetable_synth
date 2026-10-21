#pragma once

#include "synth/SynthVoice.h"
#include "synth/VoiceModeController.h"

#include <juce_audio_basics/juce_audio_basics.h>

// Extends JUCE's Synthesiser so MIDI noteOn/noteOff can honor Poly / Mono / Legato.
class WavetableSynthesizer final : public juce::Synthesiser
{
public:
    void setVoiceMode (VoiceMode voiceMode) { controller_.setMode (voiceMode); }
    VoiceMode getVoiceMode() const { return controller_.getMode(); }

    void noteOn (int midiChannel, int midiNoteNumber, float velocity) override;
    void noteOff (int midiChannel, int midiNoteNumber, float velocity, bool allowTailOff) override;
    void allNotesOff (int midiChannel, bool allowTailOff) override;

private:
    VoiceModeController controller_;

    juce::SynthesiserSound* findSoundFor (int note, int channel) const;
    juce::SynthesiserVoice* monoVoice (juce::SynthesiserSound* sound, int channel, int note);
};
