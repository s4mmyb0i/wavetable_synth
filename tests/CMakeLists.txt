include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.4
)
FetchContent_MakeAvailable(Catch2)

add_executable(WavetableTests
    WavetableTests.cpp
    WavetableOscTests.cpp
    EnvelopeTests.cpp
    ${CMAKE_SOURCE_DIR}/Source/dsp/Wavetable.cpp
    ${CMAKE_SOURCE_DIR}/Source/dsp/WavetableOsc.cpp
    ${CMAKE_SOURCE_DIR}/Source/dsp/Envelope.cpp)

target_include_directories(WavetableTests PRIVATE ${CMAKE_SOURCE_DIR}/Source)

target_link_libraries(WavetableTests PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(WavetableTests)
