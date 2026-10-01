#include "waveform_calculator.h"

#include <cmath>

namespace wave {
namespace {
constexpr double pi = 3.14159265358979323846;
}

double sampleValue(Waveform waveform, double phase)
{
    switch (waveform) {
    case Waveform::Square:
        return phase < 0.5 ? 1.0 : -1.0;
    case Waveform::Triangle:
        return 1.0 - 4.0 * std::abs(phase - 0.5);
    case Waveform::Sawtooth:
        return 2.0 * phase - 1.0;
    case Waveform::Sine:
    default:
        return std::sin(2.0 * pi * phase);
    }
}

}