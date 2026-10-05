# Wavetable Synth

A polyphonic wavetable synthesizer built with [JUCE](https://juce.com/) and CMake. Learning project for wavetable DSP and overall plugin design.

**Formats:** AU · VST3 · Standalone

## Features

- **8-voice polyphony** via JUCE `Synthesiser` / `SynthesiserVoice`
- **Waveforms:** sine, triangle (fixed tables); saw & square (additive, band-limited mip banks)
- **Mip selection** by note frequency so harmonics stay under ~0.9× Nyquist
- **ADSR envelope** (custom, not `juce::ADSR`)
- **SVF low-pass** with cutoff and resonance
- **APVTS** parameters + custom LookAndFeel editor (osc mix/detune, filter, ADSR)
- **Catch2** unit tests for core DSP

Signal path per voice: `oscillator → filter → × envelope × velocity`

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

Filter by tag, e.g. mip tests only:

```bash
./build/tests/WavetableTests "[mip]"
```

## Parameters

| Parameter   | Role                                      |
|------------|--------------------------------------------|
| Osc 1–3 Level | Mix levels for each oscillator          |
| Osc 2–3 Detune | Cents offset vs note pitch              |
| Wave       | Sine / Saw / Square / Triangle             |
| Cutoff     | Filter cutoff (Hz)                         |
| Resonance  | SVF resonance (0 = gentle, 1 ≈ self-osc) |
| Attack…Release | ADSR times / sustain level            |