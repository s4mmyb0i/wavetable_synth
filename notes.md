# Documenting this project

## Build commands:

`cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug`
`cmake --build build --target WavetableSynth_AU WavetableSynth_VST3 WavetableSynth_Standalone`
`open "build/WavetableSynth_artefacts/Debug/Standalone/Wavetable Synth.app"`

`cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug   # once / when CMake changes`
`cmake --build build --target WavetableTests`
`./build/tests/WavetableTests`
OR
`ctest --test-dir build --output-on-failure`

## October 3, 2026

- Added multiple (3) oscillators
- Connect PlugInGuiMagic for UI
- Add tests for multi-osc

## October 1, 2026

- Added additional mip tables
- Added two pole filter - resonance now works correctly
- Removed unused naive square/saw table value generation
- Updated tests to match

## Setpember 25, 2026

- Added basic 1 pole low pass filter

## September 21, 2026

- Added triangle waves
- Initial attempt at RMS normalization (saw and square waves sound much louder than triangle and sine)

## September 19, 2026

- Added square and saw waves
- Added unit tests

## September 15, 2026

- Added ADSR
- Added GUI

## September 13, 2026

- Connected JUCE Synthesizer API - made it polyphonic
- Added ADSR

## September 11, 2026

- Implemented a simple wavetable that plays a mono sine wave.
