# Copyright (c) Maia

add_subdirectory(gui)

find_package(Eigen3 CONFIG REQUIRED)
find_package(raylib CONFIG REQUIRED)

add_executable(maia main.cpp)

target_link_libraries(
  maia
  PRIVATE
    raylib
    gui
    Eigen3::Eigen
)
