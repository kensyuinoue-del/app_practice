#include "signal_analyzer.h"

#include <QFrame>
#include <QGridLayout>
#include <QWidget>

SignalAnalyzer::SignalAnalyzer(QWidget *parent)
	: QMainWindow(parent) {

	QWidget *centralWidget = new QWidget(this);
	QGridLayout *layout = new QGridLayout(centralWidget);
	layout->setSpacing(0);
	layout->setRowStretch(0, 9);
	layout->setRowStretch(1, 3);
	layout->setColumnStretch(0, 9);
	layout->setColumnStretch(1, 3);

	QFrame *graphArea = new QFrame;
	graphArea->setStyleSheet(
		"QFrame { background-color: white; border: 2px solid #888181; }"
	);
	layout->addWidget(graphArea, 0, 0);
	setCentralWidget(centralWidget);
}
