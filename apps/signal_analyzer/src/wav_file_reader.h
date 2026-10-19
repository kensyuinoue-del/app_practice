#pragma once

#include <QVector>
#include <QString>

struct WavFileData {
    int sampleRateHz = 0;
    quint16 channelCount = 0;
    quint16 bitsPerSample = 0;
    QString encoding;
    QVector<QVector<double>> channels;
};

bool readWavFile(const QString &filePath, WavFileData *wavData, QString *errorMessage);
