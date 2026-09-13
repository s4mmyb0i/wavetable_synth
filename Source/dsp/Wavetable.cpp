#include "dsp/Wavetable.h"
#include "dsp/MathConstants.h"

#include <cmath>

Wavetable Wavetable::sine()
{
    Wavetable table;

    double phase;
    for (int i = 0; i < static_cast<int>(size); ++i)
    {
        phase = static_cast<double>(i) / static_cast<double>(size);
        table.samples_[static_cast<std::size_t>(i)] = static_cast<float>(std::sin(phase * dsp::kTwoPi));
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
