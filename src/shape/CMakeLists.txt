cmake_minimum_required(VERSION 3.10)

project(shape)

add_library(shape Shape.h Shape.cpp Sphere.h Sphere.cpp Triangle.h Triangle.cpp Plane.h)
target_link_libraries(shape math)
target_link_libraries(shape glm)
target_include_directories(shape INTERFACE ..)
