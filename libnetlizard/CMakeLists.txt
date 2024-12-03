project(libnetlizard)

include_directories(soil)
file(GLOB_RECURSE SRC_MAIN_LIST "*.c" CONFIGURE)

aux_source_directory (soil SRC_SOIL_LIST)
include_directories(soil)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY .)

add_definitions(-D_NL_STATIC)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

add_library(
	netlizard

	STATIC

	${SRC_SOIL_LIST}
	${SRC_MAIN_LIST}
	)
