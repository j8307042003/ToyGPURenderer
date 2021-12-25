cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)


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