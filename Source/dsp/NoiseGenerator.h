#pragma once

#include <cstdint>
#include <random>

class NoiseGenerator
{
public:
    enum class Type
    {
        white,
        pink
    };

    explicit NoiseGenerator (std::uint32_t seed = 0xC0FFEE) : rng_ (seed) {}

    void setType (Type t) { type_ = t; }
    Type getType() const { return type_; }

    void reset() { b0_ = b1_ = b2_ = b3_ = b4_ = b5_ = b6_ = 0.0f; }

    float process() { return (type_ == Type::pink) ? processPink() : processWhite(); }

private:
    float processWhite() { return dist_ (rng_); }

    float processPink()
    {
        const float white = processWhite();

        b0_ = 0.99886f * b0_ + white * 0.0555179f;
        b1_ = 0.99332f * b1_ + white * 0.0750759f;
        b2_ = 0.96900f * b2_ + white * 0.1538520f;
        b3_ = 0.86650f * b3_ + white * 0.3104856f;
        b4_ = 0.55000f * b4_ + white * 0.5329522f;
        b5_ = -0.7616f * b5_ - white * 0.0168980f;

        const float pink = b0_ + b1_ + b2_ + b3_ + b4_ + b5_ + b6_ + white * 0.5362f;
        b6_ = white * 0.115926f;

        return pink * 0.11f;
    }

    Type type_ = Type::white;
    std::mt19937 rng_;
    std::uniform_real_distribution<float> dist_ {-1.0f, 1.0f};

    float b0_ = 0, b1_ = 0, b2_ = 0, b3_ = 0, b4_ = 0, b5_ = 0, b6_ = 0;
};
