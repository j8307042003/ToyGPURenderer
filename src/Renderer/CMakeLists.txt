cmake_minimum_required(VERSION 3.10)



project(Renderer)

file(GLOB_RECURSE project_headers *.h *.hpp)
file(GLOB_RECURSE project_cpps *.cpp)

file(GLOB_RECURSE PathTraceFolderHeader PathTrace/*.h PathTrace/*.hpp)
file(GLOB_RECURSE PathTraceFolderCpp PathTrace/*.cpp)

file(GLOB_RECURSE CameraFolderHeader Camera/*.h Camera/*.hpp)
file(GLOB_RECURSE CameraFolderCpp Camera/*.cpp)

source_group("Header Files/PathTrace" FILES ${PathTraceFolderHeader})
source_group("Source Files/PathTrace" FILES ${PathTraceFolderCpp})

source_group("Header Files/Camera" FILES ${CameraFolderHeader})
source_group("Source Files/Camera" FILES ${CameraFolderCpp})

set(all_files ${project_headers} ${project_cpps} ${project_cs} ${project_ccs})
add_library(Renderer ${all_files})
target_include_directories(Renderer PRIVATE ${CMAKE_SOURCE_DIR}/third-party/oidnlib/include)
target_include_directories(Renderer PRIVATE ${CMAKE_SOURCE_DIR}/third-party/tbb/include)
target_include_directories(Renderer PRIVATE ${CMAKE_SOURCE_DIR}/third-party/tbb/include)
target_link_libraries(Renderer PUBLIC math)
target_link_libraries(Renderer PUBLIC shape)
target_link_libraries(Renderer PUBLIC VulkanLib)
target_link_libraries(Renderer PUBLIC glm)
target_link_libraries(Renderer PUBLIC assimp)
#target_link_libraries(Renderer PUBLIC OpenImageDenoise)
#target_link_libraries(Renderer TBB)
#target_link_libraries(Renderer PUBLIC TBB)
target_include_directories(Renderer INTERFACE ..)

if(WIN32)
#TARGET_LINK_LIBRARIES(Renderer ${OIDN_LIBRARIES})
#TARGET_LINK_LIBRARIES(Renderer ${TBB_LIBRARIES})
else()
#TARGET_LINK_LIBRARIES(Renderer ${OIDN_LIBRARIES} dl)
#TARGET_LINK_LIBRARIES(Renderer ${TBB_LIBRARIES} dl)
endif()
