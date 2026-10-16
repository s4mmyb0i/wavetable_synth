#!/usr/bin/env bash

set -e

BUILD_DIR="build"

# Optional first flag: --release  → use build-release (CMAKE_BUILD_TYPE=Release)
if [ "${1:-}" = "--release" ]; then
    BUILD_DIR="build-release"
    shift
    if [ ! -f "$BUILD_DIR/CMakeCache.txt" ]; then
        echo "Configuring $BUILD_DIR (Release)..."
        cmake -B "$BUILD_DIR" -G Ninja -DCMAKE_BUILD_TYPE=Release \
            -DFETCHCONTENT_SOURCE_DIR_CATCH2="${PWD}/build/_deps/catch2-src"
    fi
fi

# No target args: build everything in the selected tree
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
        echo "Usage: ./buildall.sh [--release] [au|vst3|standalone|tests]..."
        exit 1
        ;;
    esac
done

cmake --build "$BUILD_DIR" --target "${BUILD_TARGETS[@]}"
