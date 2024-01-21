cmake_minimum_required (VERSION 3.23)
enable_language(CSharp)
SET(CMAKE_CSharp_FLAGS "/platform:AnyCPU /langversion:8")

file(GLOB_RECURSE cs_files ${CMAKE_CURRENT_SOURCE_DIR} *.cs)

add_executable (7Zap.NET
				${cs_files})

set_property(TARGET 7Zap.NET 
				PROPERTY DOTNET_TARGET_FRAMEWORK netstandard2)
set_property(TARGET 7Zap.NET 
				PROPERTY DOTNET_SDK Microsoft.NET.Sdk)
set_property(TARGET 7Zap.NET 
				PROPERTY VS_PACKAGE_REFERENCES "System")
add_dependencies(7Zap.NET 7Zap)
