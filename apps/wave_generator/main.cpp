#include "wave_generator.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    WaveGenerator window;
    window.setWindowTitle(QStringLiteral("Wave_Generator"));
    window.resize(560, 600);
    window.show();

    return app.exec();
}