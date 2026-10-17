# The port: one executable, Steamboat, of an object library per original binary
# (port/flags.py) and the platform, which implements the Windows, DirectX and media APIs
# on the host backends of the target. Everything links statically (port/deps.py builds
# SDL3 and FFmpeg); only the system's own libraries stay shared.
#   cmake -S port -B out/port/build -G Ninja && cmake --build out/port/build   (macOS)
cmake_minimum_required(VERSION 3.24)
project(MickeyPort C CXX)

# optimised by default: the software renderer needs it (the game's own modules set
# -O1 per file in port/flags.py, which comes after these flags)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "build type" FORCE)
endif()

find_package(Python3 REQUIRED COMPONENTS Interpreter)

# The target, and its static third-party libraries (built once into out/deps/<target>)
if(EMSCRIPTEN)
  set(PORT_TARGET web)
  set(PORT_HOST_BACKENDS web)
else()
  set(PORT_TARGET macos)
  set(PORT_HOST_BACKENDS sdl coretext posix)
  set(CMAKE_OSX_ARCHITECTURES arm64)
  set(CMAKE_OSX_DEPLOYMENT_TARGET 13.0)
endif()
if(PORT_TARGET STREQUAL web)
  # threads (the game's and the renderer's) and native wasm exceptions everywhere; the
  # game runs on a worker (PROXY_TO_PTHREAD), where it may block, and starts with
  # 8 MB of stack as on Windows
  add_compile_options(-pthread -fwasm-exceptions)
  add_link_options(-pthread -fwasm-exceptions -sPROXY_TO_PTHREAD -sALLOW_MEMORY_GROWTH
    -sINITIAL_MEMORY=256MB -sSTACK_SIZE=8MB -sDEFAULT_PTHREAD_STACK_SIZE=2MB -sPTHREAD_POOL_SIZE=8
    -sEXIT_RUNTIME -sWASMFS
    # no data below 64 KB, as on Windows: the game takes values below 0x10000 for resource
    # and ordinal numbers instead of string pointers (IS_INTRESOURCE)
    -sGLOBAL_BASE=65536
    # a call through a declaration of another type traps in WebAssembly: an error
    -Wl,--fatal-warnings)
endif()
set(PORT_DEPS ${CMAKE_CURRENT_SOURCE_DIR}/../out/deps/${PORT_TARGET})
execute_process(
  COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/deps.py ${PORT_TARGET} ${PORT_DEPS}
  COMMAND_ERROR_IS_FATAL ANY)

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
if(PORT_TARGET STREQUAL macos)
  find_package(SDL3 REQUIRED CONFIG PATHS ${PORT_DEPS} NO_DEFAULT_PATH)
  target_link_libraries(platform PUBLIC SDL3::SDL3-static
    "-framework CoreText" "-framework CoreGraphics" "-framework CoreFoundation")
endif()
# FFmpeg decodes Bink, Smacker and the sounds (platform/media)
target_include_directories(platform SYSTEM PRIVATE ${PORT_DEPS}/include)
foreach(lib avformat avcodec swscale swresample avutil)
  target_link_libraries(platform PUBLIC ${PORT_DEPS}/lib/lib${lib}.a)
endforeach()
target_link_libraries(platform PUBLIC m)

# the modules, from port/flags.py
execute_process(
  COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/flags.py --target ${PORT_TARGET} cmake ${CMAKE_BINARY_DIR}/modules.cmake
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
  ${CMAKE_CURRENT_SOURCE_DIR}/deps.py ${CMAKE_CURRENT_SOURCE_DIR}/flags.py ${CMAKE_CURRENT_SOURCE_DIR}/rewrite.py
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/options.txt ${PORT_OVERLAY_INPUTS})
include(${CMAKE_BINARY_DIR}/modules.cmake)

if(PORT_TARGET STREQUAL web)
  # the game for the page (port/web): createMickey() starts it; the page reads the shared
  # block (host/web/shared.h) through the heap views
  target_link_options(Steamboat PRIVATE -sMODULARIZE -sEXPORT_NAME=createMickey -sENVIRONMENT=web,worker
    -sEXPORTED_RUNTIME_METHODS=wasmMemory,ENV -sEXPORTED_FUNCTIONS=_main,_host_web_shared)
  # the page next to it
  file(GLOB PORT_WEB_PAGE CONFIGURE_DEPENDS web/*.html web/*.js)
  add_custom_command(TARGET Steamboat POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PORT_WEB_PAGE} ${CMAKE_BINARY_DIR})
endif()
