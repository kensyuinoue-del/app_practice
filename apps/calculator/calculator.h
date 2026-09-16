#pragma once

#include <QWidget>

namespace Ui {
class CalculatorUi;
}

class Calculator : public QWidget {
    Q_OBJECT

public:
    explicit Calculator(QWidget *parent = nullptr);
    ~Calculator();

private slots:
    void handleDigitClicked();
    void handleOperatorClicked();
    void handleEqualsClicked();
    void handleClearClicked();

private:
    // 数字ボタンが押されたときの入力処理。
    void appendDigit(const QString &digit);

    // 内部の計算状態を表示欄へ反映する。
    void updateDisplay();

    // 指定された演算子で2つの値を計算する。
    double applyOperation(double left, double right, const QString &op) const;

    // UIファイルから生成された画面部品を保持する。
    Ui::CalculatorUi *ui_;

    // 現在の入力値と計算状態を保持する。
    QString currentInput_;
    QString pendingOperator_;
    double storedValue_ = 0.0;
    bool hasStoredValue_ = false;
};
