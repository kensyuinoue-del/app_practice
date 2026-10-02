cmake_minimum_required(VERSION 3.16)
project(Wave_Generator VERSION 0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Qtの自動処理（MOC）を有効化（Qtのコア機能に必要）
set(CMAKE_AUTOMOC ON)

# 2. Qt6の必要なパッケージ（Core, Gui, Widgets）を検索
# ※環境によっては "Qt5" に書き換えてください
find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)

# 3. 実行ファイルの定義
option(WAVE_GENERATOR_GUI_SUBSYSTEM "Build without a console window on Windows" OFF)
if(WIN32 AND WAVE_GENERATOR_GUI_SUBSYSTEM)
	add_executable(Wave_Generator WIN32 main.cpp)
else()
	add_executable(Wave_Generator main.cpp)
endif()
target_sources(Wave_Generator PRIVATE
	src/wave_generator.cpp
	src/audio_preview_player.cpp
	src/waveform_calculator.cpp
	src/waveform_preview.cpp
	src/wav_file_writer.cpp
)
target_include_directories(Wave_Generator PRIVATE src)
target_compile_definitions(Wave_Generator PRIVATE
	WAVE_GENERATOR_WAVES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/waves"
)

# 4. Qtのライブラリをプログラムにリンク
target_link_libraries(Wave_Generator PRIVATE Qt6::Widgets Qt6::Multimedia)
