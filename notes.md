# Documenting this project

## Build commands:

`make -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug`
`cmake --build build --target WavetableSynth_AU WavetableSynth_VST3 WavetableSynth_Standalone`
`open "build/WavetableSynth_artefacts/Debug/Standalone/Wavetable Synth.app"`

## September 15, 2026

- Added ADSR
- Added GUI
- TODO: Add additional wave types (saw, triangle, square)

## September 13, 2026

- Connected JUCE Synthesizer API - made it polyphonic
- Added ADSR

## September 11, 2026

- Implemented a simple wavetable that plays a mono sine wave.