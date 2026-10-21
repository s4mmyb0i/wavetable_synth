#include "synth/WavetableSynthesizer.h"

juce::SynthesiserSound* WavetableSynthesizer::findSoundFor (int note, int channel) const
{
    for (auto* sound : sounds)
        if (sound->appliesToNote (note) && sound->appliesToChannel (channel)) return sound;
    return nullptr;
}

juce::SynthesiserVoice* WavetableSynthesizer::monoVoice (juce::SynthesiserSound* sound, int channel,
                                                         int note)
{
    if (sound == nullptr) return nullptr;

    for (auto* voice : voices)
        if (voice->getCurrentlyPlayingNote() >= 0 && voice->isPlayingChannel (channel))
            return voice;

    return findFreeVoice (sound, channel, note, true);
}

void WavetableSynthesizer::noteOn (int midiChannel, int midiNoteNumber, float velocity)
{
    const juce::ScopedLock sl (lock);

    if (controller_.getMode() == VoiceMode::Poly)
    {
        juce::Synthesiser::noteOn (midiChannel, midiNoteNumber, velocity);
        return;
    }

    const bool alreadyHeld = ! controller_.allNotesOff();
    controller_.noteOn (midiNoteNumber, velocity);

    auto* sound = findSoundFor (midiNoteNumber, midiChannel);
    if (sound == nullptr) return;

    auto* voice = monoVoice (sound, midiChannel, midiNoteNumber);
    if (voice == nullptr) return;

    const bool useLegato = controller_.getMode() == VoiceMode::Legato && alreadyHeld &&
                           voice->getCurrentlyPlayingNote() >= 0;

    if (useLegato)
    {
        if (auto* sv = dynamic_cast<SynthVoice*> (voice)) sv->legatoToNote (midiNoteNumber);
    }
    else
    {
        startVoice (voice, sound, midiChannel, midiNoteNumber, velocity);
    }
}

void WavetableSynthesizer::noteOff (int midiChannel, int midiNoteNumber, float velocity,
                                    bool allowTailOff)
{
    const juce::ScopedLock sl (lock);

    if (controller_.getMode() == VoiceMode::Poly)
    {
        juce::Synthesiser::noteOff (midiChannel, midiNoteNumber, velocity, allowTailOff);
        return;
    }

    controller_.noteOff (midiNoteNumber);

    auto* sound = findSoundFor (midiNoteNumber, midiChannel);
    auto* voice = monoVoice (sound, midiChannel, midiNoteNumber);
    if (voice == nullptr) return;

    if (! controller_.allNotesOff())
    {
        const auto held = controller_.top();

        if (controller_.getMode() == VoiceMode::Legato)
        {
            if (auto* sv = dynamic_cast<SynthVoice*> (voice)) sv->legatoToNote (held.noteNumber);
        }
        else if (sound != nullptr)
        {
            startVoice (voice, sound, midiChannel, held.noteNumber, held.velocity);
        }
    }
    else
    {
        stopVoice (voice, velocity, allowTailOff);
    }
}

void WavetableSynthesizer::allNotesOff (int midiChannel, bool allowTailOff)
{
    controller_.clear();
    juce::Synthesiser::allNotesOff (midiChannel, allowTailOff);
}
