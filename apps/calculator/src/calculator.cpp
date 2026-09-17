#include "calculator.h"

// 入力された数字を現在の入力値へ追加する。
void CalculatorLogic::appendDigit(const QString &digit) {
    // 0が連続して入力される場合は、余分な0を追加しない。
    if (digit == "0" && currentInput_ == "0") {
        return;
    }

    // 現在の入力が0なら、新しい数字で置き換える。
    if (currentInput_ == "0") {
        currentInput_.clear();
    }

    // 入力された数字を現在の入力値の末尾へ追加する。
    currentInput_ += digit;
}

// 演算子を保存し、必要に応じて途中の計算を実行する。
void CalculatorLogic::setOperator(const QString &op) {
    // 数字がまだ入力されていない場合は演算子を受け付けない。
    if (currentInput_.isEmpty() && !hasStoredValue_) {
        return;
    }

    // すでに保存された値と演算子がある場合は、途中結果を計算する。
    if (hasStoredValue_ && !currentInput_.isEmpty() && !pendingOperator_.isEmpty()) {
        storedValue_ = applyOperation(storedValue_, currentInput_.toDouble(), pendingOperator_);
        currentInput_.clear();
    }

    // 現在の入力値を、次の計算で使う値として保存する。
    if (!currentInput_.isEmpty()) {
        storedValue_ = currentInput_.toDouble();
        currentInput_.clear();
        hasStoredValue_ = true;
    }

    // 次に実行する演算子を保存する。
    pendingOperator_ = op;
}

// 保存されている値と現在の入力値を使って計算する。
void CalculatorLogic::calculate() {
    // 計算に必要な値や演算子が揃っていなければ何もしない。
    if (!hasStoredValue_ || pendingOperator_.isEmpty() || currentInput_.isEmpty()) {
        return;
    }

    // 計算結果を保存し、次の入力に備えて入力状態を整理する。
    storedValue_ = applyOperation(storedValue_, currentInput_.toDouble(), pendingOperator_);
    currentInput_.clear();
    pendingOperator_.clear();
    hasStoredValue_ = true;
}

// 計算機の入力値と計算状態を初期状態へ戻す。
void CalculatorLogic::clear() {
    currentInput_.clear();
    pendingOperator_.clear();
    storedValue_ = 0.0;
    hasStoredValue_ = false;
}

// 現在の状態を、画面に表示する文字列として返す。
QString CalculatorLogic::displayText() const {
    // 演算子が保存されている場合は、左辺・演算子・右辺を式として表示する。
    if (hasStoredValue_ && !pendingOperator_.isEmpty()) {
        const QString right = currentInput_.isEmpty() ? "" : " " + currentInput_;
        return QString::number(storedValue_) + " " + pendingOperator_ + right;
    }

    // 入力中の値があれば、それを優先して表示する。
    if (!currentInput_.isEmpty()) {
        return currentInput_;
    }

    return hasStoredValue_ ? QString::number(storedValue_) : "0";
}

// 指定された演算子で、2つの値の四則演算を行う。
double CalculatorLogic::applyOperation(double left, double right, const QString &op) const {
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
        // 0除算を避けるため、割る数が0なら0を返す。
        if (qFuzzyIsNull(right)) {
            return 0.0;
        }
        return left / right;
    }
    return right;
}
