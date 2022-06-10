cmake_minimum_required(VERSION 3.23)
project(omfgdogs C)

find_package(raylib)
if (raylib_FOUND)
set(RAYLIB_INCLUDE_DIR ${raylib_INCLUDE_DIR})
set(RAYLIB_LIBRARY ${raylib_LIBRARY})
endif(raylib_FOUND)

add_executable(omfgdogs main.c)
target_include_directories(omfgdogs PRIVATE ${RAYLIB_INCLUDE_DIR})
target_link_libraries(omfgdogs ${RAYLIB_LIBRARY})

install(TARGETS omfgdogs DESTINATION bin)
install(FILES
 ${CMAKE_CURRENT_SOURCE_DIR}/../assets/omfgdogs.mp3
 ${CMAKE_CURRENT_SOURCE_DIR}/../assets/omfgdogs.gif
 DESTINATION share/omfgdogs)
