# 编译为动态库
add_library(${ProjectName} STATIC)
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
