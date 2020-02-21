cmake_minimum_required(VERSION 3.10)


project(Renderer)
add_library(Renderer Camera.h Camera.cpp RenderView.h RenderView.cpp Scene.h Scene.cpp TestScene1.h Material.h Renderer.h Renderer.cpp ParallelRenderer.h ParallelRenderer.cpp VulkanRenderer.h VulkanRenderer.cpp)
target_link_libraries(Renderer math)
target_link_libraries(Renderer shape)
target_link_libraries(Renderer VulkanLib)
target_include_directories(Renderer INTERFACE ..)
