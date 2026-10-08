#include "signal_analyzer.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QWidget>

#include <algorithm>

class GraphArea : public QFrame {
public:
    void setSamples(const QVector<double> &samples) {
        m_samples = samples;
        if (!m_samples.isEmpty()) {
            const auto minmax = std::minmax_element(m_samples.begin(), m_samples.end());
            m_minAmplitude = *minmax.first;
            m_maxAmplitude = *minmax.second;
        } else {
            m_minAmplitude = 0.0;
            m_maxAmplitude = 0.0;
        }
        update();
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        QFrame::paintEvent(event);

        const QRectF contents(contentsRect());
        if (contents.width() <= 0 || contents.height() <= 0) {
            return;
        }

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setClipRect(contents);

        const qreal plotMargin = 20.0;
        const QRectF plotArea(contents.adjusted(plotMargin, plotMargin, -plotMargin, -plotMargin));

        painter.setBrush(QBrush(QColor("#ffffff")));
        painter.setPen(Qt::NoPen);
        painter.drawRect(plotArea);

        painter.setPen(QPen(QColor("#d0d6dc"), 1, Qt::DashLine));
        constexpr int verticalDivisions = 10;
        constexpr int horizontalDivisions = 8;
        const qreal gridSpacingX = plotArea.width() / verticalDivisions;
        const qreal gridSpacingY = plotArea.height() / horizontalDivisions;
        for (int index = 0; index <= verticalDivisions; ++index) {
            const qreal x = plotArea.left() + index * gridSpacingX;
            painter.drawLine(QPointF(x, plotArea.top()), QPointF(x, plotArea.bottom()));
        }
        for (int index = 0; index <= horizontalDivisions; ++index) {
            const qreal y = plotArea.top() + index * gridSpacingY;
            painter.drawLine(QPointF(plotArea.left(), y), QPointF(plotArea.right(), y));
        }

        const QPen axisPen(QColor("#4d5560"), 1.5);
        painter.setPen(axisPen);
        painter.drawLine(QPointF(plotArea.left(), plotArea.center().y()),
                         QPointF(plotArea.right(), plotArea.center().y()));
        painter.drawLine(QPointF(plotArea.center().x(), plotArea.top()),
                         QPointF(plotArea.center().x(), plotArea.bottom()));

        if (m_samples.isEmpty()) {
            painter.setPen(QColor("#556070"));
            painter.setFont(QFont(QStringLiteral("Sans Serif"), 10));
            painter.drawText(contents, Qt::AlignCenter, QStringLiteral("表示するデータがありません"));
            return;
        }

        const double minVoltage = std::min(0.0, m_minAmplitude);
        const double maxVoltage = std::max(0.0, m_maxAmplitude);
        const double voltageRange = std::max(1e-6, maxVoltage - minVoltage);

        QVector<QPointF> points;
        points.reserve(m_samples.size());
        for (int index = 0; index < m_samples.size(); ++index) {
            const double normalizedX = m_samples.size() > 1 ? static_cast<double>(index) / (m_samples.size() - 1) : 0.0;
            const double normalizedY = (m_samples[index] - minVoltage) / voltageRange;
            const qreal x = plotArea.left() + normalizedX * plotArea.width();
            const qreal y = plotArea.bottom() - normalizedY * plotArea.height();
            points.append(QPointF(x, y));
        }

        QPainterPath path;
        if (!points.isEmpty()) {
            path.moveTo(points.first());
            for (int index = 1; index < points.size(); ++index) {
                path.lineTo(points.at(index));
            }
        }

        painter.setPen(QPen(QColor("#3478c5"), 2, Qt::SolidLine,
                            Qt::RoundCap, Qt::RoundJoin));
        painter.drawPath(path);
    }

private:
    QVector<double> m_samples;
    double m_minAmplitude = 0.0;
    double m_maxAmplitude = 0.0;
};

SignalAnalyzer::SignalAnalyzer(QWidget *parent)
    : QMainWindow(parent) {
    QWidget *centralWidget = new QWidget(this);
    auto *rootLayout = new QHBoxLayout(centralWidget);
    const int margin = 8;
    const int widgetSpacing = 8;
    rootLayout->setContentsMargins(margin, margin, margin, margin); // ウィンドウの端からの余白をmargin分のピクセルに設定
    rootLayout->setSpacing(widgetSpacing);  // ウィジェット間のスペースをwidgetSpacing分のピクセルに設定

    auto *graphArea = new GraphArea;
    graphArea->setStyleSheet(
        "QFrame { background-color: #ffffff; border: 2px solid #888181; }"
    );
    const int minHeight = 320;
    graphArea->setMinimumHeight(minHeight);
    rootLayout->addWidget(graphArea);
    setCentralWidget(centralWidget);
}
