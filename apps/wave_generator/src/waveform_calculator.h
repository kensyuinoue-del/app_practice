#pragma once

namespace wave {

enum class Waveform {
    Sine,
    Square,
    Triangle,
    Sawtooth
};

struct Settings {
    Waveform waveform;
    double frequency;
    double duration;
    double amplitude;
    int sampleRate;
    int channels;
};

double sampleValue(Waveform waveform, double phase);

}