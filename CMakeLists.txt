project(netlizard)

add_definitions(-D_NL_STATIC)
set(CMAKE_CXX_STANDARD 17)

include_directories(libnetlizard)

add_subdirectory(libnetlizard)

SET(LIBRARY_OUTPUT_PATH .)

link_directories(
	${PROJECT_SOURCE_DIR}/libnetlizard
)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

add_executable(
	netlizard

	main.cpp
	str.cpp
	filesystem.cpp
	buffer.cpp
	converter.cpp
	converter_texture.cpp
	converter_material.cpp
	converter_map.cpp
	dict.cpp
	material.cpp
	map.cpp
	)
add_library(libnetlizard STATIC IMPORTED)
set_target_properties(libnetlizard PROPERTIES IMPORTED_LOCATION libnetlizard/libnetlizard.a)
target_link_libraries(
	netlizard
	PUBLIC
	libnetlizard
	)
