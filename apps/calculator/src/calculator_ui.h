#pragma once

#include <QLineEdit>
#include <QPushButton>

class QWidget;

namespace Ui {

class CalculatorUi {
public:
    void setupUi(QWidget *calculator);

    QLineEdit *display = nullptr;

    QPushButton *button1 = nullptr;
    QPushButton *button2 = nullptr;
    QPushButton *button3 = nullptr;
    QPushButton *button4 = nullptr;
    QPushButton *button5 = nullptr;
    QPushButton *button6 = nullptr;
    QPushButton *button7 = nullptr;
    QPushButton *button8 = nullptr;
    QPushButton *button9 = nullptr;
    QPushButton *button0 = nullptr;

    QPushButton *buttonAdd = nullptr;
    QPushButton *buttonSubtract = nullptr;
    QPushButton *buttonMultiply = nullptr;
    QPushButton *buttonDivide = nullptr;

    QPushButton *buttonClear = nullptr;
    QPushButton *buttonBackSpace = nullptr;

    QPushButton *buttonEquals = nullptr;
    QPushButton *buttonPoint = nullptr;
    QPushButton *buttonParenthesis = nullptr;
    QPushButton *buttonSign = nullptr;
};

} // namespace Ui
