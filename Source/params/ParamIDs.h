#pragma once

namespace ParamIDs
{
// Osc mix/detune
inline constexpr const char* osc1Level = "osc1Level";
inline constexpr const char* osc2Level = "osc2Level";
inline constexpr const char* osc3Level = "osc3Level";

inline constexpr const char* osc2Detune = "osc2Detune"; // cents
inline constexpr const char* osc3Detune = "osc3Detune";

// Per-osc wavetype
inline constexpr const char* osc1Wave = "osc1Wave";
inline constexpr const char* osc2Wave = "osc2Wave";
inline constexpr const char* osc3Wave = "osc3Wave";

// Filter
inline constexpr const char* cutoff = "cutoff";
inline constexpr const char* resonance = "resonance";

// ADSR
inline constexpr const char* attack = "attack";
inline constexpr const char* decay = "decay";
inline constexpr const char* sustain = "sustain";
inline constexpr const char* release = "release";
} // namespace ParamIDs
