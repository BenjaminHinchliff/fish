cmake_minimum_required(VERSION 3.8)

add_executable(fish "main.cpp")

if(MSVC)
	# if MVSC fetch and build pdcurses
	include(FetchContent)
	FetchContent_Declare(
		pdcurses
		GIT_REPOSITORY https://github.com/wmcbrine/PDCurses.git
		GIT_TAG 3.9
	)
	FetchContent_GetProperties(pdcurses)
	if(NOT pdcurses_POPULATED)
		FetchContent_Populate(pdcurses)
		add_custom_command(
			OUTPUT "${pdcurses_SOURCE_DIR}/wincon/pdcurses.lib"
			COMMAND nmake /f Makefile.vc
			WORKING_DIRECTORY "${pdcurses_SOURCE_DIR}/wincon"
		)
		add_custom_target(pdcurses_build DEPENDS "${pdcurses_SOURCE_DIR}/wincon/pdcurses.lib")
		add_library(pdcurses STATIC IMPORTED)
		target_include_directories(pdcurses INTERFACE "${pdcurses_SOURCE_DIR}")
		set_property(TARGET pdcurses PROPERTY IMPORTED_LOCATION "${pdcurses_SOURCE_DIR}/wincon/pdcurses.lib")
	endif()
	target_link_libraries(fish PUBLIC pdcurses)
else()
	find_package(Curses REQUIRED)
	target_include_directories(fish PUBLIC ${CURSES_INCLUDE_DIRS})
	target_link_libraries(fish PUBLIC ${CURSES_LIBRARIES})
endif()
