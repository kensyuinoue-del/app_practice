#pragma once

#include "signal_graph_renderer.h"

#include <QFrame>
#include <QVector>

#include <memory>

class QPaintEvent;

class SignalGraphWidget : public QFrame {
public:
    explicit SignalGraphWidget(QWidget *parent = nullptr);

    void setAxisSettings(int microseconds, double amplitudeInterval);
    void setSampleRate(int sampleRateHz);
    void setSamples(std::shared_ptr<const QVector<double>> samples);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    SignalGraphRenderer m_renderer;
    std::shared_ptr<const QVector<double>> m_samples;
    int m_sampleRateHz = 0;
    int m_xIntervalMicroseconds = 1;
    double m_yInterval = 1.0;
};
