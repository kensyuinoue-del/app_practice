#include "signal_analyzer.h"
#include "wav_file_reader.h"

#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>
#include <QMessageBox>
#include <QComboBox>

#include <algorithm>
#include <cmath>
#include <memory>

class GraphArea : public QFrame {
public:
    // グラフの軸設定を更新する。
    void setAxisSettings(int microseconds, double amplitudeRange) {
        m_xIntervalMicroseconds = microseconds;
        m_yRange = amplitudeRange;
        update();
    }

    // サンプルレートを設定する。
    void setSampleRate(int sampleRateHz) {
        m_sampleRateHz = sampleRateHz;
        update();
    }

    // サンプルデータを設定する。
    void setSamples(const QVector<double> &samples) {
        m_samples = samples;
        update();
    }

protected:
    // グラフ表示領域に描画する。
    void paintEvent(QPaintEvent *event) override {
        QFrame::paintEvent(event);

        // 描画領域のサイズを取得、描画領域が有効でない場合は描画をスキップ
        const QRectF contents(contentsRect());
        if (contents.width() <= 0 || contents.height() <= 0) {
            return;
        }

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);  // アンチエイリアスを有効にする
        painter.setClipRect(contents);  // 描画領域をクリップ

        // 描画領域のサイズを取得、描画領域が有効でない場合は描画をスキップ
        const QRectF plotArea(contents);
        if (plotArea.width() <= 0 || plotArea.height() <= 0) {
            return;
        }

        painter.setBrush(QBrush(QColor("#ffffff")));
        painter.setPen(Qt::NoPen);
        painter.drawRect(plotArea);

        // サンプルデータが空の場合は描画をスキップ
        if (m_samples.isEmpty()) {
            return;
        }

        // 表示範囲と目盛り間隔を取得する。
        const double samplePeriodMicroseconds = getSamplePeriodMicroseconds();
        const double maxX = getMaxX(samplePeriodMicroseconds);
        const double minY = -m_yRange;
        const double maxY = m_yRange;
        const double visibleXInterval = getVisibleXInterval(plotArea, maxX);
        const double visibleYInterval = getVisibleYInterval(plotArea, minY, maxY);
        const QPen gridPen(QColor("#d0d6dc"), 1, Qt::DashLine);

        // 各パーツの描画を順番に行う。
        painter.setFont(QFont(QStringLiteral("Sans Serif"), 8));
        drawXGrid(painter, plotArea, maxX, visibleXInterval, gridPen);
        drawYGrid(painter, plotArea, minY, maxY, visibleYInterval, gridPen);
        drawAxes(painter, plotArea);
        drawSamples(painter, plotArea, samplePeriodMicroseconds, maxX, minY, maxY);
    }

