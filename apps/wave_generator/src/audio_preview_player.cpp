#include "audio_preview_player.h"

#include "waveform_calculator.h"

#include <QAudioDevice>
#include <QAudioFormat>
#include <QAudioSink>
#include <QBuffer>
#include <QDataStream>
#include <QMediaDevices>
#include <QSysInfo>

#include <cmath>
#include <limits>

namespace {
bool setError(QString *errorMessage, const QString &message)
{
    if (errorMessage) {
        *errorMessage = message;
    }
    return false;
}

bool appendSample(QDataStream &stream, QByteArray &data, QAudioFormat::SampleFormat sampleFormat,
    double value)
{
    switch (sampleFormat) {
    case QAudioFormat::UInt8: {
        const auto sample = static_cast<quint8>(qBound(0, qRound(value * 127.5 + 127.5), 255));
        data.append(static_cast<char>(sample));
        return true;
    }
    case QAudioFormat::Int16:
        stream << static_cast<qint16>(qRound(value * 32767.0));
        return stream.status() == QDataStream::Ok;
    case QAudioFormat::Int32:
        stream << static_cast<qint32>(qRound64(value * 2147483647.0));
        return stream.status() == QDataStream::Ok;
    case QAudioFormat::Float:
        stream << static_cast<float>(value);
        return stream.status() == QDataStream::Ok;
    case QAudioFormat::Unknown:
    default:
        return false;
    }
}
}

AudioPreviewPlayer::AudioPreviewPlayer(QObject *parent)
    : QObject(parent)
{
}

AudioPreviewPlayer::~AudioPreviewPlayer()
{
    stop();
}

bool AudioPreviewPlayer::play(const wave::Settings &settings, QString *errorMessage)
{
    stop();

    const QAudioDevice device = QMediaDevices::defaultAudioOutput();
    if (device.isNull()) {
        return setError(errorMessage, QStringLiteral("音声出力デバイスが見つかりません。"));
    }

    const QAudioFormat format = device.preferredFormat();
    const int sampleRate = format.sampleRate();
    const int channels = format.channelCount();
    if (!format.isValid() || sampleRate <= 0 || channels <= 0) {
        return setError(errorMessage, QStringLiteral("音声出力デバイスの形式を取得できません。"));
    }
    if (settings.frequency >= sampleRate / 2.0) {
        return setError(errorMessage, QStringLiteral("出力デバイスのサンプルレートでは、この周波数を再生できません。"));
    }

    const double requestedFrames = settings.duration * sampleRate;
    const quint64 maxFrames = std::numeric_limits<qsizetype>::max()
        / (channels * format.bytesPerSample());
    if (!std::isfinite(requestedFrames) || requestedFrames <= 0.0 || requestedFrames > maxFrames) {
        return setError(errorMessage, QStringLiteral("プレビューの長さが再生可能な範囲を超えています。"));
    }

    const qint64 frameCount = qRound64(requestedFrames);
    audioData.clear();
    audioData.reserve(static_cast<qsizetype>(frameCount * channels * format.bytesPerSample()));
    QDataStream stream(&audioData, QIODevice::WriteOnly);
    stream.setByteOrder(QSysInfo::ByteOrder == QSysInfo::LittleEndian
        ? QDataStream::LittleEndian
        : QDataStream::BigEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);

    for (qint64 frame = 0; frame < frameCount; ++frame) {
        const double phase = std::fmod(settings.frequency * frame / sampleRate, 1.0);
        const double value = settings.amplitude * wave::sampleValue(settings.waveform, phase);
        for (int channel = 0; channel < channels; ++channel) {
            if (!appendSample(stream, audioData, format.sampleFormat(), value)) {
                audioData.clear();
                return setError(errorMessage, QStringLiteral("出力デバイスの音声形式に対応していません。"));
            }
        }
    }

    if (stream.status() != QDataStream::Ok) {
        audioData.clear();
        return setError(errorMessage, QStringLiteral("プレビュー用の音声データを作成できませんでした。"));
    }

    audioBuffer = new QBuffer(this);
    audioBuffer->setData(audioData);
    if (!audioBuffer->open(QIODevice::ReadOnly)) {
        delete audioBuffer;
        audioBuffer = nullptr;
        return setError(errorMessage, QStringLiteral("プレビュー用の音声データを開けませんでした。"));
    }

    audioSink = new QAudioSink(device, format, this);
    connect(audioSink, &QAudioSink::stateChanged, this, [this](QAudio::State state) {
        if (state == QAudio::IdleState && playing) {
            playing = false;
            if (playbackStateCallback) {
                playbackStateCallback(false);
            }
        }
    });
    audioSink->start(audioBuffer);
    if (audioSink->error() != QAudio::NoError) {
        stop();
        return setError(errorMessage, QStringLiteral("音声を再生できませんでした。"));
    }

    playing = true;
    if (playbackStateCallback) {
        playbackStateCallback(true);
    }
    return true;
}

void AudioPreviewPlayer::stop()
{
    const bool wasPlaying = playing;
    playing = false;
    if (audioSink) {
        audioSink->stop();
        delete audioSink;
        audioSink = nullptr;
    }
    if (audioBuffer) {
        audioBuffer->close();
        delete audioBuffer;
        audioBuffer = nullptr;
    }
    audioData.clear();
    if (wasPlaying) {
        if (playbackStateCallback) {
            playbackStateCallback(false);
        }
    }
}

bool AudioPreviewPlayer::isPlaying() const
{
    return playing;
}

void AudioPreviewPlayer::setPlaybackStateCallback(std::function<void(bool)> callback)
{
    playbackStateCallback = std::move(callback);
}