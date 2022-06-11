cmake_minimum_required(VERSION 3.20)
project(omfgdogs C)

find_package(raylib 4.0 REQUIRED)
if (raylib_FOUND)
set(RAYLIB_INCLUDE_DIR ${raylib_INCLUDE_DIR})
set(RAYLIB_LIBRARY ${raylib_LIBRARY})
endif(raylib_FOUND)

include(CheckFunctionExists)
CHECK_FUNCTION_EXISTS(GetApplicationDirectory RAYLIB42)

if (RAYLIB42)
set(RAYLIB_VERSION 4200)
else(RAYLIB42)
set(RAYLIB_VERSION 4000)
set(SOURCES polyfill42.c)
endif(RAYLIB42)

set(EXECUTABLE_NAME omfgdogs)

add_executable(${EXECUTABLE_NAME} main.c ${SOURCES})
if(MSVC)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
set_target_properties(${EXECUTABLE_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif(MSVC)

target_compile_definitions(${EXECUTABLE_NAME} PRIVATE -DRAYLIB ${RAYLIB_VERSION})
target_compile_definitions(${EXECUTABLE_NAME} PRIVATE -DPLATFORM_DESKTOP)
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${RAYLIB_INCLUDE_DIR})
target_link_libraries(${EXECUTABLE_NAME} ${RAYLIB_LIBRARY})

if (WIN32)
install(TARGETS ${EXECUTABLE_NAME})
install(FILES
 ${CMAKE_CURRENT_SOURCE_DIR}/../assets/omfgdogs.mp3
 ${CMAKE_CURRENT_SOURCE_DIR}/../assets/omfgdogs.gif
 DESTINATION .
)
else(WIN32)
install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)
install(FILES
 ${CMAKE_CURRENT_SOURCE_DIR}/../assets/omfgdogs.mp3
 ${CMAKE_CURRENT_SOURCE_DIR}/../assets/omfgdogs.gif
 DESTINATION share/${EXECUTABLE_NAME})
endif(WIN32)
