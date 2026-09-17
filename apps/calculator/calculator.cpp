#include "calculator.h"

void CalculatorLogic::appendDigit(const QString &digit) {
    if (digit == "0" && currentInput_ == "0") {
        return;
    }

    if (currentInput_ == "0") {
        currentInput_.clear();
    }

    currentInput_ += digit;
}

void CalculatorLogic::setOperator(const QString &op) {
    if (currentInput_.isEmpty() && !hasStoredValue_) {
        return;
    }

    if (hasStoredValue_ && !currentInput_.isEmpty() && !pendingOperator_.isEmpty()) {
        storedValue_ = applyOperation(storedValue_, currentInput_.toDouble(), pendingOperator_);
        currentInput_.clear();
    }

    if (!currentInput_.isEmpty()) {
        storedValue_ = currentInput_.toDouble();
        currentInput_.clear();
        hasStoredValue_ = true;
    }

    pendingOperator_ = op;
}

void CalculatorLogic::calculate() {
    if (!hasStoredValue_ || pendingOperator_.isEmpty() || currentInput_.isEmpty()) {
        return;
    }

    storedValue_ = applyOperation(storedValue_, currentInput_.toDouble(), pendingOperator_);
    currentInput_.clear();
    pendingOperator_.clear();
    hasStoredValue_ = true;
}

void CalculatorLogic::clear() {
    currentInput_.clear();
    pendingOperator_.clear();
    storedValue_ = 0.0;
    hasStoredValue_ = false;
}

QString CalculatorLogic::displayText() const {
    if (!currentInput_.isEmpty()) {
        return currentInput_;
    }

    return hasStoredValue_ ? QString::number(storedValue_) : "0";
}

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
        if (qFuzzyIsNull(right)) {
            return 0.0;
        }
        return left / right;
    }
    return right;
}
