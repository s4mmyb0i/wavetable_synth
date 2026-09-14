# Documenting this project

## Build commands:

`make -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug`
`cmake --build build --target WavetableSynth_AU WavetableSynth_VST3 WavetableSynth_Standalone`
`open "build/WavetableSynth_artefacts/Debug/Standalone/Wavetable Synth.app"`

## September 13, 2026

- Connected JUCE Synthesizer API - made it polyphonic
- Added ADSR
- TODO: Make ADSR customizable and add GUI

## September 11, 2026

- Implemented a simple wavetable that plays a mono sine wave.