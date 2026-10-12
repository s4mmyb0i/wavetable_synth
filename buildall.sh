#!/usr/bin/env bash

set -e

BUILD_DIR="build"

# No arguments: build everything
if [ "$#" -eq 0 ]; then
    cmake --build "$BUILD_DIR" --target \
        WavetableSynth_AU \
        WavetableSynth_VST3 \
        WavetableSynth_Standalone \
        WavetableTests
    exit 0
fi

BUILD_TARGETS=()

for arg in "$@"; do
    case "$arg" in
    au)
        BUILD_TARGETS+=("WavetableSynth_AU")
        ;;
    vst3)
        BUILD_TARGETS+=("WavetableSynth_VST3")
        ;;
    standalone)
        BUILD_TARGETS+=("WavetableSynth_Standalone")
        ;;
    tests)
        BUILD_TARGETS+=("WavetableTests")
        ;;
    *)
        echo "Error: Unknown target '$arg'"
        echo "Available targets: au, vst3, standalone, tests"
        exit 1
        ;;
    esac
done

cmake --build "$BUILD_DIR" --target "${BUILD_TARGETS[@]}"
