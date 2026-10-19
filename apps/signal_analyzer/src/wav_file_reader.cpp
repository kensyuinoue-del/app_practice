#include "wav_file_reader.h"

#include <QFile>

#include <cmath>
#include <cstring>
#include <limits>

namespace {
quint16 readUInt16(const uchar *data)
{
    return static_cast<quint16>(data[0])
        | (static_cast<quint16>(data[1]) << 8);
}

quint32 readUInt32(const uchar *data)
{
    return static_cast<quint32>(data[0])
        | (static_cast<quint32>(data[1]) << 8)
        | (static_cast<quint32>(data[2]) << 16)
        | (static_cast<quint32>(data[3]) << 24);
}

bool fail(QString *errorMessage, const QString &message)
{
    if (errorMessage) {
        *errorMessage = message;
    }
    return false;
}

double readPcmSample(const uchar *data, quint16 bitsPerSample)
{
    if (bitsPerSample == 8) {
        return (static_cast<int>(data[0]) - 128) / 128.0;
    }

    qint64 value = 0;
    const int byteCount = bitsPerSample / 8;
    for (int byteIndex = 0; byteIndex < byteCount; ++byteIndex) {
        value |= static_cast<qint64>(data[byteIndex]) << (byteIndex * 8);
    }

    const int signBit = bitsPerSample - 1;
    if ((value & (qint64(1) << signBit)) != 0) {
        value -= qint64(1) << bitsPerSample;
    }
    return static_cast<double>(value) / std::ldexp(1.0, signBit);
}

double readFloatSample(const uchar *data, quint16 bitsPerSample)
{
    if (bitsPerSample == 32) {
        const quint32 bits = readUInt32(data);
        float value = 0.0F;
        std::memcpy(&value, &bits, sizeof(value));
        return value;
    }

    quint64 bits = 0;
    for (int byteIndex = 0; byteIndex < 8; ++byteIndex) {
        bits |= static_cast<quint64>(data[byteIndex]) << (byteIndex * 8);
    }
    double value = 0.0;
    std::memcpy(&value, &bits, sizeof(value));
    return value;
}
}

