cmake_minimum_required (VERSION 3.23)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

file(GLOB hdr_7zap_test *.h)
file(GLOB src_7zap_test *.cpp)

add_executable(7ZapTest)
target_sources(7ZapTest PRIVATE ${src_7zap_test}
				PRIVATE ${hdr_7zap_test})

target_link_libraries(7ZapTest 7Zap)
add_custom_command(TARGET 7ZapTest POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:7ZapTest> $<TARGET_FILE_DIR:7ZapTest>
  COMMAND_EXPAND_LISTS
  )
