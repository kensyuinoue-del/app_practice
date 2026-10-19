#include "signal_analyzer.h"
#include "signal_graph_widget.h"
#include "wav_file_reader.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>

#include <memory>
#include <utility>

SignalAnalyzer::SignalAnalyzer(QWidget *parent)
    : QMainWindow(parent) {
    QWidget *centralWidget = new QWidget(this);
    auto *rootLayout = new QHBoxLayout(centralWidget);
    constexpr int margin = 8;
    constexpr int widgetSpacing = 8;
    rootLayout->setContentsMargins(margin, margin, margin, margin); // ウィンドウの端からの余白をmargin分のピクセルに設定
    rootLayout->setSpacing(widgetSpacing);  // ウィジェット間のスペースをwidgetSpacing分のピクセルに設定

    // コントロールパネルのレイアウトを作成
    auto *controlsLayout = new QVBoxLayout;

    auto *openWavButton = new QPushButton(QStringLiteral("WAVを開く"), centralWidget);
    auto *fileInfoLabel = new QLabel(QStringLiteral("ファイル未読み込み"), centralWidget);
    fileInfoLabel->setWordWrap(true);
    auto *channelLabel = new QLabel(QStringLiteral("表示チャンネル:"), centralWidget);
    auto *channelInput = new QComboBox(centralWidget);
    channelLabel->hide();
    channelInput->hide();
    controlsLayout->addWidget(openWavButton);
    controlsLayout->addWidget(fileInfoLabel);
    controlsLayout->addWidget(channelLabel);
    controlsLayout->addWidget(channelInput);
    
    auto *xIntervalLabel = new QLabel(QStringLiteral("X軸目盛り間隔（μs）:"), centralWidget);
    xIntervalLabel->setWordWrap(true);
    auto *xIntervalInput = new QSpinBox(centralWidget);
    xIntervalInput->setRange(1, 1000000);
    xIntervalInput->setValue(100);
    controlsLayout->addWidget(xIntervalLabel);
    controlsLayout->addWidget(xIntervalInput);

    auto *yIntervalLabel = new QLabel(QStringLiteral("Y軸目盛り間隔（サンプル値）:"), centralWidget);
    yIntervalLabel->setWordWrap(true);
    auto *yIntervalInput = new QDoubleSpinBox(centralWidget);
    yIntervalInput->setDecimals(3);
    yIntervalInput->setRange(0.01, 1.0);
    yIntervalInput->setSingleStep(0.01);
    yIntervalInput->setValue(0.1);
    controlsLayout->addWidget(yIntervalLabel);
    controlsLayout->addWidget(yIntervalInput);

    controlsLayout->addStretch();

    // グラフ表示領域を作成
    auto *graphArea = new SignalGraphWidget;
    graphArea->setStyleSheet(
        "QFrame { background-color: #ffffff; border: 2px solid #888181; }"
    );
    constexpr int minHeight = 320;
    graphArea->setMinimumHeight(minHeight);
    rootLayout->addWidget(graphArea, 1);

    // コントロールパネルを作成
    auto *controlsPanel = new QWidget(centralWidget);
    constexpr int minWidth = 190;
    controlsPanel->setLayout(controlsLayout);
    controlsPanel->setMinimumWidth(minWidth);
    rootLayout->addWidget(controlsPanel);

    // グラフの軸設定を更新するラムダ関数を定義
    const auto updateAxisSettings = [graphArea, xIntervalInput, yIntervalInput]() {
        graphArea->setAxisSettings(xIntervalInput->value(), yIntervalInput->value());
    };
    QObject::connect(xIntervalInput, &QSpinBox::valueChanged, graphArea,
                     [updateAxisSettings](int) { updateAxisSettings(); }
    );
    QObject::connect(yIntervalInput, &QDoubleSpinBox::valueChanged, graphArea,
                     [updateAxisSettings](double) { updateAxisSettings(); }
    );
    updateAxisSettings();
    
    // WAVファイルを読み込むボタンがクリックされたときの処理を定義
    const auto loadedChannels = std::make_shared<QVector<QVector<double>>>();
    QObject::connect(channelInput, qOverload<int>(&QComboBox::currentIndexChanged),
                     graphArea, [graphArea, loadedChannels](int index) {
        if (index >= 0 && index < loadedChannels->size()) {
            graphArea->setSamples(std::shared_ptr<const QVector<double>>(
                loadedChannels, &(*loadedChannels)[index]));
        }
    });
    QObject::connect(openWavButton, &QPushButton::clicked, this,
                     [this, graphArea, fileInfoLabel, channelLabel, channelInput, loadedChannels]() {
        const QString filePath = QFileDialog::getOpenFileName(
            this, QStringLiteral("WAVファイルを開く"), QString(),
            QStringLiteral("WAVファイル (*.wav *.wave);;すべてのファイル (*)"));
        if (filePath.isEmpty()) {
            return;
        }

        WavFileData wavData;
        QString errorMessage;
        if (!readWavFile(filePath, &wavData, &errorMessage)) {
            QMessageBox::critical(this, QStringLiteral("読み込みエラー"), errorMessage);
            return;
        }

        // WAVファイルの情報を表示し、チャンネル選択コンボボックスを更新する
        channelInput->clear();
        *loadedChannels = std::move(wavData.channels);
        for (int channel = 0; channel < wavData.channels.size(); ++channel) {
            channelInput->addItem(QStringLiteral("チャンネル %1").arg(channel + 1), channel);
        }
        const bool hasMultipleChannels = wavData.channels.size() > 1;
        channelLabel->setVisible(hasMultipleChannels);
        channelInput->setVisible(hasMultipleChannels);
        graphArea->setSampleRate(wavData.sampleRateHz);
        graphArea->setSamples(std::shared_ptr<const QVector<double>>(
            loadedChannels, &loadedChannels->first()));
        fileInfoLabel->setText(
            QStringLiteral("%1\n%2 Hz / %3 ch / %4-bit %5 / %6 samples")
                .arg(QFileInfo(filePath).fileName())
                .arg(wavData.sampleRateHz)
                .arg(wavData.channelCount)
                .arg(wavData.bitsPerSample)
                .arg(wavData.encoding)
                .arg(loadedChannels->first().size()));
    });
    setCentralWidget(centralWidget);
}
