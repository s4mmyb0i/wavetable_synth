cmake_minimum_required(VERSION 3.22)

project(WavetableSynth VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# clangd / Cursor Cmd-click needs this compilation database.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Keep the JUCE tree itself from building extras/examples.
set(JUCE_BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
set(JUCE_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(libs/JUCE)

juce_add_plugin(WavetableSynth
    COMPANY_NAME "Sam"
    BUNDLE_ID com.sam.wavetablesynth
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Ssam
    PLUGIN_CODE Wt01
    FORMATS AU VST3 Standalone
    PRODUCT_NAME "Wavetable Synth"
    VST3_CATEGORIES Instrument Synth
    AU_MAIN_TYPE kAudioUnitType_MusicDevice)

target_sources(WavetableSynth
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/constants.h
        Source/dsp/Wavetable.cpp
        Source/dsp/WavetableOsc.cpp
        Source/dsp/Envelope.cpp
        Source/synth/SynthSound.h
        Source/synth/SynthVoice.cpp
        Source/params/ParameterLayout.cpp)

target_include_directories(WavetableSynth
    PRIVATE
        Source)

target_compile_definitions(WavetableSynth
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(WavetableSynth
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

# Offline DSP unit tests (Catch2). Does not build the plugin.
enable_testing()
add_subdirectory(tests)
