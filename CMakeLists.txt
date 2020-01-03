cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(src/math)
add_subdirectory(src/shape)
add_subdirectory(src/GLFW)
add_subdirectory(src/Renderer)

include_directories(src/glad)
include_directories(src/GLFW/include)




project(App)

add_executable(App src/main.cpp)
target_link_libraries(App Renderer)
target_link_libraries(App math)
target_link_libraries(App shape)
target_link_libraries(App glfw ${GLFW_LIBRARIES})



if (APPLE)
  target_link_libraries(App "-framework OpenGL")
endif()


if(WIN32)
  find_package(OpenGL REQUIRED)
  target_link_libraries(App ${OPENGL_LIBRARIES})
endif()