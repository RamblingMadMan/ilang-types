set(
	ILANG_TYPES_HEADERS
	../include/ilang/Type.hpp
)

set(
	ILANG_TYPES_SOURCES
	Type.cpp
)

add_library(ilang-types ${ILANG_TYPES_SOURCES})
set_target_properties(ilang-types PROPERTIES PUBLIC_HEADER ${ILANG_TYPES_HEADERS})
