#include "calculator_window.h"

#include "calculator.h"
#include "calculator_ui.h"

CalculatorWindow::CalculatorWindow(QWidget *parent)
    : QWidget(parent), ui_(new Ui::CalculatorUi), logic_(new CalculatorLogic) {
    ui_->setupUi(this);

    const QList<QPushButton *> digitButtons = {
        ui_->button7, ui_->button8, ui_->button9,
        ui_->button4, ui_->button5, ui_->button6,
        ui_->button1, ui_->button2, ui_->button3,
        ui_->button0
    };
    for (QPushButton *button : digitButtons) {
        connect(button, &QPushButton::clicked, this, &CalculatorWindow::handleDigitClicked);
    }

    const QList<QPushButton *> operatorButtons = {
        ui_->buttonDivide, ui_->buttonMultiply,
        ui_->buttonSubtract, ui_->buttonAdd
    };
    for (QPushButton *button : operatorButtons) {
        connect(button, &QPushButton::clicked, this, &CalculatorWindow::handleOperatorClicked);
    }

    connect(ui_->buttonClear, &QPushButton::clicked, this, &CalculatorWindow::handleClearClicked);
    connect(ui_->buttonEquals, &QPushButton::clicked, this, &CalculatorWindow::handleEqualsClicked);

    updateDisplay();
}

CalculatorWindow::~CalculatorWindow() {
    delete logic_;
    delete ui_;
}

void CalculatorWindow::handleDigitClicked() {
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (button) {
        logic_->appendDigit(button->text());
        updateDisplay();
    }
}

void CalculatorWindow::handleOperatorClicked() {
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (button) {
        logic_->setOperator(button->text());
        updateDisplay();
    }
}

void CalculatorWindow::handleEqualsClicked() {
    logic_->calculate();
    updateDisplay();
}

void CalculatorWindow::handleClearClicked() {
    logic_->clear();
    updateDisplay();
}

void CalculatorWindow::updateDisplay() {
    ui_->display->setText(logic_->displayText());
}
