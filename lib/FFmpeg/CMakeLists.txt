cmake_minimum_required(VERSION 3.14)
project(ffmpeg)



if(APPLE)

    #include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/include)
    #find_library(libavcodec.58.77.101 libavcodec.58.77.101.dylib PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib)
    #find_library(libavdevice.58.9.103 libavdevice.58.9.103.dylib PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib)
    #find_library(libavfilter.7.77.101 libavfilter.7.77.101.dylib PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib)
    #find_library(libavformat.58.42.100 libavformat.58.42.100.dylib PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib)
    #find_library(libavutil.56.42.102 libavutil.56.42.102.dylib PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib)
    #find_library(libpostproc.55.6.100 libpostproc.55.6.100.dylib PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib)
    #find_library(libswresample.3.6.100 libswresample.3.6.100.dylib PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib)
    #find_library(libswscale.5.6.101 libswscale.5.6.101.dylib PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib)



    add_library(ffmpeg INTERFACE IMPORTED GLOBAL)
    target_include_directories(ffmpeg INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/include
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/include/libavcodec
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/include/libavdevice
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/include/libavfilter
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/include/libavformat
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/include/libavutil
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/include/libpostproc
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/include/libswscale
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/include/libswresample
    )


    target_link_options(ffmpeg INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib/libavcodec.58.77.101.dylib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib/libavformat.58.42.100.dylib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib/libavfilter.7.77.101.dylib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib/libavdevice.58.9.103.dylib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib/libavutil.56.42.102.dylib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib/libpostproc.55.6.100.dylib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib/libswresample.3.6.100.dylib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-mac/lib/libswscale.5.6.101.dylib

)



else()



endif()



if(APPLE)
    #target_link_libraries(ffmpeg INTERFACE
    #${libavcodec.58.77.101}
    #${libavdevice.58.9.103}
    #${libavfilter.7.77.101}
    #${libavformat.58.42.100}
    #${libavutil.56.42.102}
    #${libpostproc.55.6.100}
    #${libswresample.3.6.100}
    #${libswscale.5.6.101}
    #)


else()



endif()