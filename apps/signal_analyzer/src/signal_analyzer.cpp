#include "signal_analyzer.h"

#include <QFrame>
#include <QGridLayout>
#include <QPainter>
#include <QWidget>

#include <array>
#include <cmath>

// グラフの領域を定義し描画するクラス
class GraphArea : public QFrame {
protected:
	void paintEvent(QPaintEvent *event) override {
		QFrame::paintEvent(event);

		// 描画領域の取得、描画領域が空の場合は描画を行わない
		const QRectF contents(contentsRect());
		if (contents.width() <= 0 || contents.height() <= 0) {
			return;
		}

		// グラフエリアの設定
		QPainter painter(this);
		painter.setRenderHint(QPainter::Antialiasing);
		if (m_plotSize.isEmpty()) {
			m_plotSize = contents.size();
		}
		QRectF plotArea(QPointF(0, 0), m_plotSize);
		plotArea.moveCenter(contents.center());
		painter.setClipRect(contents);

		// 描画領域の座標を取得
		const qreal top = plotArea.top();
		const qreal bottom = plotArea.bottom();
		const qreal centerY = plotArea.center().y();
		const qreal centerX = plotArea.center().x();

		// グリッドの設定
		constexpr int verticalDivisions = 10;
		constexpr int horizontalDivisions = 8;
		const qreal gridSpacingX = m_plotSize.width() / verticalDivisions;
		const qreal gridSpacingY = m_plotSize.height() / horizontalDivisions;
		const qreal firstVertical = centerX - gridSpacingX * std::ceil(
			(centerX - contents.left()) / gridSpacingX);
		const qreal firstHorizontal = centerY - gridSpacingY * std::ceil(
			(centerY - contents.top()) / gridSpacingY);

		// グリッドの描画
		painter.setPen(QPen(QColor("#c6cbd1"), 1, Qt::DashLine));
		for (qreal x = firstVertical; x <= contents.right(); x += gridSpacingX) {
			painter.drawLine(QPointF(x, contents.top()), QPointF(x, contents.bottom()));
		}
		for (qreal y = firstHorizontal; y <= contents.bottom(); y += gridSpacingY) {
			painter.drawLine(QPointF(contents.left(), y), QPointF(contents.right(), y));
		}

		// 軸の描画設定
		const QPen axisPen(QColor("#737b84"), 1.5);
		const QPen axisLabelPen(QColor("#555d66"));

		// 軸の描画
		painter.setPen(axisPen);
		painter.drawLine(QPointF(contents.left(), centerY), QPointF(contents.right(), centerY));
		painter.drawLine(QPointF(centerX, contents.top()), QPointF(centerX, contents.bottom()));
		painter.setPen(axisLabelPen);
		painter.drawText(QRectF(contents.right() - 48, contents.bottom() - 24, 48, 20),
						Qt::AlignRight, "時間");
		painter.drawText(QRectF(contents.left() + 6, contents.top() + 4, 36, 20),
						Qt::AlignLeft, "電圧");

		// 波形の描画設定
		painter.setPen(QPen(QColor("#3478c5"), 3, Qt::SolidLine,
							Qt::RoundCap, Qt::RoundJoin));

		// 波形の描画
		const qreal amplitude = (bottom - top) / 2.0;
		constexpr int pointCount = 129;
		std::array<QPointF, pointCount> points;
		for (int index = 0; index < pointCount; ++index) {
			const qreal t = static_cast<qreal>(index) / (pointCount - 1);
			const qreal x = plotArea.left() + t * plotArea.width();
			const qreal phase = 2.0 * M_PI * 2.0 * t;
			const qreal y = centerY - amplitude * std::sin(phase);
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
