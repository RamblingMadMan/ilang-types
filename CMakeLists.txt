cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)

project(ilang-types VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_subdirectory(lib)

install(
	TARGETS ilang-types
	ARCHIVE
		DESTINATION lib
	PUBLIC_HEADER
		DESTINATION include/ilang
)
