#include "calculator_window.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    // Qtアプリケーションを作成し、イベント処理を開始する。
    QApplication app(argc, argv);

    // 計算機ウィンドウを作成して、サイズとタイトルを設定する。
    CalculatorWindow calculator;
    calculator.setWindowTitle("Calculator");
    calculator.resize(300, 420);
    calculator.show();

    // ボタン操作などのイベントを待ち受ける。
    return app.exec();
}
