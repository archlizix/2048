cmake_minimum_required(VERSION 3.16)

project(2048)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

include_directories(${CMAKE_SOURCE_DIR}/Src)

aux_source_directory(${CMAKE_SOURCE_DIR}/Src srcs)

add_executable(
	2048
	${srcs}
)

find_package(Curses)
if(CURSES_FOUND)
    target_link_libraries(2048 ${CURSES_LIBRARIES})
else(CURSES_FOUND)
    message(FATAL_ERROR "Curses library not found")
endif(CURSES_FOUND)

# https://cmake.org/cmake/help/latest/index.html
