cmake_minimum_required(VERSION 3.11)

add_executable(fish "main.cpp")

find_package(Boost REQUIRED)
target_include_directories(fish PUBLIC ${Boost_INCLUDE_DIRS})

if(MSVC)
	find_package(PDCurses REQUIRED)
	target_link_libraries(fish PUBLIC pdcurses)
else()
	find_package(Curses REQUIRED)
	target_include_directories(fish PUBLIC ${CURSES_INCLUDE_DIRS})
	target_link_libraries(fish PUBLIC ${CURSES_LIBRARIES})
endif()
