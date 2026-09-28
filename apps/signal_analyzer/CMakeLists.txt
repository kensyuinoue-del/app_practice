cmake_minimum_required(VERSION 3.16)
project(Signal_Analyzer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Qtの自動処理（MOC）を有効化（Qtのコア機能に必要）
set(CMAKE_AUTOMOC ON)

# 2. Qt6の必要なパッケージ（Core, Gui, Widgets）を検索
# ※環境によっては "Qt5" に書き換えてください
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# 3. 実行ファイルの定義
add_executable(Signal_Analyzer main.cpp)
target_sources(Signal_Analyzer PRIVATE
	src/signal_analyzer.cpp
)
target_include_directories(Signal_Analyzer PRIVATE src)

# 4. Qtのライブラリをプログラムにリンク
target_link_libraries(Signal_Analyzer PRIVATE Qt6::Widgets)
