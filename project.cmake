# 编译为动态库
add_library(${ProjectName} STATIC)
target_import_src(${ProjectName})
target_import_ffmpeg(${ProjectName} PUBLIC)
target_import_base(${ProjectName} PUBLIC)
target_import_pinvoke(${ProjectName} PUBLIC)

# 添加测试程序
if(1)
	set(test_exe_target_name "test")
	add_executable(${test_exe_target_name})
	target_import_test(${test_exe_target_name})

	target_compile_definitions(${test_exe_target_name} PUBLIC Predefine_ResourceDir="D:/video/视频开发测试")

	target_link_libraries(${ProjectName} PUBLIC -Wl,--start-group)
	target_link_libraries(${test_exe_target_name} PUBLIC ${ProjectName})
	target_link_libraries(${ProjectName} PUBLIC -Wl,--end-group)
endif()
