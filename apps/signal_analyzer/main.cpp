#include "signal_analyzer.h"

#include <QApplication>

int main(int argc, char *argv[]){
    QApplication app(argc, argv);

    QWidget signalAnalyzer;
    signalAnalyzer.setWindowTitle("SignalAnalyzer");
    signalAnalyzer.resize(800, 450);
    signalAnalyzer.show();

    return app.exec();
}