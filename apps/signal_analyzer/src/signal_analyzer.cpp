#include "signal_analyzer.h"

#include <QFrame>
#include <QGridLayout>
#include <QWidget>

SignalAnalyzer::SignalAnalyzer(QWidget *parent)
	: QMainWindow(parent) {

	// ウィンドウの中央に、子ウィジェットを配置するための領域を作る。
	QWidget *centralWidget = new QWidget(this);
	QGridLayout *layout = new QGridLayout(centralWidget);
	layout->setSpacing(0);
	// 行を9:3、列を9:3の比率で分割し、左上のセルを広く確保する。
	// この比率では左上セルは各方向で約75%になる。
	layout->setRowStretch(0, 9);
	layout->setRowStretch(1, 3);
	layout->setColumnStretch(0, 9);
	layout->setColumnStretch(1, 3);

	// グラフを描画する領域として、白背景と枠線を持つフレームを作る。
	QFrame *graphArea = new QFrame;
	graphArea->setStyleSheet(
		"QFrame { background-color: white; border: 2px solid #888181; }"
	);
	// グラフ領域を左上のセルに配置し、中央ウィジェットをメインウィンドウに設定する。
	layout->addWidget(graphArea, 0, 0);
	setCentralWidget(centralWidget);
}
