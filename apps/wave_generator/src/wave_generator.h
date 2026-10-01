#pragma once

#include <QMainWindow>
#include "waveform_calculator.h"

class QComboBox;
class QDoubleSpinBox;
class QLineEdit;
class AudioPreviewPlayer;
class WaveformPreview;

class WaveGenerator : public QMainWindow {
public:
	explicit WaveGenerator(QWidget *parent = nullptr);

private:
	wave::Settings currentSettings() const;
	void saveWaveFile();

	QComboBox *waveformBox;
	QDoubleSpinBox *frequencyBox;
	QDoubleSpinBox *durationBox;
	QDoubleSpinBox *amplitudeBox;
	QComboBox *sampleRateBox;
	QComboBox *channelBox;
	QLineEdit *fileNameEdit;
	WaveformPreview *waveformPreview;
	AudioPreviewPlayer *audioPreviewPlayer;
};