cmake_minimum_required (VERSION 3.23)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

file(GLOB hdr_7zap_test *.h)
file(GLOB src_7zap_test *.cpp)

add_executable(7ZapTest)
target_sources(7ZapTest PRIVATE ${src_7zap_test}
				PRIVATE ${hdr_7zap_test})

target_include_directories(7ZapTest
	PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(7ZapTest 
	$<LINK_LIBRARY:WHOLE_ARCHIVE,7zrLib>)

target_precompile_headers(7ZapTest
	PRIVATE pch.h)
