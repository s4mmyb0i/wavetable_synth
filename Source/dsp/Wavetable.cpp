#include "dsp/Wavetable.h"
#include "dsp/MathConstants.h"

#include <cmath>

namespace
{
constexpr float kTargetRms = 0.70710678118f; // 1/sqrt(2) — matches a unit-peak sine
}

void Wavetable::normalizeRms (float targetRms)
{
    double sumSquares = 0.0;

    for (float sample : samples_)
        sumSquares += static_cast<double> (sample) * static_cast<double> (sample);

    const double rms = std::sqrt (sumSquares / static_cast<double> (size));

    if (rms <= 1.0e-12)
        return;

    const float scale = targetRms / static_cast<float> (rms);

    for (float& sample : samples_)
        sample *= scale;
}

Wavetable Wavetable::sine()
{
    Wavetable table;

    for (std::size_t i = 0; i < size; ++i)
    {
        const double phase = static_cast<double> (i) / static_cast<double> (size);
        table.samples_[i] = static_cast<float> (std::sin (phase * dsp::kTwoPi));
    }

    table.normalizeRms (kTargetRms);
    return table;
}

Wavetable Wavetable::saw()
{
    Wavetable table;

    for (std::size_t i = 0; i < size; ++i)
    {
        const double phase = static_cast<double> (i) / static_cast<double> (size);
        table.samples_[i] = static_cast<float> (2.0 * phase - 1.0);
    }

    table.normalizeRms (kTargetRms);
    return table;
}

Wavetable Wavetable::square()
{
    Wavetable table;

    for (std::size_t i = 0; i < size; ++i)
    {
        const double phase = static_cast<double> (i) / static_cast<double> (size);
        table.samples_[i] = phase < 0.5 ? 1.0f : -1.0f;
    }

    table.normalizeRms (kTargetRms);
    return table;
}

Wavetable Wavetable::triangle()
{
    Wavetable table;

    for (std::size_t i = 0; i < size; ++i)
    {
        const double phase = static_cast<double> (i) / static_cast<double> (size);

        if (phase < 0.5)
            table.samples_[i] = static_cast<float> (4.0 * phase - 1.0);
        else
            table.samples_[i] = static_cast<float> (-4.0 * phase + 3.0);
    }

    table.normalizeRms (kTargetRms);
    return table;
}

float Wavetable::lookup (float phase01) const
{
    phase01 = phase01 - std::floor (phase01);
    const float position = phase01 * static_cast<float> (size);

    const auto index0 = static_cast<std::size_t> (std::floor (position));
    const std::size_t index1 = (index0 + 1) & (size - 1);

    const float frac = position - static_cast<float> (index0);
    return samples_[index0] + frac * (samples_[index1] - samples_[index0]);
}
