#include "calculator_ui.h"

#include <QGridLayout>
#include <QWidget>

namespace Ui {

void CalculatorUi::setupUi(QWidget *calculator) {
    // 表示欄とボタンを4列のグリッドへ配置する。
    QGridLayout *layout = new QGridLayout(calculator);

    //表示欄の定義
    display = new QLineEdit("0", calculator);
    display->setReadOnly(true);
    display->setAlignment(Qt::AlignRight);
    display->setStyleSheet("QLineEdit { font-size: 24px; }");
    layout->addWidget(display, 0, 0, 1, 4); //要素：ウェジット, 行, 列, 高さ, 幅

    auto createButton = [calculator, layout](const QString &text, int row, int column) {
        QPushButton *button = new QPushButton(text, calculator);
        button->setMinimumHeight(50);
        button->setStyleSheet("QPushButton { font-size: 18px; }");
        layout->addWidget(button, row, column);
        return button;
    };

    //ボタンの配置行列(表示する文字, 行, 列), 上記のautoで定義
    buttonClear = createButton("AC", 1, 0);
    buttonSign = createButton("±", 1, 1);
    buttonParenthesis = createButton("()", 1, 2);
    buttonMultiply = createButton("*", 1, 3);
    button1 = createButton("1", 2, 0);
    button2 = createButton("2", 2, 1);
    button3 = createButton("3", 2, 2);
    buttonDivide = createButton("/", 2, 3);
    button4 = createButton("4", 3, 0);
    button5 = createButton("5", 3, 1);
    button6 = createButton("6", 3, 2);
    buttonAdd = createButton("+", 3, 3);
    button7 = createButton("7", 4, 0);
    button8 = createButton("8", 4, 1);
    button9 = createButton("9", 4, 2);
    buttonSubtract = createButton("-", 4, 3);
    buttonPoint = createButton(".", 5, 0);
    button0 = createButton("0", 5, 1);
    buttonBackSpace = createButton("←", 5, 2);
    buttonEquals = createButton("=", 5, 3);
}

} // namespace Ui
