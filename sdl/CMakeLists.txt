cmake_minimum_required(VERSION 3.23)
project(omfgdogs C)
set(CMAKE_C_STANDARD_REQUIRED 11)
set(CMAKE_C_STANDARD 11)
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
find_package(SDL3_image REQUIRED)

add_executable(omfgdogs main.c)
target_link_libraries(omfgdogs PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
)

install(TARGETS omfgdogs DESTINATION bin)
install(FILES
 ${CMAKE_CURRENT_SOURCE_DIR}/../assets/omfgdogs.mp3
 ${CMAKE_CURRENT_SOURCE_DIR}/../assets/omfgdogs.gif
 DESTINATION share/omfgdogs)
