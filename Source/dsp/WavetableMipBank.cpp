#include "dsp/WavetableMipBank.h"

#include <algorithm>

void WavetableMipBank::build (Shape shape)
{
    shape_ = shape;

    for (int i = 0; i < numLevels; ++i)
    {
        const int n = kMaxHarmonics[static_cast<std::size_t> (i)];

        switch (shape)
        {
        case Shape::saw:
            levels_[static_cast<std::size_t> (i)] = Wavetable::sawBandLimited (n);
            break;
        case Shape::square:
            levels_[static_cast<std::size_t> (i)] = Wavetable::squareBandLimited (n);
            break;
        }
    }
}

const Wavetable& WavetableMipBank::select (float frequencyHz, float sampleRate) const
{
    if (frequencyHz <= 0.0f || sampleRate <= 0.0f)
        return levels_[static_cast<std::size_t> (numLevels - 1)];

    // Leave a little headroom under Nyquist.
    const float limit = 0.9f * 0.5f * sampleRate;

    for (int i = 0; i < numLevels; ++i)
    {
        const int maxHarmonics = kMaxHarmonics[static_cast<std::size_t> (i)];

        if (static_cast<float> (maxHarmonics) * frequencyHz < limit)
            return levels_[static_cast<std::size_t> (i)];
    }

    return levels_[static_cast<std::size_t> (numLevels - 1)];
}
