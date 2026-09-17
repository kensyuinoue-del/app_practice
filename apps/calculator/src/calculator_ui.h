#pragma once

#include <QLineEdit>
#include <QPushButton>

class QWidget;

namespace Ui {

class CalculatorUi {
public:
    void setupUi(QWidget *calculator);

    QLineEdit *display = nullptr;
    QPushButton *button7 = nullptr;
    QPushButton *button8 = nullptr;
    QPushButton *button9 = nullptr;
    QPushButton *buttonDivide = nullptr;
    QPushButton *button4 = nullptr;
    QPushButton *button5 = nullptr;
    QPushButton *button6 = nullptr;
    QPushButton *buttonMultiply = nullptr;
    QPushButton *button1 = nullptr;
    QPushButton *button2 = nullptr;
    QPushButton *button3 = nullptr;
    QPushButton *buttonSubtract = nullptr;
    QPushButton *buttonClear = nullptr;
    QPushButton *button0 = nullptr;
    QPushButton *buttonEquals = nullptr;
    QPushButton *buttonAdd = nullptr;
};

} // namespace Ui
