cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
include(ExternalProject)

if(NOT TARGET glm)
    message(STATUS "Gathering GLM from submodules")
    option(GLM_QUIET "No CMake Message" ON)
    option(GLM_TEST_ENABLE "Build unit tests" OFF)
    option(GLM_TEST_ENABLE_CXX_14 "Enable C++ 14" ON)
    set(CMAKE_EXPORT_NO_PACKAGE_REGISTRY ON)
    set(CMAKE_INSTALL_LIBDIR lib)
    add_subdirectory(glm)

    # WAR for https://github.com/g-truc/glm/issues/854
    if(CMAKE_COMPILER_IS_GNUCXX AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.0)
      target_compile_options(glm INTERFACE -Wno-error=class-memaccess)
    endif()
endif()



project(glad)
add_library(glad STATIC)
set(GLAD_DIR glad)
target_include_directories(glad PUBLIC ${GLAD_DIR})
target_sources( glad PRIVATE glad/glad.c )

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)


#dear imgui
set(IMGUI_DIR imgui)
# include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends ..)
project(IMGUI)
add_library(IMGUI STATIC)
# target_compile_definitions(IMGUI PUBLIC IMGUI_IMPL_OPENGL_LOADER_CUSTOM)

target_sources( IMGUI
                PRIVATE
                    ${IMGUI_DIR}/imgui_demo.cpp
                    ${IMGUI_DIR}/imgui_draw.cpp
                    ${IMGUI_DIR}/imgui_tables.cpp
                    ${IMGUI_DIR}/imgui_widgets.cpp
                    ${IMGUI_DIR}/imgui.cpp

                PRIVATE
                    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
                    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
                )

target_include_directories( IMGUI
                            PUBLIC ${IMGUI_DIR}
                            PUBLIC ${IMGUI_DIR}/backends
                            )
target_link_libraries(IMGUI glfw ${GLFW_LIBRARIES})


# file(GLOB sources *.cpp)

#oneTBB
set(TBB_DIR tbb/lib/cmake/TBB)
find_package(TBB REQUIRED)
#project(TBB)
#add_library(TBB SHARED IMPORTED)
#set(TBB_LIBRARIES "TBB")
#set(TBB_LIBDIR tbb)
#set(TBB_LIBDIR tbb/bin)
#
#
#set_target_properties(TBB PROPERTIES
#    IMPORTED_LOCATION "${TBB_LIBDIR}/bin"
#    IMPORTED_IMPLIB "${TBB_LIBDIR}/lib"
#    INTERFACE_INCLUDE_DIRECTORIES "${TBB_LIBDIR}/include"
#)



#Open Image Denoise
set(OpenImageDenoise_DIR oidnlib/lib/cmake/OpenImageDenoise-1.4.2)
set(OIDN_LIBDIR ${CMAKE_CURRENT_LIST_DIR}/oidnlib)
set(OIDN_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/oidnlib/include)
set(OPENIMAGEDENOISE_LIBRARIES
    optimized ${OIDN_LIBDIR}/lib/OpenImageDenoise.lib
    debug ${OIDN_LIBDIR}/lib/OpenImageDenoise_d.lib
    )
find_package(OpenImageDenoise REQUIRED)


#project(OpenImageDenoise)
#add_library(OpenImageDenoise SHARED IMPORTED GLOBAL)
#set(OIDN_LIBRARIES "OpenImageDenoise")
#set(OIDN_LIBDIR oidnlib)
#target_include_directories(OpenImageDenoise INTERFACE ${OIDN_LIBDIR}/include/)
#set_target_properties(OpenImageDenoise PROPERTIES
#    IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
#    IMPORTED_LOCATION "${OIDN_LIBDIR}/bin/OpenImageDenoise.dll"
#    IMPORTED_IMPLIB "${OIDN_LIBDIR}/lib/OpenImageDenoise.lib"
#    INTERFACE_INCLUDE_DIRECTORIES "${OIDN_LIBDIR}/include"
#)
#add_library(OpenImageDenoise::OpenImageDenoise ALIAS OpenImageDenoise)



#file(GLOB_RECURSE OIDN_HEADERS
#    ${CMAKE_CURRENT_LIST_DIR}/oidnlib/*.h
#    ${CMAKE_CURRENT_LIST_DIR}/oidnlib/*.hpp
#)
#target_link_directories(OpenImageDenoise INTERFACE ${OIDN_LIBDIR}/lib)


#set(TBB_ROOT C:/Code/ToyGPURenderer/third-party/tbb)
#add_subdirectory(oidn)

#project(oidn)



# add_executable(example_glfw_vulkan ${sources} ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/imgui_draw.cpp ${IMGUI_DIR}/imgui_demo.cpp ${IMGUI_DIR}/imgui_tables.cpp ${IMGUI_DIR}/imgui_widgets.cpp)
# target_link_libraries(example_glfw_vulkan ${LIBRARIES})
