cmake_minimum_required(VERSION 3.4.1)
set(ffmpeg_lib_dir /opt/ffmpeg/lib)
set(ffmpeg_head_dir /opt/ffmpeg/include)
SET( CMAKE_CXX_FLAGS "-std=c++11 ")
set(CMAKE_BUILD_TYPE "Debug")
add_compile_options(-fno-elide-constructors)

# 2. 添加头文件目录
include_directories(${ffmpeg_head_dir})

add_library( avutil
        SHARED
        IMPORTED )
set_target_properties( avutil
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libavutil.so )

add_library( swresample
        SHARED
        IMPORTED )
set_target_properties( swresample
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libswresample.so )

add_library( avcodec
        SHARED
        IMPORTED )
set_target_properties( avcodec
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libavcodec.so )

add_library( avfilter
        SHARED
        IMPORTED)
set_target_properties( avfilter
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libavfilter.so )

add_library( swscale
        SHARED
        IMPORTED)
set_target_properties( swscale
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libswscale.so )

add_library( avformat
        SHARED
        IMPORTED)
set_target_properties( avformat
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libavformat.so )

include_directories(${ffmpeg_head_dir}/include)


add_executable(demo_yec trans.cpp)
target_link_libraries(
        demo_yec
        avutil
        swresample
        avcodec
        avfilter
        swscale
        avformat
        )