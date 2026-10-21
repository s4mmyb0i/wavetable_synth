#pragma once

#include <vector>

// Order matches AudioParameterChoice / VoiceModeSwitch: Poly | Mono | Legato
enum class VoiceMode
{
    Poly = 0,
    Mono = 1,
    Legato = 2,
};

struct HeldNote
{
    int noteNumber;
    float velocity;
};

class VoiceModeController
{
public:
    using NoteStack = std::vector<HeldNote>;

    void setMode (VoiceMode voiceMode) { mode_ = voiceMode; }
    VoiceMode getMode() const { return mode_; }

    void noteOn (int noteNumber, float velocity)
    {
        for (auto it = stack_.begin(); it != stack_.end(); ++it)
        {
            if (it->noteNumber == noteNumber)
            {
                stack_.erase (it);
                break;
            }
        }
        stack_.push_back ({noteNumber, velocity});
    }

    void noteOff (int noteNumber)
    {
        for (auto it = stack_.begin(); it != stack_.end(); ++it)
        {
            if (it->noteNumber == noteNumber)
            {
                stack_.erase (it);
                break;
            }
        }
    }

    bool allNotesOff() const { return stack_.empty(); }
    void clear() { stack_.clear(); }

    HeldNote top() const { return stack_.back(); }

private:
    VoiceMode mode_ = VoiceMode::Poly; // match previous always-poly behaviour until UI exists
    NoteStack stack_;
};
