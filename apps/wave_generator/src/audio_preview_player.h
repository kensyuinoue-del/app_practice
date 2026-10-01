#pragma once

#include "waveform_calculator.h"

#include <QObject>
#include <functional>

class QByteArray;
class QAudioSink;
class QBuffer;

class AudioPreviewPlayer : public QObject {
public:
    explicit AudioPreviewPlayer(QObject *parent = nullptr);
    ~AudioPreviewPlayer() override;

    bool play(const wave::Settings &settings, QString *errorMessage);
    void stop();
    bool isPlaying() const;
    void setPlaybackStateCallback(std::function<void(bool)> callback);

private:
    QByteArray audioData;
    QAudioSink *audioSink = nullptr;
    QBuffer *audioBuffer = nullptr;
    bool playing = false;
    std::function<void(bool)> playbackStateCallback;
};