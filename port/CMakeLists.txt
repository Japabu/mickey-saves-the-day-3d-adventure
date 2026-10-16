# The macOS port: one shared library per original DLL, the Steamboat executable, and
# the platform library that implements the Windows, DirectX and media APIs.
#   cmake -S port -B out/port/build -G Ninja && cmake --build out/port/build
cmake_minimum_required(VERSION 3.24)
project(MickeyPort C CXX)

find_package(Python3 REQUIRED COMPONENTS Interpreter)
find_package(SDL3 REQUIRED CONFIG)

set(CMAKE_OSX_ARCHITECTURES arm64)
set(CMAKE_OSX_DEPLOYMENT_TARGET 13.0)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_C_VISIBILITY_PRESET hidden)


# Win32, DirectDraw/Direct3D 7, Miles, Bink and Smacker for the game
file(GLOB_RECURSE PLATFORM_SOURCES CONFIGURE_DEPENDS platform/*.cpp platform/*.c)
add_library(platform SHARED ${PLATFORM_SOURCES} platform/README.md)
target_include_directories(platform SYSTEM PRIVATE compat/include)
target_compile_options(platform PRIVATE -include ${CMAKE_CURRENT_SOURCE_DIR}/compat/prelude.h
  $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++17>)
target_link_libraries(platform PRIVATE SDL3::SDL3)

# the modules, from port/flags.py
execute_process(
  COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/flags.py cmake ${CMAKE_BINARY_DIR}/modules.cmake
  COMMAND_ERROR_IS_FATAL ANY)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/flags.py ${CMAKE_CURRENT_SOURCE_DIR}/../src/options.txt)
include(${CMAKE_BINARY_DIR}/modules.cmake)
