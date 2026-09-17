#pragma once

#include <QString>

class CalculatorLogic {
public:
    void appendDigit(const QString &digit);
    void appendPoint();
    void backspace();
    void toggleSign();
    void appendParenthesis();
    void setOperator(const QString &op);
    void calculate();
    void clear();
    QString displayText() const;

private:
    bool evaluateExpression(const QString &expression, double &result) const;

    QString expression_;
};
