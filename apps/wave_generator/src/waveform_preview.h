#pragma once

#include "waveform_calculator.h"

#include <QWidget>

class QPaintEvent;

class WaveformPreview : public QWidget {
public:
    explicit WaveformPreview(QWidget *parent = nullptr);

    void setSignal(wave::Waveform waveform, double frequency, double duration, double amplitude);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    wave::Waveform waveform = wave::Waveform::Sine;
    double frequency = 440.0;
    double duration = 2.0;
    double amplitude = 0.8;
};