#pragma once

#include <QString>

class CalculatorLogic {
public:
    void appendDigit(const QString &digit);
    void setOperator(const QString &op);
    void calculate();
    void clear();
    QString displayText() const;

private:
    double applyOperation(double left, double right, const QString &op) const;

    QString currentInput_;
    QString pendingOperator_;
    double storedValue_ = 0.0;
    bool hasStoredValue_ = false;
};
