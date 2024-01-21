cmake_minimum_required (VERSION 3.23)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")					
add_compile_definitions(Z7_PROG_VARIANT_R)

file(GLOB src_c *.c)
file(GLOB hdr_c *.h)

add_library(7zC STATIC ${src_c})

target_sources(7zC PRIVATE
		PUBLIC ${hdr_c})
