project(netlizard)

add_definitions(-D_NL_STATIC)
set(CMAKE_CXX_STANDARD 17)

include_directories(libnetlizard . src src/lib src/io src/id src/math)

add_subdirectory(libnetlizard)

SET(LIBRARY_OUTPUT_PATH .)

link_directories(
	${PROJECT_SOURCE_DIR}/libnetlizard
)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

SET(SRC_LIST main.cpp
src/lib/str.cpp
src/lib/buffer.cpp
src/lib/dict.cpp
src/io/filesystem.cpp
src/id/material.cpp
src/id/brush.cpp
src/id/entity.cpp
src/id/map.cpp
src/math/maths.cpp
src/math/vector.cpp
src/math/matrix.cpp
src/math/plane.cpp
src/converter.cpp
src/converter_texture.cpp
src/converter_material.cpp
src/converter_map.cpp
)

add_executable(
	netlizard

	${SRC_LIST}
	)
add_library(libnetlizard STATIC IMPORTED)
set_target_properties(libnetlizard PROPERTIES IMPORTED_LOCATION libnetlizard/libnetlizard.a)
target_link_libraries(
	netlizard
	PUBLIC
	libnetlizard
	)
