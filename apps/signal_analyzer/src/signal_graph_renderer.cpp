#include "signal_graph_renderer.h"

#include <QFrame>
#include <QPainter>
#include <QPainterPath>

#include <algorithm>
#include <cmath>

void SignalGraphRenderer::invalidateEnvelope() {
    m_envelopeWidth = 0;
    m_envelopeSampleCount = 0;
}

void SignalGraphRenderer::draw(QPainter &painter, const QRectF &plotArea,
    const std::shared_ptr<const QVector<double>> &samples,
    int sampleRateHz, int xIntervalMicroseconds, double yInterval) {
    if (plotArea.width() <= 0 || plotArea.height() <= 0) {
        return;
    }

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setClipRect(plotArea);

    painter.setBrush(QBrush(QColor("#ffffff")));
    painter.setPen(Qt::NoPen);
    painter.drawRect(plotArea);

    const double maxX = xIntervalMicroseconds * visibleXTickCount;
    const double yRange = yInterval * visibleYHalfTickCount;
    const double minY = -yRange;
    const double maxY = yRange;
    const double visibleXInterval = xIntervalMicroseconds;
    const double visibleYInterval = yInterval;

    const QPen gridPen(QColor("#d0d6dc"), 1, Qt::DashLine);
    drawXGrid(painter, plotArea, maxX, visibleXInterval, gridPen);
    drawYGrid(painter, plotArea, minY, maxY, visibleYInterval, gridPen);
    drawAxes(painter, plotArea);

    if (!samples || samples->isEmpty() || sampleRateHz <= 0) {
        return;
    }

    const double samplePeriodMicroseconds = 1000000.0 / sampleRateHz;
    const double visibleSampleCountValue = std::floor(maxX / samplePeriodMicroseconds) + 1.0;
    const int visibleSampleCount = static_cast<int>(std::min(
        static_cast<double>(samples->size()), visibleSampleCountValue)
    );
    updateEnvelope(std::max(1, qRound(plotArea.width())), visibleSampleCount, *samples);
    drawSamples(painter, plotArea, samplePeriodMicroseconds, maxX, minY, maxY, *samples);
}

void SignalGraphRenderer::drawXGrid(QPainter &painter, const QRectF &plotArea, double maxX,
    double visibleXInterval, const QPen &gridPen) const {
    painter.setPen(gridPen);
    for (double timeMicroseconds = 0.0;
         timeMicroseconds <= maxX + (visibleXInterval * 1e-9);
         timeMicroseconds += visibleXInterval) {
        const qreal x = plotArea.left() + (timeMicroseconds / maxX) * plotArea.width();
        painter.drawLine(QPointF(x, plotArea.top()), QPointF(x, plotArea.bottom()));
    }
}

void SignalGraphRenderer::drawYGrid(QPainter &painter, const QRectF &plotArea, double minY, double maxY,
    double visibleYInterval, const QPen &gridPen) const {
    const double firstYTick = std::ceil(minY / visibleYInterval);
    const double lastYTick = std::floor(maxY / visibleYInterval);
    painter.setPen(gridPen);
    for (double tick = firstYTick; tick <= lastYTick; ++tick) {
        const double amplitude = tick * visibleYInterval;
        const qreal y = plotArea.bottom() - ((amplitude - minY) / (maxY - minY)) * plotArea.height();
        painter.drawLine(QPointF(plotArea.left(), y), QPointF(plotArea.right(), y));
    }
}

void SignalGraphRenderer::drawAxes(QPainter &painter, const QRectF &plotArea) const {
    const QPen axisPen(QColor("#4d5560"), 1.5);
    painter.setPen(axisPen);
    painter.drawLine(QPointF(plotArea.left(), plotArea.center().y()),
                     QPointF(plotArea.right(), plotArea.center().y())
    );
    painter.drawLine(QPointF(plotArea.left(), plotArea.top()),
                     QPointF(plotArea.left(), plotArea.bottom())
    );
}

void SignalGraphRenderer::updateEnvelope(int width, int visibleSampleCount,
    const QVector<double> &samples) {
    if (m_envelopeWidth == width && m_envelopeSampleCount == visibleSampleCount) {
        return;
    }

    m_envelopeWidth = width;
    m_envelopeSampleCount = visibleSampleCount;
    m_envelope.clear();
    m_envelope.resize(width);
    for (int index = 0; index < visibleSampleCount; ++index) {
        const int column = static_cast<int>(
            static_cast<qint64>(index) * width / visibleSampleCount);
        EnvelopeBucket &bucket = m_envelope[column];
        const double sample = samples[index];
        if (bucket.minIndex < 0 || sample < samples[bucket.minIndex]) {
            bucket.minIndex = index;
        }
        if (bucket.maxIndex < 0 || sample > samples[bucket.maxIndex]) {
            bucket.maxIndex = index;
        }
    }
}

void SignalGraphRenderer::drawSamples(QPainter &painter, const QRectF &plotArea,
    double samplePeriodMicroseconds, double maxX, double minY, double maxY,
    const QVector<double> &samples) const {
    QPainterPath path;
    bool hasPoint = false;
    for (const EnvelopeBucket &bucket : m_envelope) {
        const int firstIndex = std::min(bucket.minIndex, bucket.maxIndex);
        const int lastIndex = std::max(bucket.minIndex, bucket.maxIndex);
        const int indices[] = { firstIndex, lastIndex };
        for (int point = 0; point < 2; ++point) {
            const int index = indices[point];
            if (index < 0 || (point == 1 && firstIndex == lastIndex)) {
                continue;
            }
            const double sampleTimeMicroseconds = index * samplePeriodMicroseconds;
            const double normalizedX = sampleTimeMicroseconds / maxX;
            const double normalizedY = (samples[index] - minY) / (maxY - minY);
            const qreal x = plotArea.left() + normalizedX * plotArea.width();
            const qreal y = plotArea.bottom() - normalizedY * plotArea.height();
            const QPointF position(x, y);
            if (!hasPoint) {
                path.moveTo(position);
                hasPoint = true;
            } else {
                path.lineTo(position);
            }
        }
    }

    painter.setPen(QPen(QColor("#3478c5"), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawPath(path);
}
