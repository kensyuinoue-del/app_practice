#pragma once

#include <QWidget>

namespace Ui {
class CalculatorUi;
}

class CalculatorLogic;

class CalculatorWindow : public QWidget {
    Q_OBJECT

public:
    explicit CalculatorWindow(QWidget *parent = nullptr);
    ~CalculatorWindow();

private slots:
    void handleDigitClicked();
    void handleOperatorClicked();
    void handleEqualsClicked();
    void handleClearClicked();

private:
    void updateDisplay();

    Ui::CalculatorUi *ui_;
    CalculatorLogic *logic_;
};
