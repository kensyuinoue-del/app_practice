#include "calculator.h"

#include <QChar>
#include <QRegularExpression>

namespace {
    class ExpressionParser {
        public:
            explicit ExpressionParser(const QString &expression) : expression_(expression) {}

            bool parse(double &result) {
                position_ = 0;
                if (!parseExpression(result)) {
                    return false;
                }
                skipSpaces();
                if (position_ != expression_.size()) {
                    return false;
                }
                return true;
            }

        private:
            void skipSpaces() {
                while (position_ < expression_.size() && expression_[position_].isSpace()) {
                    ++position_;
                }
            }

            bool parseExpression(double &result) {
                if (!parseTerm(result)) {
                    return false;
                }
                while (true) {
                    skipSpaces();
                    if (position_ >= expression_.size() || (expression_[position_] != '+' && expression_[position_] != '-')) {
                        return true;
                    }
                    const QChar op = expression_[position_++];
                    double right = 0.0;
                    if (!parseTerm(right)) {
                        return false;
                    }
                    result = op == '+' ? result + right : result - right;
                }
            }

            bool parseTerm(double &result) {
                if (!parseFactor(result)) {
                    return false;
                }
                while (true) {
                    skipSpaces();
                    if (position_ >= expression_.size() || (expression_[position_] != '*' && expression_[position_] != '/')) {
                        return true;
                    }
                    const QChar op = expression_[position_++];
                    double right = 0.0;
                    if (!parseFactor(right)) {
                        return false;
                    }
                    if (op == '/' && qFuzzyIsNull(right)) {
                        return false;
                    }
                    result = op == '*' ? result * right : result / right;
                }
            }

            bool parseFactor(double &result) {
                skipSpaces();
                if (position_ >= expression_.size()) {
                    return false;
                }
                if (expression_[position_] == '+') {
                    ++position_;
                    return parseFactor(result);
                }
                if (expression_[position_] == '-') {
                    ++position_;
                    if (!parseFactor(result)) {
                        return false;
                    }
                    result = -result;
                    return true;
                }
                if (expression_[position_] == '(') {
                    ++position_;
                    if (!parseExpression(result)) {
                        return false;
                    }
                    skipSpaces();
                    if (position_ >= expression_.size() || expression_[position_] != ')') {
                        return false;
                    }
                    ++position_;
                    return true;
                }

                const int start = position_;
                while (position_ < expression_.size() && (expression_[position_].isDigit() || expression_[position_] == '.')) {
                    ++position_;
                }
                if (start == position_) {
                    return false;
                }
                bool ok = false;
                result = expression_.mid(start, position_ - start).toDouble(&ok);
                return ok;
            }

            QString expression_;
            int position_ = 0;
    };
}

void CalculatorLogic::appendDigit(const QString &digit) {
    if (expression_.endsWith(')')) {
        return;
    }
    expression_ += digit;
}

void CalculatorLogic::appendPoint() {
    int start = expression_.lastIndexOf(QRegularExpression("[+\\-*/( ]")) + 1;
    const QString currentNumber = expression_.mid(start);
    if (!currentNumber.contains('.')) {
        expression_ += currentNumber.isEmpty() ? "0." : ".";
    }
}

void CalculatorLogic::backspace() {
    if (!expression_.isEmpty()) {
        expression_.chop(1);
        while (expression_.endsWith(' ')) {
            expression_.chop(1);
        }
    }
}

void CalculatorLogic::toggleSign() {
    int start = expression_.lastIndexOf(QRegularExpression("[+*/( ]")) + 1;
    // 現在の数値の先頭がマイナスなら削除し、なければ追加する。
    if (start < expression_.size() && expression_[start] == '-') {
        expression_.remove(start, 1);
    } else {
        expression_.insert(start, '-');
    }
}

void CalculatorLogic::appendParenthesis() {
    const int openCount = expression_.count('(');
    const int closeCount = expression_.count(')');
    if (openCount > closeCount && !expression_.isEmpty() && (expression_.back().isDigit() || expression_.back() == ')')) {
        expression_ += ')';
    } else if (expression_.isEmpty() || expression_.back() == '(' || expression_.back().isSpace() || expression_.back() == '+' || expression_.back() == '-' || expression_.back() == '*' || expression_.back() == '/') {
        expression_ += '(';
    }
}

void CalculatorLogic::setOperator(const QString &op) {
    if (expression_.isEmpty() || expression_.endsWith('(') || expression_.endsWith(' ')) {
        return;
    }
    while (expression_.endsWith(')') == false && expression_.endsWith(' ')) {
        expression_.chop(1);
    }
    expression_ += " " + op + " ";
}

void CalculatorLogic::calculate() {
    double result = 0.0;
    if (evaluateExpression(expression_, result)) {
        expression_ = QString::number(result);
    }
}

void CalculatorLogic::clear() {
    expression_.clear();
}

QString CalculatorLogic::displayText() const {
    return expression_.isEmpty() ? "0" : expression_;
}

bool CalculatorLogic::evaluateExpression(const QString &expression, double &result) const {
    return ExpressionParser(expression).parse(result);
}
