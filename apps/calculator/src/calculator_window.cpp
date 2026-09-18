#include "calculator_window.h"

#include "calculator.h"
#include "calculator_ui.h"

// 接続処理
CalculatorWindow::CalculatorWindow(QWidget *parent)
    : QWidget(parent), ui_(new Ui::CalculatorUi), logic_(new CalculatorLogic) {
    ui_->setupUi(this);
    connectDigitButtons();
    connectOperatorButtons();
    connectControlButtons();
    updateDisplay();
}

void CalculatorWindow::connectDigitButtons() {
    const QList<QPushButton *> digitButtons = {
        ui_->button7, ui_->button8, ui_->button9,
        ui_->button4, ui_->button5, ui_->button6,
        ui_->button1, ui_->button2, ui_->button3,
        ui_->button0
    };
    for (QPushButton *button : digitButtons) {
        connect(button, &QPushButton::clicked, this, &CalculatorWindow::handleDigitClicked);
    }
}

void CalculatorWindow::connectOperatorButtons() {
    const QList<QPushButton *> operatorButtons = {
        ui_->buttonDivide, ui_->buttonMultiply,
        ui_->buttonSubtract, ui_->buttonAdd
    };
    for (QPushButton *button : operatorButtons) {
        connect(button, &QPushButton::clicked, this, &CalculatorWindow::handleOperatorClicked);
    }
}

void CalculatorWindow::connectControlButtons() {
    // 処理が別のため個別に接続
    connect(ui_->buttonClear, &QPushButton::clicked, this, &CalculatorWindow::handleClearClicked);
    connect(ui_->buttonEquals, &QPushButton::clicked, this, &CalculatorWindow::handleEqualsClicked);
    connect(ui_->buttonPoint, &QPushButton::clicked, this, &CalculatorWindow::handlePointClicked);
    connect(ui_->buttonBackSpace, &QPushButton::clicked, this, &CalculatorWindow::handleBackSpaceClicked);
    connect(ui_->buttonParenthesis, &QPushButton::clicked, this, &CalculatorWindow::handleParenthesisClicked);
    connect(ui_->buttonSign, &QPushButton::clicked, this, &CalculatorWindow::handleSignClicked);
}

CalculatorWindow::~CalculatorWindow() {
    delete logic_;
    delete ui_;
}

// 数字を渡す
void CalculatorWindow::handleDigitClicked() {
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (button) {
        logic_->appendDigit(button->text());
        updateDisplay();
    }
}

// 演算子を渡す
void CalculatorWindow::handleOperatorClicked() {
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (button) {
        logic_->setOperator(button->text());
        updateDisplay();
    }
}

// 小数点を入力
void CalculatorWindow::handlePointClicked() {
    logic_->appendPoint();
    updateDisplay();
}

// 括弧を追加
void CalculatorWindow::handleParenthesisClicked() {
    logic_->appendParenthesis();
    updateDisplay();
}

// 数値の正負を切り替え
void CalculatorWindow::handleSignClicked() {
    logic_->toggleSign();
    updateDisplay();
}

// 計算を実行
void CalculatorWindow::handleEqualsClicked() {
    logic_->calculate();
    updateDisplay();
}

// 1文字削除
void CalculatorWindow::handleBackSpaceClicked() {
    logic_->backspace();
    updateDisplay();
}

// 初期化
void CalculatorWindow::handleClearClicked() {
    logic_->clear();
    updateDisplay();
}

// 計算ロジックが返した文字列を表示欄へ反映する。
void CalculatorWindow::updateDisplay() {
    ui_->display->setText(logic_->displayText());
}
