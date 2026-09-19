#include "dsp/Wavetable.h"
#include "dsp/MathConstants.h"

#include <cmath>

Wavetable Wavetable::sine()
{
    Wavetable table;

    for (std::size_t i = 0; i < size; ++i)
    {
        const double phase = static_cast<double>(i) / size;
        table.samples_[i] = static_cast<float>(std::sin(phase * dsp::kTwoPi));
    }

    return table;
}

Wavetable Wavetable::saw()
{
    Wavetable table;

    for (std::size_t i = 0; i < size; ++i)
    {
        const double phase = static_cast<double>(i) / size;
        table.samples_[i] = static_cast<float>(2.0 * phase - 1.0);
    }

    return table;
}

Wavetable Wavetable::square()
{
    Wavetable table;

    for (std::size_t i = 0; i < size; ++i)
    {
        const double phase = static_cast<double>(i) / size;
        table.samples_[i] = phase < 0.5 ? 1.0f : -1.0f;
    }
    
    return table;
}

float Wavetable::lookup(float phase01) const
{
    phase01 = phase01 - std::floor(phase01);
    float position = phase01 * size;
    
    auto index0 = static_cast<std::size_t>(std::floor(position));
    std::size_t index1 = (index0 + 1) & (size - 1);
    
    float frac = position - static_cast<float>(index0);
    return samples_[index0] + frac * (samples_[index1] - samples_[index0]);
}
