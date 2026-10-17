# The macOS port: one executable, Steamboat, of an object library per original binary
# (port/flags.py) and the platform, which implements the Windows, DirectX and media APIs.
#   cmake -S port -B out/port/build -G Ninja && cmake --build out/port/build
cmake_minimum_required(VERSION 3.24)
project(MickeyPort C CXX)

# optimised by default: the software renderer needs it (the game's own modules set
# -O1 per file in port/flags.py, which comes after these flags)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "build type" FORCE)
endif()

find_package(Python3 REQUIRED COMPONENTS Interpreter)
find_package(SDL3 REQUIRED CONFIG)

set(CMAKE_OSX_ARCHITECTURES arm64)
set(CMAKE_OSX_DEPLOYMENT_TARGET 13.0)

# optional sanitizer build, for precise crash reports from playtests:
#   cmake -S port -B out/port/build-asan -G Ninja -DPORT_SANITIZE=address,undefined
# (every target: the platform, the modules and the executable; default off)
set(PORT_SANITIZE "" CACHE STRING "clang sanitizers for every target, e.g. address,undefined (empty: none)")
if(PORT_SANITIZE)
  add_compile_options(-fsanitize=${PORT_SANITIZE} -fno-omit-frame-pointer)
  add_link_options(-fsanitize=${PORT_SANITIZE})
endif()


# Win32, DirectDraw/Direct3D 7, Miles, Bink and Smacker for the game, on the host
# backends of the target (platform/host/host.h)
set(PORT_HOST_BACKENDS sdl coretext posix)
set(PLATFORM_DIRS platform platform/win32 platform/dx platform/media)
foreach(b ${PORT_HOST_BACKENDS})
  list(APPEND PLATFORM_DIRS platform/host/${b})
endforeach()
set(PLATFORM_SOURCES)
foreach(d ${PLATFORM_DIRS})
  file(GLOB s CONFIGURE_DEPENDS ${d}/*.cpp ${d}/*.c)
  list(APPEND PLATFORM_SOURCES ${s})
endforeach()
add_library(platform OBJECT ${PLATFORM_SOURCES} platform/README.md)
target_include_directories(platform SYSTEM PRIVATE compat/include)
# the platform sees the game's own headers (d3d7.h, ...) as the modules do: through the
# overlay (written when flags.py generates modules.cmake below)
target_compile_options(platform PRIVATE -ivfsoverlay ${CMAKE_CURRENT_SOURCE_DIR}/../out/port/overlay.yaml)
target_compile_options(platform PRIVATE "SHELL:-Xclang -mconstructor-aliases")
target_compile_options(platform PRIVATE -include ${CMAKE_CURRENT_SOURCE_DIR}/compat/prelude.h
  $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++17>)
target_link_libraries(platform PUBLIC SDL3::SDL3 "-framework CoreText" "-framework CoreGraphics" "-framework CoreFoundation")
# FFmpeg decodes Bink, Smacker and the sounds (platform/media)
find_path(FFMPEG_INCLUDE_DIR libavformat/avformat.h REQUIRED)
target_include_directories(platform SYSTEM PRIVATE ${FFMPEG_INCLUDE_DIR})
foreach(lib avformat avcodec avutil swresample swscale)
  find_library(FFMPEG_${lib} ${lib} REQUIRED)
  target_link_libraries(platform PUBLIC ${FFMPEG_${lib}})
endforeach()

# the modules, from port/flags.py
execute_process(
  COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/flags.py cmake ${CMAKE_BINARY_DIR}/modules.cmake
  COMMAND_ERROR_IS_FATAL ANY)
# the overlay (overrides, rewrite rules, verified edits) is generated at configure
# time: any change to its inputs reconfigures
# (the src/ files too: a rewritten copy is regenerated only when configuring, so an edit
# to its src/ original must reconfigure, or the build compiles the stale copy)
file(GLOB_RECURSE PORT_OVERLAY_INPUTS CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/override/* ${CMAKE_CURRENT_SOURCE_DIR}/edits/*.py
  ${CMAKE_CURRENT_SOURCE_DIR}/improve/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/*.c ${CMAKE_CURRENT_SOURCE_DIR}/../src/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/*.h)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/flags.py ${CMAKE_CURRENT_SOURCE_DIR}/rewrite.py
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/options.txt ${PORT_OVERLAY_INPUTS})
include(${CMAKE_BINARY_DIR}/modules.cmake)
