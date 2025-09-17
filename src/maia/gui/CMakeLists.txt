# Copyright (c) Maia

find_package(imgui CONFIG REQUIRED)
find_package(raylib CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

add_library(gui)
target_sources(gui PUBLIC imgui_extensions.cpp)
target_link_libraries(
  gui
  PUBLIC
    imgui::imgui
    raylib
    glfw
)
