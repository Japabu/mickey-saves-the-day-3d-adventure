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
# the platform library exists to export the API the modules call
set_target_properties(platform PROPERTIES C_VISIBILITY_PRESET default CXX_VISIBILITY_PRESET default)
target_compile_options(platform PRIVATE -include ${CMAKE_CURRENT_SOURCE_DIR}/compat/prelude.h
  $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++17>)
target_link_libraries(platform PRIVATE SDL3::SDL3 "-framework CoreText" "-framework CoreGraphics" "-framework CoreFoundation")
# FFmpeg decodes Bink, Smacker and the sounds (platform/media)
find_path(FFMPEG_INCLUDE_DIR libavformat/avformat.h REQUIRED)
target_include_directories(platform SYSTEM PRIVATE ${FFMPEG_INCLUDE_DIR})
foreach(lib avformat avcodec avutil swresample swscale)
  find_library(FFMPEG_${lib} ${lib} REQUIRED)
  target_link_libraries(platform PRIVATE ${FFMPEG_${lib}})
endforeach()

# the modules, from port/flags.py
execute_process(
  COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/flags.py cmake ${CMAKE_BINARY_DIR}/modules.cmake
  COMMAND_ERROR_IS_FATAL ANY)
# the overlay (overrides, rewrite rules, verified edits) is generated at configure
# time: any change to its inputs reconfigures
file(GLOB_RECURSE PORT_OVERLAY_INPUTS CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/override/* ${CMAKE_CURRENT_SOURCE_DIR}/edits/*.py)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/flags.py ${CMAKE_CURRENT_SOURCE_DIR}/rewrite.py
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/options.txt ${PORT_OVERLAY_INPUTS})
include(${CMAKE_BINARY_DIR}/modules.cmake)