bool readWavFile(const QString &filePath, WavFileData *wavData, QString *errorMessage)
{
    if (!wavData) {
        return fail(errorMessage, QStringLiteral("読み込み先が指定されていません。"));
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return fail(errorMessage, file.errorString());
    }

    const qint64 fileSize = file.size();
    if (fileSize < 12) {
        return fail(errorMessage, QStringLiteral("WAVファイルのヘッダーが不完全です。"));
    }

    const QByteArray header = file.read(12);
    if (header.size() != 12 || std::memcmp(header.constData(), "RIFF", 4) != 0
        || std::memcmp(header.constData() + 8, "WAVE", 4) != 0) {
        return fail(errorMessage, QStringLiteral("RIFF/WAVE形式のファイルではありません。"));
    }

    const quint64 riffEnd = static_cast<quint64>(readUInt32(
        reinterpret_cast<const uchar *>(header.constData()) + 4)) + 8;
    if (riffEnd > static_cast<quint64>(fileSize) || riffEnd < 12) {
        return fail(errorMessage, QStringLiteral("WAVファイルのサイズ情報が不正です。"));
    }

    bool hasFormat = false;
    bool hasData = false;
    quint16 formatCode = 0;
    quint16 channelCount = 0;
    quint16 bitsPerSample = 0;
    quint16 blockAlign = 0;
    quint32 sampleRateHz = 0;
    quint64 dataOffset = 0;
    quint32 dataSize = 0;

    quint64 chunkOffset = 12;
    while (chunkOffset + 8 <= riffEnd) {
        if (!file.seek(static_cast<qint64>(chunkOffset))) {
            return fail(errorMessage, file.errorString());
        }
        const QByteArray chunkHeader = file.read(8);
        if (chunkHeader.size() != 8) {
            return fail(errorMessage, QStringLiteral("WAVチャンクのヘッダーを読み込めません。"));
        }
        const auto *chunkBytes = reinterpret_cast<const uchar *>(chunkHeader.constData());
        const quint32 chunkSize = readUInt32(chunkBytes + 4);
        const quint64 contentOffset = chunkOffset + 8;
        const quint64 paddedSize = static_cast<quint64>(chunkSize) + (chunkSize & 1U);
        if (contentOffset + paddedSize > riffEnd) {
            return fail(errorMessage, QStringLiteral("WAVチャンクのサイズ情報が不正です。"));
        }

        if (std::memcmp(chunkHeader.constData(), "fmt ", 4) == 0) {
            if (chunkSize < 16) {
                return fail(errorMessage, QStringLiteral("WAV形式情報が不完全です。"));
            }
            const QByteArray format = file.read(16);
            if (format.size() != 16) {
                return fail(errorMessage, QStringLiteral("WAV形式情報を読み込めません。"));
            }
            const auto *formatBytes = reinterpret_cast<const uchar *>(format.constData());
            formatCode = readUInt16(formatBytes);
            channelCount = readUInt16(formatBytes + 2);
            sampleRateHz = readUInt32(formatBytes + 4);
            blockAlign = readUInt16(formatBytes + 12);
            bitsPerSample = readUInt16(formatBytes + 14);
            hasFormat = true;
        } else if (std::memcmp(chunkHeader.constData(), "data", 4) == 0 && !hasData) {
            dataOffset = contentOffset;
            dataSize = chunkSize;
            hasData = true;
        }

        chunkOffset = contentOffset + paddedSize;
    }

    if (chunkOffset != riffEnd) {
        return fail(errorMessage, QStringLiteral("WAVチャンクの並びが不正です。"));
    }
    if (!hasFormat || !hasData) {
        return fail(errorMessage, QStringLiteral("WAVファイルに形式情報または音声データがありません。"));
    }
    if (formatCode != 1 && formatCode != 3) {
        return fail(errorMessage, QStringLiteral("このWAVの符号化方式には対応していません（PCM/IEEE Floatのみ対応）。"));
    }
    if (channelCount == 0 || sampleRateHz == 0
        || sampleRateHz > static_cast<quint32>(std::numeric_limits<int>::max())) {
        return fail(errorMessage, QStringLiteral("チャンネル数またはサンプルレートが不正です。"));
    }
    const bool validPcmBits = bitsPerSample == 8 || bitsPerSample == 16
        || bitsPerSample == 24 || bitsPerSample == 32;
    const bool validFloatBits = bitsPerSample == 32 || bitsPerSample == 64;
    if ((formatCode == 1 && !validPcmBits) || (formatCode == 3 && !validFloatBits)) {
        return fail(errorMessage, QStringLiteral("このビット深度には対応していません。"));
    }

    const quint16 bytesPerSample = bitsPerSample / 8;
    const quint32 expectedBlockAlign = static_cast<quint32>(channelCount) * bytesPerSample;
    if (blockAlign != expectedBlockAlign || dataSize == 0 || dataSize % blockAlign != 0) {
        return fail(errorMessage, QStringLiteral("WAV音声データのサイズまたはチャンネル情報が不正です。"));
    }
    const quint64 frameCount = dataSize / blockAlign;
    if (frameCount > static_cast<quint64>(std::numeric_limits<int>::max())) {
        return fail(errorMessage, QStringLiteral("WAV音声データが大きすぎます。"));
    }
    if (!file.seek(static_cast<qint64>(dataOffset))) {
        return fail(errorMessage, file.errorString());
    }
    const QByteArray sampleData = file.read(dataSize);
    if (sampleData.size() != static_cast<qsizetype>(dataSize)) {
        const QString message = file.error() == QFileDevice::NoError
            ? QStringLiteral("WAV音声データを最後まで読み込めません。")
            : file.errorString();
        return fail(errorMessage, message);
    }

    WavFileData loaded;
    loaded.sampleRateHz = static_cast<int>(sampleRateHz);
    loaded.channelCount = channelCount;
    loaded.bitsPerSample = bitsPerSample;
    loaded.encoding = formatCode == 1 ? QStringLiteral("PCM") : QStringLiteral("IEEE Float");
    loaded.channels.resize(channelCount);
    for (QVector<double> &channel : loaded.channels) {
        channel.reserve(static_cast<int>(frameCount));
    }

    const auto *samples = reinterpret_cast<const uchar *>(sampleData.constData());
    for (quint64 frame = 0; frame < frameCount; ++frame) {
        for (quint16 channel = 0; channel < channelCount; ++channel) {
            const quint64 sampleOffset = frame * blockAlign + channel * bytesPerSample;
            const uchar *sample = samples + sampleOffset;
            const double value = formatCode == 1
                ? readPcmSample(sample, bitsPerSample)
                : readFloatSample(sample, bitsPerSample);
            if (!std::isfinite(value)) {
                return fail(errorMessage, QStringLiteral("WAVに有効でない数値（NaNまたは無限大）が含まれています。"));
            }
            loaded.channels[channel].append(value);
        }
    }

    *wavData = std::move(loaded);
    if (errorMessage) {
        errorMessage->clear();
    }
    return true;
}
