cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

SET(ASSIMP_BUILD_TESTS CACHE BOOL OFF)

Set(ThirdPartyFolder third-party)


if (APPLE)
	#define Molten sdk path here!
	Set(MacVulkanSDK /Users/pine/lib/vulkansdk-macos-1.1.130.0)

	Set(VULKAN_SDK ${MacVulkanSDK}/macOS)
	Set(VK_LAYER_PATH ${MacVulkanSDK}/macOS/share/vulkan/explicit_layer.d)
	Set(VK_ICD_FILENAMES ${MacVulkanSDK}/MoltenVK/dylib/macOS/MoltenVK_icd.json)
	Set(Vulkan_LIBRARY ${MacVulkanSDK}/MoltenVK/dylib/macOS/libMoltenVK.dylib)
	Set(Vulkan_INCLUDE_DIR ${MacVulkanSDK}/MoltenVK/include)
	Set(DYLD_LIBRARY_PATH ${MacVulkanSDK}/macOS/lib)
	Set(VK_SDK_PATH ${MacVulkanSDK}/MoltenVK/)
	Set(PATH ${MacVulkanSDK}/macOS/bin:$PATH)	
endif()

# Add third party libraries
add_subdirectory(${ThirdPartyFolder})

add_subdirectory(src/math src/math/build)
add_subdirectory(src/shape src/shape/build)
add_subdirectory(src/BVH src/BVH/build)
add_subdirectory(src/VulkanLib src/VulkanLib/build)
add_subdirectory(src/Renderer src/Renderer/build)


include_directories(src/glad)
include_directories(src/GLFW/include)


###assimp
add_subdirectory(src/assimp src/assimp/build)
LINK_DIRECTORIES (${Assimp_BINARY_DIR} ${AssetImporter_BINARY_DIR}/lib)
include_directories(src/assimp/include)


project(PineRdr)

file(GLOB_RECURSE project_headers src/Core/*.h src/Core/*.hpp)
file(GLOB_RECURSE project_cpps src/Core/*.cpp)

set(all_files ${project_headers} ${project_cpps} ${project_cs} ${project_ccs})
#source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/Core FILES ${all_files})
source_group("Header Files" FILES ${project_headers})
source_group("Source Files" FILES ${project_cpps})

add_executable(PineRdr src/main.cpp ${all_files})
set_target_properties( PineRdr PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/working")
target_include_directories(PineRdr PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/Core)
target_link_libraries(PineRdr PRIVATE Renderer)
target_link_libraries(PineRdr PRIVATE math)
target_link_libraries(PineRdr PRIVATE shape)
target_link_libraries(PineRdr PRIVATE BVH)
target_link_libraries(PineRdr PRIVATE VulkanLib)
target_link_libraries(PineRdr PRIVATE ${glm_LIBRARIES} glm::glm)
target_link_libraries(PineRdr PRIVATE glfw ${GLFW_LIBRARIES})



if (APPLE)
  target_link_libraries(PineRdr PRIVATE "-framework OpenGL")
endif()


if(WIN32)
  find_package(OpenGL REQUIRED)
  target_link_libraries(PineRdr PRIVATE ${OPENGL_LIBRARIES})
endif()