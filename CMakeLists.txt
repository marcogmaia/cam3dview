# Copyright (c) Maia

cmake_minimum_required(VERSION 4.0)

project(
  maia
  VERSION 0.0.1
  LANGUAGES
    C
    CXX
)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INCLUDE_CURRENT_DIR OFF)
set(
  CMAKE_MODULE_PATH
  "${CMAKE_MODULE_PATH}"
  "${CMAKE_SOURCE_DIR}/cmake"
)

if(WIN32)
  add_compile_definitions(
    NOUSER
    NOGDI
  )
endif()

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
  add_compile_options("/W4")
endif()

include_directories(src)

add_subdirectory(src/maia)
