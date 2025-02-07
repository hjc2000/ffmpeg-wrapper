cmake_minimum_required(VERSION 3.8)
include($ENV{cpp_lib_build_scripts_path}/cmake-module/setup.cmake)
include(target_import_ffmpeg)
include(target_import_base)
include(target_import_pinvoke)

set(ProjectName ffmpeg-wrapper)
project(${ProjectName})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)


# 编译为动态库
add_library(${ProjectName})
target_import_src(${ProjectName})
target_import_ffmpeg(${ProjectName} PUBLIC)
target_import_base(${ProjectName} PUBLIC)
target_import_pinvoke(${ProjectName} PUBLIC)

# 添加测试程序
if(1)
	add_executable(test ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
	target_compile_definitions(test PRIVATE Predefine_ResourceDir="D:/video/视频开发测试")

	target_link_libraries(${ProjectName} PUBLIC -Wl,--start-group)
	target_link_libraries(test ${ProjectName})
	target_link_libraries(${ProjectName} PUBLIC -Wl,--end-group)

	install(TARGETS test
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)
endif()
