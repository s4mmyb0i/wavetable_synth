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

// Per-osc wavetable position (used when wave = Morph)
inline constexpr const char* osc1Position = "osc1Position";
inline constexpr const char* osc2Position = "osc2Position";
inline constexpr const char* osc3Position = "osc3Position";

// Per-osc pan
inline constexpr const char* osc1Pan = "osc1Pan";
inline constexpr const char* osc2Pan = "osc2Pan";
inline constexpr const char* osc3Pan = "osc3Pan";

// Noise Generator
inline constexpr const char* noiseType = "noiseType";
inline constexpr const char* noiseLevel = "noiseLevel";
inline constexpr const char* noisePan = "noisePan";

// Filter
inline constexpr const char* cutoff = "cutoff";
inline constexpr const char* resonance = "resonance";

// Per-source ADSR (osc 1–3 + noise)
inline constexpr const char* osc1Attack = "osc1Attack";
inline constexpr const char* osc1Decay = "osc1Decay";
inline constexpr const char* osc1Sustain = "osc1Sustain";
inline constexpr const char* osc1Release = "osc1Release";

inline constexpr const char* osc2Attack = "osc2Attack";
inline constexpr const char* osc2Decay = "osc2Decay";
inline constexpr const char* osc2Sustain = "osc2Sustain";
inline constexpr const char* osc2Release = "osc2Release";

inline constexpr const char* osc3Attack = "osc3Attack";
inline constexpr const char* osc3Decay = "osc3Decay";
inline constexpr const char* osc3Sustain = "osc3Sustain";
inline constexpr const char* osc3Release = "osc3Release";

inline constexpr const char* noiseAttack = "noiseAttack";
inline constexpr const char* noiseDecay = "noiseDecay";
inline constexpr const char* noiseSustain = "noiseSustain";
inline constexpr const char* noiseRelease = "noiseRelease";
} // namespace ParamIDs
