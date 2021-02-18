cmake_minimum_required(VERSION 3.11)

set(FISH_HDRS
	"fish.hpp")

set(FISH_SRCS
	"fish.cpp"
	"main.cpp")

add_executable(fish ${FISH_HDRS} ${FISH_SRCS})

if(MSVC)
	find_package(PDCurses REQUIRED)
	target_link_libraries(fish PUBLIC pdcurses)
else()
	find_package(Curses REQUIRED)
	target_include_directories(fish PUBLIC ${CURSES_INCLUDE_DIRS})
	target_link_libraries(fish PUBLIC ${CURSES_LIBRARIES})
endif()
