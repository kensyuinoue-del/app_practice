#include "signal_analyzer.h"

#include <QFrame>
#include <QGridLayout>
#include <QWidget>

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
	QFrame *graphArea = new QFrame;
	graphArea->setStyleSheet(
		"QFrame { background-color: #f0efef; border: 2px solid #888181; }"
	);

	// グラフ領域を左上のセルに配置し、中央ウィジェットをメインウィンドウに設定する。
	layout->addWidget(graphArea, 0, 0);
	setCentralWidget(centralWidget);
}
