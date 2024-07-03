cmake_minimum_required(VERSION 3.8)
include($ENV{cpp_lib_build_scripts_path}/cmake-module/setup.cmake)
include(target_import_ffmpeg)
include(target_import_jccpp)
include(target_import_base)
include(target_import_pinvoke)
include(target_import_thread)

set(ProjectName ffmpeg-wrapper)
project(${ProjectName})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)


# 编译为动态库
add_library(${ProjectName})
target_import_src(${ProjectName})
target_import_jccpp(${ProjectName} PUBLIC)
target_import_ffmpeg(${ProjectName} PUBLIC)
target_import_base(${ProjectName} PUBLIC)
target_import_pinvoke(${ProjectName} PUBLIC)
target_import_thread(${ProjectName} PUBLIC)

set(CMAKE_INSTALL_PREFIX ${cpp_lib_build_scripts_path}/${platform}/.libs/${ProjectName}/)