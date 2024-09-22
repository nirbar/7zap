cmake_minimum_required (VERSION 3.23)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Version number
set (VERSION_MAJOR 24 CACHE STRING "Version number major (first figure)")
set (VERSION_MINOR 8 CACHE STRING "Version number minor (second figure)")
set (VERSION_BUILD 0 CACHE STRING "Version number build (third figure)")
configure_file(7Zap.rc 7Zap.rc)

file(GLOB hdr_7zap pch.h)
file(GLOB hdr_7zap UpdateCallback7Zap.h)
file(GLOB pub_hdr_7zap 7Zap.h)
file(GLOB src_7zap *.cpp)
list(APPEND src_7zap 7Zap.def)
list(APPEND src_7zap ${CMAKE_CURRENT_BINARY_DIR}/7Zap.rc)

add_library(7Zap SHARED)
target_link_libraries(7Zap $<LINK_LIBRARY:WHOLE_ARCHIVE,7zrLib>)
target_sources(7Zap PRIVATE ${src_7zap}
				PRIVATE ${hdr_7zap}
				PUBLIC ${pub_hdr_7zap})

target_include_directories(7Zap
	PRIVATE ${CMAKE_SOURCE_DIR})

target_precompile_headers(7Zap
	PRIVATE pch.h)

if(${CMAKE_GENERATOR_PLATFORM} STREQUAL "Win32")
	target_link_options(7Zap PUBLIC "-SAFESEH:NO")
endif()
