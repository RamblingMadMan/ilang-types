cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)

project(ilang-types VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(
	ILANG_TYPES_HEADERS
	include/ilang/Type.hpp
)

set(
	ILANG_TYPES_SOURCES
	src/Type.cpp
)

add_library(ilang-types ${ILANG_TYPES_SOURCES})

target_include_directories(ilang-types PUBLIC include)
set_target_properties(ilang-types PROPERTIES PUBLIC_HEADER ${ILANG_TYPES_HEADERS})

install(
	TARGETS ilang-types
	ARCHIVE
		DESTINATION lib
	PUBLIC_HEADER
		DESTINATION include/ilang
)

