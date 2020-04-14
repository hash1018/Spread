cmake_minimum_required(VERSION 3.14)
project(Spread C CXX)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

#set(CMAKE_PREFIX_PATH "/Users/andrea/Developer/Qt5/5.2.1/clang_64")

find_package(Qt5 COMPONENTS OpenGL Widgets REQUIRED)

add_subdirectory(lib/ffmpeg)


list(APPEND SOURCES
    src/main.cpp
    src/ui/mainwindow.cpp
    src/ui/mainwindow.h
    src/ui/mainwindow.ui
    src/ui/videoplayerwidget.cpp
    src/ui/videoplayerwidget.h
    src/ui/playvideodialog.cpp
    src/ui/playvideodialog.h
    src/ui/playvideodialog.ui
    src/spread-ffmpeg/videoreader.cpp
    src/spread-ffmpeg/videoreader.h
    src/spread-ffmpeg/framedata.cpp
    src/spread-ffmpeg/framedata.h
    src/spread-ffmpeg/remuxer.cpp
    src/spread-ffmpeg/remuxer.h
    src/spread-ffmpeg/hwaccelsdecoder.cpp
    src/spread-ffmpeg/hwaccelsdecoder.h
)

add_executable(Spread ${SOURCES})

target_link_libraries(Spread ffmpeg Qt5::OpenGL Qt5::Widgets )
