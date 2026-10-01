#pragma once

#include "waveform_calculator.h"

#include <QString>

namespace wave {

bool writeWavFile(const QString &filePath, const Settings &settings, QString *errorMessage);

}