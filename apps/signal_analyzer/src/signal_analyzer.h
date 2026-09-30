#pragma once

#include <QMainWindow>

// グラフ表示領域を含むメインウィンドウ。
class SignalAnalyzer : public QMainWindow {
public:
	// ウィンドウ内のレイアウトとグラフ領域を初期化する。
	explicit SignalAnalyzer(QWidget *parent = nullptr);
};