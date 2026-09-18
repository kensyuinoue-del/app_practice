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
    void handlePointClicked();
    void handleBackSpaceClicked();
    void handleParenthesisClicked();
    void handleSignClicked();
    void handleOperatorClicked();
    void handleEqualsClicked();
    void handleClearClicked();

private:
    void connectDigitButtons();
    void connectOperatorButtons();
    void connectControlButtons();
    void executeOperation(void (CalculatorLogic::*operation)());
    void updateDisplay();

    Ui::CalculatorUi *ui_;
    CalculatorLogic *logic_;
};
