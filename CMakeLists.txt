cmake_minimum_required(VERSION 3.5)

project(Spread LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()




find_package(Qt5 COMPONENTS Widgets REQUIRED)

if(ANDROID)
  add_library(Spread SHARED
    main.cpp
    ui/mainwindow.cpp
    ui/mainwindow.h
    ui/mainwindow.ui
  )
else()
  add_executable(Spread
    main.cpp
    ui/mainwindow.cpp
    ui/mainwindow.h
    ui/mainwindow.ui
  )
endif()

target_link_libraries(Spread PRIVATE Qt5::Widgets)

if(APPLE)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/include)
    find_library(libavcodec.58.77.101 libavcodec.58.77.101.dylib PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib)
    find_library(libavdevice.58.9.103 libavdevice.58.9.103.dylib PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib)
    find_library(libavfilter.7.77.101 libavfilter.7.77.101.dylib PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib)
    find_library(libavformat.58.42.100 libavformat.58.42.100.dylib PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib)
    find_library(libavutil.56.42.102 libavutil.56.42.102.dylib PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib)
    find_library(libpostproc.55.6.100 libpostproc.55.6.100.dylib PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib)
    find_library(libswresample.3.6.100 libswresample.3.6.100.dylib PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib)
    find_library(libswscale.5.6.101 libswscale.5.6.101.dylib PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib)


    #link_directories(${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib)
else()

endif()

if(APPLE)
    target_link_libraries(
        Spread PUBLIC
        ${libavcodec.58.77.101}
        ${libavdevice.58.9.103}
        ${libavfilter.7.77.101}
        ${libavformat.58.42.100}
        ${libavutil.56.42.102}
        ${libpostproc.55.6.100}
        ${libswresample.3.6.100}
        ${libswscale.5.6.101}
        )
else()
endif()
