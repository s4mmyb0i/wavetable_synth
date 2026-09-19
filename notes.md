# Documenting this project

## Build commands:

`make -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug`
`cmake --build build --target WavetableSynth_AU WavetableSynth_VST3 WavetableSynth_Standalone`
`open "build/WavetableSynth_artefacts/Debug/Standalone/Wavetable Synth.app"`

`cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug   # once / when CMake changes`
`cmake --build build --target WavetableTests`
`./build/tests/WavetableTests`
OR
`ctest --test-dir build --output-on-failure`

September 19, 2026

- Added square and saw waves
- Added unit tests
- TODO: Add triangle wave
- TODO: Fix notes getting stuck when clicking on a menu item

## September 15, 2026

- Added ADSR
- Added GUI

## September 13, 2026

- Connected JUCE Synthesizer API - made it polyphonic
- Added ADSR

## September 11, 2026

- Implemented a simple wavetable that plays a mono sine wave.