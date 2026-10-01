#include "waveform_preview.h"

#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>

#include <algorithm>
#include <cmath>

WaveformPreview::WaveformPreview(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(300, 170);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void WaveformPreview::setSignal(wave::Waveform newWaveform, double newFrequency,
    double newDuration, double newAmplitude)
{
    waveform = newWaveform;
    frequency = newFrequency;
    duration = newDuration;
    amplitude = newAmplitude;
    update();
}

void WaveformPreview::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(QStringLiteral("#f7f9f8")));

    const QRectF availableRect(54.0, 30.0, width() - 72.0, height() - 58.0);
    if (availableRect.width() <= 0.0 || availableRect.height() <= 0.0) {
        return;
    }

    constexpr qreal plotAspectRatio = 4.0;
    qreal plotWidth = availableRect.width();
    qreal plotHeight = plotWidth / plotAspectRatio;
    if (plotHeight > availableRect.height()) {
        plotHeight = availableRect.height();
        plotWidth = plotHeight * plotAspectRatio;
    }
    const QRectF plotRect(
        availableRect.center().x() - plotWidth / 2.0,
        availableRect.center().y() - plotHeight / 2.0,
        plotWidth,
        plotHeight);

    painter.setFont(QFont(painter.font().family(), 9));
    painter.setPen(QColor(QStringLiteral("#aebbb5")));
    for (int division = 0; division <= 4; ++division) {
        const qreal x = plotRect.left() + plotRect.width() * division / 4.0;
        const qreal y = plotRect.top() + plotRect.height() * division / 4.0;
        painter.drawLine(QPointF(x, plotRect.top()), QPointF(x, plotRect.bottom()));
        painter.drawLine(QPointF(plotRect.left(), y), QPointF(plotRect.right(), y));
    }

    painter.setPen(QColor(QStringLiteral("#596962")));
    painter.drawText(QRectF(2.0, plotRect.top() - 8.0, 48.0, 16.0), Qt::AlignRight | Qt::AlignVCenter,
        QStringLiteral("+1.0"));
    painter.drawText(QRectF(2.0, plotRect.center().y() - 8.0, 48.0, 16.0), Qt::AlignRight | Qt::AlignVCenter,
        QStringLiteral("0"));
    painter.drawText(QRectF(2.0, plotRect.bottom() - 8.0, 48.0, 16.0), Qt::AlignRight | Qt::AlignVCenter,
        QStringLiteral("-1.0"));

    constexpr double previewWindowSeconds = 0.02;
    const double displayDuration = std::min(duration, previewWindowSeconds);
    const int pointCount = qBound(100, static_cast<int>(plotRect.width() * 2.0), 2000);
    QPainterPath waveformPath;
    for (int point = 0; point < pointCount; ++point) {
        const double progress = static_cast<double>(point) / (pointCount - 1);
        const double phase = std::fmod(frequency * displayDuration * progress, 1.0);
        const double sample = amplitude * wave::sampleValue(waveform, phase);
        const QPointF position(
            plotRect.left() + plotRect.width() * progress,
            plotRect.center().y() - sample * plotRect.height() / 2.0);
        if (point == 0) {
            waveformPath.moveTo(position);
        } else {
            waveformPath.lineTo(position);
        }
    }

    painter.save();
    painter.setClipRect(plotRect);
    painter.setPen(QPen(QColor(QStringLiteral("#168b78")), 2.0));
    painter.drawPath(waveformPath);
    painter.restore();

    painter.setPen(QColor(QStringLiteral("#596962")));
    painter.drawText(QRectF(plotRect.left(), plotRect.bottom() + 4.0, plotRect.width(), 18.0),
        Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral("0 ms"));
    painter.drawText(QRectF(plotRect.left(), plotRect.bottom() + 4.0, plotRect.width(), 18.0),
        Qt::AlignRight | Qt::AlignVCenter,
        QStringLiteral("%1 ms").arg(displayDuration * 1000.0, 0, 'f', 2));
}