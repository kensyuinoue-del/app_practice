cmake_minimum_required(VERSION 3.16)
project(PracticeApps LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

add_subdirectory(apps/gui_practice)
add_subdirectory(apps/calculator)
add_subdirectory(apps/signal_analyzer)
