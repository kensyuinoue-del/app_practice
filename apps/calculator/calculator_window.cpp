#include "calculator_window.h"

#include "calculator.h"
#include "calculator_ui.h"

CalculatorWindow::CalculatorWindow(QWidget *parent)
    : QWidget(parent), ui_(new Ui::CalculatorUi), logic_(new CalculatorLogic) {
    // UI部品を作成する。
    ui_->setupUi(this);

    // 数字ボタンを、数字入力用の処理へ接続する。
    const QList<QPushButton *> digitButtons = {
        ui_->button7, ui_->button8, ui_->button9,
        ui_->button4, ui_->button5, ui_->button6,
        ui_->button1, ui_->button2, ui_->button3,
        ui_->button0
    };
    for (QPushButton *button : digitButtons) {
        connect(button, &QPushButton::clicked, this, &CalculatorWindow::handleDigitClicked);
    }

    // 演算子ボタンを、演算子処理用の処理へ接続する。
    const QList<QPushButton *> operatorButtons = {
        ui_->buttonDivide, ui_->buttonMultiply,
        ui_->buttonSubtract, ui_->buttonAdd
    };
    for (QPushButton *button : operatorButtons) {
        connect(button, &QPushButton::clicked, this, &CalculatorWindow::handleOperatorClicked);
    }

    // クリアボタンとイコールボタンを、それぞれの処理へ接続する。
    connect(ui_->buttonClear, &QPushButton::clicked, this, &CalculatorWindow::handleClearClicked);
    connect(ui_->buttonEquals, &QPushButton::clicked, this, &CalculatorWindow::handleEqualsClicked);

    updateDisplay();
}

// UI部品と計算ロジック用のオブジェクトを解放する。
CalculatorWindow::~CalculatorWindow() {
    delete logic_;
    delete ui_;
}

// 押された数字を計算ロジックへ渡し、表示を更新する。
void CalculatorWindow::handleDigitClicked() {
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (button) {
        logic_->appendDigit(button->text());
        updateDisplay();
    }
}

// 押された演算子を計算ロジックへ渡し、表示を更新する。
void CalculatorWindow::handleOperatorClicked() {
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (button) {
        logic_->setOperator(button->text());
        updateDisplay();
    }
}

// 計算ロジックに計算を実行させ、結果を表示する。
void CalculatorWindow::handleEqualsClicked() {
    logic_->calculate();
    updateDisplay();
}

// 計算ロジックの状態を初期化し、表示を更新する。
void CalculatorWindow::handleClearClicked() {
    logic_->clear();
    updateDisplay();
}

// 計算ロジックが返した文字列を表示欄へ反映する。
void CalculatorWindow::updateDisplay() {
    ui_->display->setText(logic_->displayText());
}
