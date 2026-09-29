#include "signal_analyzer.h"

#include <QFrame>
#include <QGridLayout>
#include <QPainter>
#include <QWidget>

#include <array>
#include <cmath>

class GraphArea : public QFrame {
protected:
	void paintEvent(QPaintEvent *event) override {
		QFrame::paintEvent(event);

		// アンチエイリアスを有効にして、グラフの線の描画設定をする。
		QPainter painter(this);
		painter.setRenderHint(QPainter::Antialiasing);
		painter.setPen(QPen(QColor("#3478c5"), 3, Qt::SolidLine, 
							Qt::RoundCap, Qt::RoundJoin));

		// グラフの描画領域を決定する。
		const QRectF contents(contentsRect());
		const QRectF area = contents.adjusted(24, 24, -24, -24);
		if (m_plotSize.isEmpty()) {
			if (area.width() <= 0 || area.height() <= 0) {
				return;
			}

			// グラフの描画領域のアスペクト比を4:3に固定する。
			constexpr qreal plotAspectRatio = 4.0 / 3.0;
			QRectF initialPlotArea = area;
			if (area.width() / area.height() > plotAspectRatio) {
				const qreal plotWidth = area.height() * plotAspectRatio;
				initialPlotArea.setLeft(area.center().x() - plotWidth / 2.0);
				initialPlotArea.setRight(area.center().x() + plotWidth / 2.0);
			} else {
				const qreal plotHeight = area.width() / plotAspectRatio;
				initialPlotArea.setTop(area.center().y() - plotHeight / 2.0);
				initialPlotArea.setBottom(area.center().y() + plotHeight / 2.0);
			}
			m_plotSize = initialPlotArea.size();
		}

		// グラフの描画領域を決定し、描画領域をクリップする。
		QRectF plotArea(QPointF(0, 0), m_plotSize);
		plotArea.moveCenter(contents.center());
		painter.setClipRect(contents);

		// グラフの描画
		const qreal top = plotArea.top() + plotArea.height() * 0.15;
		const qreal bottom = plotArea.bottom() - plotArea.height() * 0.15;
		const qreal radius = qMin(plotArea.width() / 2.0, bottom - top);
		constexpr int pointCount = 65;
		std::array<QPointF, pointCount> points;
		for (int index = 0; index < pointCount; ++index) {
			const qreal t = static_cast<qreal>(index) / (pointCount - 1);
			const qreal x = plotArea.left() + t * plotArea.width();
			const qreal normalizedX = 2.0 * t - 1.0;
			const qreal y = top + radius * std::sqrt(1.0 - normalizedX * normalizedX);
			points[index] = QPointF(x, y);
		}
		painter.drawPolyline(points.data(), static_cast<int>(points.size()));
	}

private:
	QSizeF m_plotSize;
};

SignalAnalyzer::SignalAnalyzer(QWidget *parent)
	: QMainWindow(parent) {

	// ウィンドウの中央に、子ウィジェットを配置するための領域を作る。
	QWidget *centralWidget = new QWidget(this);
	QGridLayout *layout = new QGridLayout(centralWidget);

	const double Stretch = 7.5;	// ウェジットの比率

	layout->setRowStretch(0, Stretch);	// ウェジットの高さ比率
	layout->setColumnStretch(0, Stretch);	// ウェジットの幅比率
	layout->setRowStretch(1, 10-Stretch);	// 余白の高さ比率(ウェジットの高さ比率に合わせ一意に決定)
	layout->setColumnStretch(1, 10-Stretch);	// 余白の幅比率(ウェジットの幅比率に合わせ一意に決定)

	// グラフを描画する領域として、背景と枠線を持つフレームを作る。
	QFrame *graphArea = new GraphArea;
	graphArea->setStyleSheet(
		"QFrame { background-color: #f0efef; border: 2px solid #888181; }"
	);

	// グラフ領域を左上のセルに配置し、中央ウィジェットをメインウィンドウに設定する。
	layout->addWidget(graphArea, 0, 0);
	setCentralWidget(centralWidget);
}