private:
    // サンプルレートからサンプル間隔（マイクロ秒）を取得する。
    double getSamplePeriodMicroseconds() const {
        return 1000000.0 / m_sampleRateHz;
    }

    // サンプルデータに合わせたX軸の最大値を取得する。
    double getMaxX(double samplePeriodMicroseconds) const {
        const double lastSampleTimeMicroseconds =
            static_cast<double>(m_samples.size() - 1) * samplePeriodMicroseconds;
        return std::max(
            static_cast<double>(m_xIntervalMicroseconds),
            std::ceil(lastSampleTimeMicroseconds / m_xIntervalMicroseconds) * m_xIntervalMicroseconds
        );
    }

    // 画面幅に収まる、読みやすいX軸目盛り間隔を取得する。
    double getVisibleXInterval(const QRectF &plotArea, double maxX) const {
        const double maxVisibleXTicks = std::max(1.0, std::floor(plotArea.width() / 80.0));
        return std::max(
            static_cast<double>(m_xIntervalMicroseconds),
            std::ceil(maxX / (maxVisibleXTicks * m_xIntervalMicroseconds)) * m_xIntervalMicroseconds
        );
    }

    // 画面高さに収まりやすい、読みやすいY軸目盛り間隔を取得する。
    double getVisibleYInterval(
        const QRectF &plotArea, double minY, double maxY) const {
        const double maxVisibleYTicks = std::max(1.0, std::floor(plotArea.height() / 28.0));
        const double rawYInterval = (maxY - minY) / maxVisibleYTicks;
        const double intervalMagnitude = std::pow(10.0, std::floor(std::log10(rawYInterval)));
        const double normalizedInterval = rawYInterval / intervalMagnitude;
        const double niceInterval = normalizedInterval <= 1.0 ? 1.0
            : normalizedInterval <= 2.0 ? 2.0
            : normalizedInterval <= 5.0 ? 5.0 : 10.0;
        return niceInterval * intervalMagnitude;
    }

    // X軸の目盛り位置に垂直なグリッド線を描画する。
    void drawXGrid(
        QPainter &painter, const QRectF &plotArea, double maxX,
        double visibleXInterval, const QPen &gridPen) const {
        painter.setPen(gridPen);
        for (double timeMicroseconds = 0.0;
             timeMicroseconds <= maxX + (visibleXInterval * 1e-9);
             timeMicroseconds += visibleXInterval) {
            const qreal x = plotArea.left() + (timeMicroseconds / maxX) * plotArea.width();
            painter.drawLine(QPointF(x, plotArea.top()), QPointF(x, plotArea.bottom()));
        }
    }

    // Y軸の目盛り位置に水平なグリッド線を描画する。
    void drawYGrid(
        QPainter &painter, const QRectF &plotArea, double minY, double maxY,
        double visibleYInterval, const QPen &gridPen) const {
        const double firstYTick = std::ceil(minY / visibleYInterval);
        const double lastYTick = std::floor(maxY / visibleYInterval);
        painter.setPen(gridPen);
        for (double tick = firstYTick; tick <= lastYTick; ++tick) {
            const double amplitude = tick * visibleYInterval;
            const qreal y = plotArea.bottom() - ((amplitude - minY) / (maxY - minY)) * plotArea.height();
            painter.drawLine(QPointF(plotArea.left(), y), QPointF(plotArea.right(), y));
        }
    }

    // X軸とY軸を描画する。
    void drawAxes(QPainter &painter, const QRectF &plotArea) const {
        const QPen axisPen(QColor("#4d5560"), 1.5);
        painter.setPen(axisPen);
        painter.drawLine(QPointF(plotArea.left(), plotArea.center().y()),
                         QPointF(plotArea.right(), plotArea.center().y())
        );
        painter.drawLine(QPointF(plotArea.left(), plotArea.top()),
                         QPointF(plotArea.left(), plotArea.bottom())
        );
    }

    // サンプルデータを座標に変換し、折れ線として描画する。
    void drawSamples(
        QPainter &painter, const QRectF &plotArea, double samplePeriodMicroseconds,
        double maxX, double minY, double maxY) const {
        QPainterPath path;
        for (int index = 0; index < m_samples.size(); ++index) {
            const double sampleTimeMicroseconds = index * samplePeriodMicroseconds;
            const double normalizedX = sampleTimeMicroseconds / maxX;
            const double normalizedY = (m_samples[index] - minY) / (maxY - minY);
            const qreal x = plotArea.left() + normalizedX * plotArea.width();
            const qreal y = plotArea.bottom() - normalizedY * plotArea.height();
            const QPointF point(x, y);
            if (index == 0) {
                path.moveTo(point);
            } else {
                path.lineTo(point);
            }
        }

        // サンプルデータの線を描画
        painter.setPen(QPen(QColor("#3478c5"), 2, Qt::SolidLine,
                            Qt::RoundCap, Qt::RoundJoin));
        painter.drawPath(path);
    }

    // 設定値を描画処理から分離して保持する。
    QVector<double> m_samples;
    int m_sampleRateHz = 0;
    int m_xIntervalMicroseconds = 1;
    double m_yRange = 1.0;
};

