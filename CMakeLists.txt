cmake_minimum_required(VERSION 3.8)

if(1)
	get_filename_component(CURRENT_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
	set(ProjectName "${CURRENT_DIR_NAME}")

	include($ENV{cpp_lib_build_scripts_path}/cmake-module/setup.cmake)

	project(${ProjectName})
	enable_language(ASM)
	set(CMAKE_CXX_STANDARD 20)
	set(CMAKE_C_STANDARD 17)
endif()

include(target_import_ffmpeg)
include(target_import_base)
include(target_import_pinvoke)



# 编译为动态库
add_library(${ProjectName})
target_import_src(${ProjectName})
target_import_ffmpeg(${ProjectName} PUBLIC)
target_import_base(${ProjectName} PUBLIC)
target_import_pinvoke(${ProjectName} PUBLIC)

# 添加测试程序
if(1)
	set(exe_name "test")
	add_executable(${exe_name} ${CMAKE_CURRENT_SOURCE_DIR}/exe/main.cpp)
	target_compile_definitions(${exe_name} PRIVATE Predefine_ResourceDir="D:/video/视频开发测试")

	target_link_libraries(${ProjectName} PUBLIC -Wl,--start-group)
	target_link_libraries(${exe_name} ${ProjectName})
	target_link_libraries(${ProjectName} PUBLIC -Wl,--end-group)

	target_install(${exe_name})
endif()
