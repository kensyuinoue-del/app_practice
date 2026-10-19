#pragma once

#include <QVector>

#include <memory>

class QPainter;
class QPen;
class QRectF;

class SignalGraphRenderer {
public:
    void draw(QPainter &painter, const QRectF &plotArea,
        const std::shared_ptr<const QVector<double>> &samples,
        int sampleRateHz, int xIntervalMicroseconds, double yInterval);
    void invalidateEnvelope();

private:
    struct EnvelopeBucket {
        int minIndex = -1;
        int maxIndex = -1;
    };

    static constexpr int visibleXTickCount = 11;
    static constexpr int visibleYHalfTickCount = 11;

    void drawXGrid(QPainter &painter, const QRectF &plotArea, double maxX,
        double visibleXInterval, const QPen &gridPen) const;
    void drawYGrid(QPainter &painter, const QRectF &plotArea, double minY, double maxY,
        double visibleYInterval, const QPen &gridPen) const;
    void drawAxes(QPainter &painter, const QRectF &plotArea) const;
    void updateEnvelope(int width, int visibleSampleCount, const QVector<double> &samples);
    void drawSamples(QPainter &painter, const QRectF &plotArea,
        double samplePeriodMicroseconds, double maxX, double minY, double maxY,
        const QVector<double> &samples) const;

    QVector<EnvelopeBucket> m_envelope;
    int m_envelopeWidth = 0;
    int m_envelopeSampleCount = 0;
};
