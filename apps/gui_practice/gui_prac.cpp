#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

int main(int argc, char *argv[]) {
    // 1. Qtアプリケーションの初期化
    QApplication app(argc, argv);

    // 2. メインウィンドウ（土台となるウインドウ）を作成
    QWidget window;
    window.setWindowTitle("GUI Practice");
    window.resize(600, 400);

    // 3. UI部品（コンポーネント）をコード上で作成
    QLabel *label = new QLabel("ボタンを押してください", &window);
    label->setAlignment(Qt::AlignCenter); // 文字を中央揃えに

    QPushButton *button = new QPushButton("ここをクリック", &window);

    // 4. レイアウトの構築（部品を縦に並べるAPI）
    QVBoxLayout *layout = new QVBoxLayout(&window);
    layout->addWidget(label);
    layout->addWidget(button);
    window.setLayout(layout); // ウィンドウにレイアウトを適用

    // 5. [シグナル・スロット] ボタンが押されたらラベルのテキストを変える
    // 外部ツールを使わず、この1行のAPI呼び出しだけでイベントを接続できます
    QObject::connect(button, &QPushButton::clicked, [=]() {
        label->setText("こんにちは！Qtの世界へようこそ！");
    });

    // 6. ウィンドウを表示してメインループを開始
    window.show();
    return app.exec();
}
