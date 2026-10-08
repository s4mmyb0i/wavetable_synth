#pragma once

namespace ParamIDs
{
// Osc mix/detune
inline constexpr const char* osc1Level = "osc1Level";
inline constexpr const char* osc2Level = "osc2Level";
inline constexpr const char* osc3Level = "osc3Level";

inline constexpr const char* osc2Detune = "osc2Detune"; // cents
inline constexpr const char* osc3Detune = "osc3Detune";

// Per-osc unison
inline constexpr const char* osc1UnisonCount = "osc1UnisonCount";
inline constexpr const char* osc1UnisonDetune = "osc1UnisonDetune";
inline constexpr const char* osc2UnisonCount = "osc2UnisonCount";
inline constexpr const char* osc2UnisonDetune = "osc2UnisonDetune";
inline constexpr const char* osc3UnisonCount = "osc3UnisonCount";
inline constexpr const char* osc3UnisonDetune = "osc3UnisonDetune";

// Per-osc wavetype
inline constexpr const char* osc1Wave = "osc1Wave";
inline constexpr const char* osc2Wave = "osc2Wave";
inline constexpr const char* osc3Wave = "osc3Wave";

// Per-osc warp (choice index matches WarpMode: Off, Bend+, Sync)
inline constexpr const char* osc1WarpMode = "osc1WarpMode";
inline constexpr const char* osc1WarpAmount = "osc1WarpAmount";
inline constexpr const char* osc2WarpMode = "osc2WarpMode";
inline constexpr const char* osc2WarpAmount = "osc2WarpAmount";
inline constexpr const char* osc3WarpMode = "osc3WarpMode";
inline constexpr const char* osc3WarpAmount = "osc3WarpAmount";

// Filter
inline constexpr const char* cutoff = "cutoff";
inline constexpr const char* resonance = "resonance";

// ADSR
inline constexpr const char* attack = "attack";
inline constexpr const char* decay = "decay";
inline constexpr const char* sustain = "sustain";
inline constexpr const char* release = "release";
} // namespace ParamIDs
