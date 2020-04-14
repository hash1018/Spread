

`cmake_minimum_required(VERSION 3.14)
project(Spread C CXX)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_subdirectory(lib/glfw)
add_subdirectory(lib/ffmpeg)

add_definitions(-DGL_SILENCE_DEPRECATION)

if(APPLE)
    list(APPEND EXTRA_LIBS
        "-framework OpenGL"     
    )

elseif(WIN32)
    list(APPEND EXTRA_LIBS
        "-lglu32 -lopengl32"
    )
    set(CMAKE_EXE_LINKER_FLAGS "-std=gnu99 -static -static-libgcc -static-libstdc++ -mwindows")

else()
    list(APPEND EXTRA_LIBS
         "lGL -lGLU lX11"
    )

endif()

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
    src/spread-ffmpeg/loadframe.h
    src/spread-ffmpeg/loadframe.cpp
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

target_link_libraries(Spread Qt5::Widgets ffmpeg glfw ${EXTRA_LIBS})
