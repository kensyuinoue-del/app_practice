#include "calculator.h"
#include "calculator_ui.h"

#include <QString>

Calculator::Calculator(QWidget *parent)
    : QWidget(parent), ui_(new Ui::CalculatorUi) {
    // UI専用クラスに表示欄とボタンを生成させる。
    ui_->setupUi(this);

    // 各ボタンを、対応する計算処理へ接続する。
    const QList<QPushButton *> digitButtons = {
        ui_->button7, ui_->button8, ui_->button9,
        ui_->button4, ui_->button5, ui_->button6,
        ui_->button1, ui_->button2, ui_->button3,
        ui_->button0
    };
    for (QPushButton *button : digitButtons) {
        connect(button, &QPushButton::clicked, this, &Calculator::handleDigitClicked);
    }

    const QList<QPushButton *> operatorButtons = {
        ui_->buttonDivide, ui_->buttonMultiply,
        ui_->buttonSubtract, ui_->buttonAdd
    };
    for (QPushButton *button : operatorButtons) {
        connect(button, &QPushButton::clicked, this, &Calculator::handleOperatorClicked);
    }

    connect(ui_->buttonClear, &QPushButton::clicked, this, &Calculator::handleClearClicked);
    connect(ui_->buttonEquals, &QPushButton::clicked, this, &Calculator::handleEqualsClicked);

    updateDisplay();
}

Calculator::~Calculator() {
    delete ui_;
}

// 数字ボタンの文字を現在の入力値へ追加する。
void Calculator::handleDigitClicked() {
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (!button) {
        return;
    }

    appendDigit(button->text());
}

// 演算子ボタンを処理し、次に行う計算を記録する。
void Calculator::handleOperatorClicked() {
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (!button) {
        return;
    }

    const QString op = button->text();

    if (currentInput_.isEmpty() && !hasStoredValue_) {
        return;
    }

    if (hasStoredValue_ && !currentInput_.isEmpty() && !pendingOperator_.isEmpty()) {
        const double current = currentInput_.toDouble();
        const double result = applyOperation(storedValue_, current, pendingOperator_);
        storedValue_ = result;
        currentInput_.clear();
        ui_->display->setText(QString::number(result));
    }

    if (!currentInput_.isEmpty()) {
        storedValue_ = currentInput_.toDouble();
        currentInput_.clear();
        hasStoredValue_ = true;
    }

    pendingOperator_ = op;
    updateDisplay();
}

// 保留中の演算子を使って計算し、結果を表示する。
void Calculator::handleEqualsClicked() {
    if (!hasStoredValue_ || pendingOperator_.isEmpty() || currentInput_.isEmpty()) {
        return;
    }

    const double result = applyOperation(storedValue_, currentInput_.toDouble(), pendingOperator_);
    ui_->display->setText(QString::number(result));
    storedValue_ = result;
    currentInput_.clear();
    pendingOperator_.clear();
    hasStoredValue_ = true;
}

// 入力値・演算子・保存値をすべて初期状態へ戻す。
void Calculator::handleClearClicked() {
    currentInput_.clear();
    pendingOperator_.clear();
    storedValue_ = 0.0;
    hasStoredValue_ = false;
    ui_->display->setText("0");
}

// 数字を現在の入力値へ追加する。先頭の余分な0も防ぐ。
void Calculator::appendDigit(const QString &digit) {
    if (digit == "0" && currentInput_ == "0") {
        return;
    }

    if (currentInput_ == "0") {
        currentInput_.clear();
    }

    currentInput_ += digit;
    updateDisplay();
}

// 現在の入力状態に合わせて表示欄の内容を更新する。
void Calculator::updateDisplay() {
    if (currentInput_.isEmpty()) {
        ui_->display->setText(hasStoredValue_ ? QString::number(storedValue_) : "0");
        return;
    }

    ui_->display->setText(currentInput_);
}

// 演算子に応じた四則演算を行う。0除算は0として扱う。
double Calculator::applyOperation(double left, double right, const QString &op) const {
    if (op == "+") {
        return left + right;
    }
    if (op == "-") {
        return left - right;
    }
    if (op == "*") {
        return left * right;
    }
    if (op == "/") {
        if (qFuzzyIsNull(right)) {
            return 0.0;
        }
        return left / right;
    }
    return right;
}
