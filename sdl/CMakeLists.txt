cmake_minimum_required(VERSION 3.23)
project(omfgdogs C)

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)

add_executable(omfgdogs main.c)
# target_include_directories(omfgdogs PRIVATE ${RAYLIB_INCLUDE_DIR})
target_link_libraries(omfgdogs SDL3::SDL3)

install(TARGETS omfgdogs DESTINATION bin)
install(FILES
 ${CMAKE_CURRENT_SOURCE_DIR}/../assets/omfgdogs.mp3
 ${CMAKE_CURRENT_SOURCE_DIR}/../assets/omfgdogs.gif
 DESTINATION share/omfgdogs)
