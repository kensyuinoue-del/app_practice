#include "wave_generator.h"
#include "audio_preview_player.h"
#include "waveform_calculator.h"
#include "waveform_preview.h"
#include "wav_file_writer.h"

#include <QComboBox>
#include <QDir>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>


WaveGenerator::WaveGenerator(QWidget *parent)
    : QMainWindow(parent)
{
    auto *centralWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(centralWidget);
    auto *form = new QFormLayout;

    waveformBox = new QComboBox(centralWidget);
    waveformBox->addItem(QStringLiteral("正弦波"), static_cast<int>(wave::Waveform::Sine));
    waveformBox->addItem(QStringLiteral("矩形波"), static_cast<int>(wave::Waveform::Square));
    waveformBox->addItem(QStringLiteral("三角波"), static_cast<int>(wave::Waveform::Triangle));
    waveformBox->addItem(QStringLiteral("のこぎり波"), static_cast<int>(wave::Waveform::Sawtooth));

    frequencyBox = new QDoubleSpinBox(centralWidget);
    frequencyBox->setRange(1.0, 20000.0);
    frequencyBox->setDecimals(1);
    frequencyBox->setValue(440.0);
    frequencyBox->setSuffix(QStringLiteral(" Hz"));

    durationBox = new QDoubleSpinBox(centralWidget);
    durationBox->setRange(0.1, 60.0);
    durationBox->setDecimals(1);
    durationBox->setSingleStep(0.1);
    durationBox->setValue(2.0);
    durationBox->setSuffix(QStringLiteral(" 秒"));

    amplitudeBox = new QDoubleSpinBox(centralWidget);
    amplitudeBox->setRange(0.00, 1.00);
    amplitudeBox->setDecimals(2);
    amplitudeBox->setSingleStep(0.01);
    amplitudeBox->setValue(0.03);

    sampleRateBox = new QComboBox(centralWidget);
    for (const int rate : {8000, 16000, 22050, 44100, 48000, 96000}) {
        sampleRateBox->addItem(QStringLiteral("%1 Hz").arg(rate), rate);
    }
    sampleRateBox->setCurrentIndex(3);

    channelBox = new QComboBox(centralWidget);
    channelBox->addItem(QStringLiteral("モノラル"), 1);
    channelBox->addItem(QStringLiteral("ステレオ"), 2);

    fileNameEdit = new QLineEdit(QStringLiteral("tone"), centralWidget);

    form->addRow(QStringLiteral("波形"), waveformBox);
    form->addRow(QStringLiteral("周波数"), frequencyBox);
    form->addRow(QStringLiteral("長さ"), durationBox);
    form->addRow(QStringLiteral("振幅"), amplitudeBox);
    form->addRow(QStringLiteral("サンプルレート"), sampleRateBox);
    form->addRow(QStringLiteral("チャンネル"), channelBox);
    form->addRow(QStringLiteral("ファイル名"), fileNameEdit);
    layout->addLayout(form);

    layout->addWidget(new QLabel(QStringLiteral("波形プレビュー（表示範囲: 最大20 ms）"), centralWidget));
    waveformPreview = new WaveformPreview(centralWidget);
    layout->addWidget(waveformPreview, 1);

    auto *playPreviewButton = new QPushButton(QStringLiteral("プレビュー再生"), centralWidget);
    layout->addWidget(playPreviewButton);

    auto *saveButton = new QPushButton(QStringLiteral("WAVを生成して保存"), centralWidget);
    layout->addWidget(saveButton);
    layout->addWidget(new QLabel(QStringLiteral("保存先: waves フォルダ"), centralWidget));
    setCentralWidget(centralWidget);

    audioPreviewPlayer = new AudioPreviewPlayer(this);
    audioPreviewPlayer->setPlaybackStateCallback([playPreviewButton](bool playing) {
        playPreviewButton->setText(playing
            ? QStringLiteral("プレビュー停止")
            : QStringLiteral("プレビュー再生"));
    });

    const auto updatePreview = [this] {
        audioPreviewPlayer->stop();
        waveformPreview->setSignal(
            static_cast<wave::Waveform>(waveformBox->currentData().toInt()),
            frequencyBox->value(), durationBox->value(), amplitudeBox->value());
    };
    connect(waveformBox, qOverload<int>(&QComboBox::currentIndexChanged), this, updatePreview);
    connect(frequencyBox, &QDoubleSpinBox::valueChanged, this, updatePreview);
    connect(durationBox, &QDoubleSpinBox::valueChanged, this, updatePreview);
    connect(amplitudeBox, &QDoubleSpinBox::valueChanged, this, updatePreview);
    updatePreview();

    connect(playPreviewButton, &QPushButton::clicked, this, [this] {
        if (audioPreviewPlayer->isPlaying()) {
            audioPreviewPlayer->stop();
            return;
        }

        QString errorMessage;
        if (!audioPreviewPlayer->play(currentSettings(), &errorMessage)) {
            QMessageBox::warning(this, QStringLiteral("再生エラー"), errorMessage);
        }
    });

    connect(saveButton, &QPushButton::clicked, this, [this] {
        saveWaveFile();
    });
}

wave::Settings WaveGenerator::currentSettings() const
{
    return {
        static_cast<wave::Waveform>(waveformBox->currentData().toInt()),
        frequencyBox->value(),
        durationBox->value(),
        amplitudeBox->value(),
        sampleRateBox->currentData().toInt(),
        channelBox->currentData().toInt()
    };
}

void WaveGenerator::saveWaveFile()
{
    QString fileName = fileNameEdit->text().trimmed();
    if (fileName.isEmpty() || fileName == QStringLiteral(".") || fileName == QStringLiteral("..")) {
        QMessageBox::warning(this, QStringLiteral("入力エラー"), QStringLiteral("ファイル名を入力してください。"));
        return;
    }

    for (const QChar character : fileName) {
        if (character.unicode() < 32 || QStringLiteral("<>:\"/\\|?*").contains(character)) {
            QMessageBox::warning(this, QStringLiteral("入力エラー"), QStringLiteral("ファイル名に使用できない文字が含まれています。"));
            return;
        }
    }
    if (!fileName.endsWith(QStringLiteral(".wav"), Qt::CaseInsensitive)) {
        fileName += QStringLiteral(".wav");
    }

    const QDir outputDirectory(QStringLiteral(WAVE_GENERATOR_WAVES_DIR));
    if (!QDir().mkpath(outputDirectory.absolutePath())) {
        QMessageBox::critical(this, QStringLiteral("保存エラー"), QStringLiteral("wavesフォルダを作成できませんでした。"));
        return;
    }

    const wave::Settings settings = currentSettings();
    QString errorMessage;
    const QString outputPath = outputDirectory.filePath(fileName);
    if (!wave::writeWavFile(outputPath, settings, &errorMessage)) {
        QMessageBox::critical(this, QStringLiteral("保存エラー"), errorMessage);
        return;
    }

    QMessageBox::information(this, QStringLiteral("完了"),
        QStringLiteral("WAVファイルを保存しました:\n%1").arg(outputPath));
}