SignalAnalyzer::SignalAnalyzer(QWidget *parent)
    : QMainWindow(parent) {
    QWidget *centralWidget = new QWidget(this);
    auto *rootLayout = new QHBoxLayout(centralWidget);
    const int margin = 8;
    const int widgetSpacing = 8;
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
    xIntervalInput->setRange(1, 1000000000);
    xIntervalInput->setValue(1);
    controlsLayout->addWidget(xIntervalLabel);
    controlsLayout->addWidget(xIntervalInput);

    auto *yRangeLabel = new QLabel(QStringLiteral("Y軸表示範囲（±サンプル値）:"), centralWidget);
    yRangeLabel->setWordWrap(true);
    auto *yRangeInput = new QDoubleSpinBox(centralWidget);
    yRangeInput->setDecimals(8);
    yRangeInput->setRange(0.001, 1000000000000.0);
    yRangeInput->setSingleStep(0.1);
    yRangeInput->setValue(1.0);
    controlsLayout->addWidget(yRangeLabel);
    controlsLayout->addWidget(yRangeInput);

    controlsLayout->addStretch();

    // グラフ表示領域を作成
    auto *graphArea = new GraphArea;
    graphArea->setStyleSheet(
        "QFrame { background-color: #ffffff; border: 2px solid #888181; }"
    );
    const int minHeight = 320;
    graphArea->setMinimumHeight(minHeight);
    rootLayout->addWidget(graphArea, 1);

    // コントロールパネルを作成
    auto *controlsPanel = new QWidget(centralWidget);
    controlsPanel->setLayout(controlsLayout);
    controlsPanel->setMinimumWidth(190);
    rootLayout->addWidget(controlsPanel);

    // グラフの軸設定を更新するラムダ関数を定義
    const auto updateAxisSettings = [graphArea, xIntervalInput, yRangeInput]() {
        graphArea->setAxisSettings(xIntervalInput->value(), yRangeInput->value());
    };
    QObject::connect(xIntervalInput, &QSpinBox::valueChanged, graphArea,
                     [updateAxisSettings](int) { updateAxisSettings(); }
    );
    QObject::connect(yRangeInput, &QDoubleSpinBox::valueChanged, graphArea,
                     [updateAxisSettings](double) { updateAxisSettings(); }
    );
    
    // WAVファイルを読み込むボタンがクリックされたときの処理を定義
    const auto loadedChannels = std::make_shared<QVector<QVector<double>>>();
    QObject::connect(channelInput, qOverload<int>(&QComboBox::currentIndexChanged),
                     graphArea, [graphArea, channelInput, loadedChannels](int index) {
        if (index >= 0 && index < loadedChannels->size()) {
            graphArea->setSamples((*loadedChannels)[index]);
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
        *loadedChannels = wavData.channels;
        for (int channel = 0; channel < wavData.channels.size(); ++channel) {
            channelInput->addItem(QStringLiteral("チャンネル %1").arg(channel + 1), channel);
        }
        const bool hasMultipleChannels = wavData.channels.size() > 1;
        channelLabel->setVisible(hasMultipleChannels);
        channelInput->setVisible(hasMultipleChannels);
        graphArea->setSampleRate(wavData.sampleRateHz);
        graphArea->setSamples(wavData.channels.first());
        fileInfoLabel->setText(
            QStringLiteral("%1\n%2 Hz / %3 ch / %4-bit %5 / %6 samples")
                .arg(QFileInfo(filePath).fileName())
                .arg(wavData.sampleRateHz)
                .arg(wavData.channelCount)
                .arg(wavData.bitsPerSample)
                .arg(wavData.encoding)
                .arg(wavData.channels.first().size()));
    });
    setCentralWidget(centralWidget);
}
