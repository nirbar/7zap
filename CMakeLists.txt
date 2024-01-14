cmake_minimum_required (VERSION 3.15)
cmake_policy(SET CMP0091 NEW)

project ("7Zap")
set_property(GLOBAL PROPERTY COMPILE_WARNING_AS_ERROR)

add_subdirectory ("lzma-sdk")
