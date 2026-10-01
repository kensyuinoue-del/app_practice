#include "wav_file_writer.h"

#include <QByteArray>
#include <QDataStream>
#include <QSaveFile>

#include <cmath>
#include <limits>

namespace wave {
namespace {
constexpr qint64 framesPerBlock = 4096;

// エラー処理用のヘルパー関数
bool fail(QString *errorMessage, const QString &message)
{
    if (errorMessage) {
        *errorMessage = message;
    }
    return false;
}
}

bool writeWavFile(const QString &filePath, const Settings &settings, QString *errorMessage)
{
    if (settings.sampleRate <= 0 || (settings.channels != 1 && settings.channels != 2)
        || !std::isfinite(settings.frequency) || settings.frequency <= 0.0
        || settings.frequency >= settings.sampleRate / 2.0
        || !std::isfinite(settings.duration) || settings.duration <= 0.0
        || !std::isfinite(settings.amplitude) || settings.amplitude < 0.0 || settings.amplitude > 1.0) {
        return fail(errorMessage, QStringLiteral("音声の設定値が不正です。"));
    }

    const double requestedFrames = settings.duration * settings.sampleRate;
    const quint64 maxDataSize = std::numeric_limits<quint32>::max() - 36;
    const quint64 maxFrames = maxDataSize / (settings.channels * sizeof(qint16));
    if (!std::isfinite(requestedFrames) || requestedFrames > maxFrames) {
        return fail(errorMessage, QStringLiteral("生成データがWAV形式の上限を超えています。"));
    }

    const qint64 frameCount = qRound64(requestedFrames);
    if (frameCount <= 0) {
        return fail(errorMessage, QStringLiteral("生成する音声の長さが短すぎます。"));
    }

    const quint32 dataSize = static_cast<quint32>(frameCount * settings.channels * sizeof(qint16));
    QSaveFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return fail(errorMessage, file.errorString());
    }

    QDataStream header(&file);
    header.setByteOrder(QDataStream::LittleEndian);
    const bool tagsWritten = header.writeRawData("RIFF", 4) == 4;
    header << static_cast<quint32>(36 + dataSize);
    const bool chunksWritten = header.writeRawData("WAVE", 4) == 4
        && header.writeRawData("fmt ", 4) == 4;
    header << quint32(16) << quint16(1) << quint16(settings.channels)
           << quint32(settings.sampleRate)
           << quint32(settings.sampleRate * settings.channels * sizeof(qint16))
           << quint16(settings.channels * sizeof(qint16)) << quint16(16);
    const bool dataTagWritten = header.writeRawData("data", 4) == 4;
    header << dataSize;
    if (!tagsWritten || !chunksWritten || !dataTagWritten || header.status() != QDataStream::Ok) {
        return fail(errorMessage, QStringLiteral("WAVヘッダーを書き込めませんでした。"));
    }

    QByteArray block;
    block.reserve(static_cast<qsizetype>(framesPerBlock * settings.channels * sizeof(qint16)));
    for (qint64 firstFrame = 0; firstFrame < frameCount; firstFrame += framesPerBlock) {
        block.clear();
        const qint64 blockFrames = qMin(framesPerBlock, frameCount - firstFrame);
        for (qint64 frame = firstFrame; frame < firstFrame + blockFrames; ++frame) {
            const double phase = std::fmod(settings.frequency * frame / settings.sampleRate, 1.0);
            const double value = settings.amplitude * sampleValue(settings.waveform, phase);
            const qint16 sample = static_cast<qint16>(qRound(value * 32767.0));
            const quint16 bits = static_cast<quint16>(sample);
            for (int channel = 0; channel < settings.channels; ++channel) {
                block.append(static_cast<char>(bits & 0xff));
                block.append(static_cast<char>((bits >> 8) & 0xff));
            }
        }
        if (file.write(block) != block.size()) {
            return fail(errorMessage, file.errorString());
        }
    }

    if (!file.commit()) {
        return fail(errorMessage, file.errorString());
    }
    return true;
}

}