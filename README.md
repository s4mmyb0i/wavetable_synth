# Wavetable Synth

A polyphonic wavetable synthesizer built with [JUCE](https://juce.com/) and CMake. Learning project for wavetable DSP and plugin design.

**Formats:** AU · VST3 · Standalone

## Features

- **8-voice polyphony** via JUCE `Synthesiser` / `SynthesiserVoice`
- **3 oscillators** per voice, each with:
  - Wave: Sine, Saw, Square, Triangle, or Morph bank
  - Morph position (when wave = Morph)
  - Warp (Off / Bend+ / Sync) + amount
  - Unison (1-8) + spread
  - Level, pan (-1...+1, equal-power)
  - Detune on Osc 2 & 3 (cents)
- **Noise generator** (white / pink) with level, pan, its own ADSR, and its own filter
- **Per-source ADSR** for Osc 1-3 and Noise (custom envelope, not `juce::ADSR`)
  - Interactive graph + A/D/S/R knobs; source selector switches which params they edit
- **Two stereo SVF low-passes**: one on the osc sum, one on noise (independent cutoff/resonance)
  - Interactive response graph; Osc 1-3 | Noise selector switches which filter the UI edits
- **Mip-mapped** band-limited saw/square tables; mip chosen by note frequency
- **APVTS** parameters + custom LookAndFeel editor (waveform previews, segment switches)
- **Catch2** unit tests for core DSP (wavetables, warp, noise, filter, envelopes, per-source mix)

### Signal path (per voice)

```text
osc1 × level × env1 ─┐
osc2 × level × env2 ─┼→ pan → sum × ¼ → osc SVF ─┐
osc3 × level × env3 ─┘                           ├→ + → × velocity → out
noise × level × env4 → pan → × ¼ → noise SVF ────┘
```

The voice stays active while **any** of the four envelopes is still running. Velocity is a linear 0…1 gain. The ¼ mix scale keeps four unit-gain sources near ±1 peak.

## Requirements

- CMake 3.22+
- C++17 compiler (Xcode / Clang on macOS)
- Ninja (recommended) or another CMake generator
- JUCE (git submodule under `libs/JUCE`)

```bash
git submodule update --init --recursive
```

## Build

Configure once (or when CMake / deps change):

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
```

Build the plugin formats:

```bash
cmake --build build --target WavetableSynth_AU WavetableSynth_VST3 WavetableSynth_Standalone
```

Run the standalone app (macOS):

```bash
open "build/WavetableSynth_artefacts/Debug/Standalone/Wavetable Synth.app"
```

With `COPY_PLUGIN_AFTER_BUILD` enabled, AU/VST3 are also copied to the usual user plugin folders after a successful build.

## Tests

```bash
cmake --build build --target WavetableTests
./build/tests/WavetableTests
# or
ctest --test-dir build --output-on-failure
```

Filter by tag, e.g. per-source envelope tests only:

```bash
./build/tests/WavetableTests "[per-source-env]"
```

Other useful tags: `[envelope]`, `[multi-osc]`, `[mip]`, `[warp]`, `[noise]`, `[filter]`.

## Parameters

| Parameter | Role |
|-----------|------|
| Osc 1-3 Level | Mix level for each oscillator |
| Osc 2-3 Detune | Cents offset vs note pitch |
| Osc 1-3 Wave | Sine / Saw / Square / Triangle / Morph |
| Osc 1-3 Position | Morph bank position (0...1; used when Wave = Morph) |
| Osc 1-3 Warp / Warp Amt | Phase warp mode and depth |
| Osc 1-3 Unison / Spread | Voice count (1-8) and detune spread (cents) |
| Osc 1-3 Pan | Equal-power pan (-1 = L, 0 = centre, +1 = R) |
| Noise Type / Level / Pan | White or pink; level; pan |
| Osc 1-3 Cutoff / Resonance | Shared stereo SVF on Osc 1+2+3 mix (Hz; 0...1) |
| Noise Cutoff / Resonance | Stereo SVF on noise only (Hz; 0...1) |
| Osc 1-3 Attack...Release | Per-oscillator ADSR |
| Noise Attack…Release | Noise ADSR |

## Project layout

| Path | Contents |
|------|----------|
| `Source/dsp/` | Wavetable, osc, envelope, filter, noise, warp helpers |
| `Source/synth/` | `SynthVoice` / `SynthSound` |
| `Source/params/` | Param IDs + APVTS layout |
| `Source/ui/` | Editor widgets (envelope graph, wave knobs, switches, …) |
| `tests/` | Catch2 DSP tests |
