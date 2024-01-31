cmake_minimum_required (VERSION 3.23)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")					

file(GLOB hdr_7zap pch.h)
file(GLOB hdr_7zap UpdateCallback7Zap.h)
file(GLOB pub_hdr_7zap 7Zap.h)
file(GLOB src_7zap *.cpp)
list(APPEND src_7zap 7Zap.def)
list(APPEND src_7zap 7Zap.rc)

add_library(7Zap SHARED $<TARGET_OBJECTS:7zrObj>)
target_sources(7Zap PRIVATE ${src_7zap}
				PRIVATE ${hdr_7zap}
				PUBLIC ${pub_hdr_7zap})

target_include_directories(7Zap
	PRIVATE ${CMAKE_SOURCE_DIR})

target_precompile_headers(7Zap
	PRIVATE pch.h)
