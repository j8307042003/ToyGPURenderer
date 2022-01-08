cmake_minimum_required(VERSION 3.10)



project(Renderer)

file(GLOB_RECURSE project_headers *.h *.hpp)
file(GLOB_RECURSE project_cpps *.cpp)

file(GLOB_RECURSE PathTraceFolderHeader PathTrace/*.h PathTrace/*.hpp)
file(GLOB_RECURSE PathTraceFolderCpp PathTrace/*.cpp)

source_group("Header Files/PathTrace" FILES ${PathTraceFolderHeader})
source_group("Source Files/PathTrace" FILES ${PathTraceFolderCpp})

set(all_files ${project_headers} ${project_cpps} ${project_cs} ${project_ccs})
add_library(Renderer ${all_files})
target_link_libraries(Renderer math)
target_link_libraries(Renderer shape)
target_link_libraries(Renderer VulkanLib)
target_link_libraries(Renderer glm)
target_link_libraries(Renderer assimp)
target_include_directories(Renderer INTERFACE ..)
