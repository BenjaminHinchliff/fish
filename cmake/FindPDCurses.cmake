cmake_minimum_required(VERSION 3.11)

include(FetchContent)

FetchContent_Declare(
	pdcurses
	GIT_REPOSITORY https://github.com/wmcbrine/PDCurses.git
	GIT_TAG 3.9
)
FetchContent_GetProperties(pdcurses)
if(NOT pdcurses_POPULATED)
	FetchContent_Populate(pdcurses)
	set(PDCURSES_LIB "${pdcurses_SOURCE_DIR}/wincon/pdcurses.lib")
	add_custom_command(
		OUTPUT ${PDCURSES_LIB}
		COMMAND nmake /f Makefile.vc
		WORKING_DIRECTORY "${pdcurses_SOURCE_DIR}/wincon"
	)
	add_custom_target(pdcurses_build DEPENDS ${PDCURSES_LIB})
	add_library(pdcurses STATIC IMPORTED)
	target_include_directories(pdcurses INTERFACE ${pdcurses_SOURCE_DIR})
	set_property(TARGET pdcurses PROPERTY IMPORTED_LOCATION ${PDCURSES_LIB})
endif()
