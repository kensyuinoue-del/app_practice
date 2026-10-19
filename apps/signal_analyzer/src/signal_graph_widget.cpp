#include "signal_graph_widget.h"

#include <QPainter>

#include <utility>

SignalGraphWidget::SignalGraphWidget(QWidget *parent)
    : QFrame(parent) {}

void SignalGraphWidget::setAxisSettings(int microseconds, double amplitudeInterval) {
    m_xIntervalMicroseconds = microseconds;
    m_yInterval = amplitudeInterval;
    update();
}

void SignalGraphWidget::setSampleRate(int sampleRateHz) {
    m_sampleRateHz = sampleRateHz;
    update();
}

void SignalGraphWidget::setSamples(std::shared_ptr<const QVector<double>> samples) {
    m_samples = std::move(samples);
    m_renderer.invalidateEnvelope();
    update();
}

void SignalGraphWidget::paintEvent(QPaintEvent *event) {
    QFrame::paintEvent(event);

    QPainter painter(this);
    m_renderer.draw(painter, contentsRect(), m_samples, m_sampleRateHz,
        m_xIntervalMicroseconds, m_yInterval);
}
