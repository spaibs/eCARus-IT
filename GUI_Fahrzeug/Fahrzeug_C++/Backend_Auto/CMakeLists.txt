cmake_minimum_required(VERSION 2.8)

project(GTF_SDK_Examples)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(${PROJECT_SOURCE_DIR}/cmake/utility_macros.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/platform.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/default_flags.cmake)

if(EB_PLATFORM_OS_QNX)
    set(GTF_TARGET_SDK_ROOT "${PROJECT_SOURCE_DIR}/../../platform/qnx_6.5_armv7" CACHE PATH "Folder containing the GTF target SDK (e.g. ../../platform/qnx_6.5_armv7)")
elseif(EB_PLATFORM_OS_LINUX)
    set(GTF_TARGET_SDK_ROOT "${PROJECT_SOURCE_DIR}/../../platform/linux_2.4_armv7" CACHE PATH "Folder containing the GTF target SDK (e.g. ../../platform/linux_2.4_armv7 or ../../platform/linux_2.2.3_i686)")
else()
    set(GTF_TARGET_SDK_ROOT "${PROJECT_SOURCE_DIR}/../../platform/win32" CACHE PATH "Folder containing the GTF target SDK (e.g. ../../platform/win32)")
endif()

include_directories(${GTF_TARGET_SDK_ROOT}/include)

option(BUILD_STATIC_LINKED_APPLICATION "Build stand alone application that links GTF run-time statically" OFF)


if (NOT BUILD_STATIC_LINKED_APPLICATION)
    include(${GTF_TARGET_SDK_ROOT}/lib/sdklibs-targets.cmake)
else()
    #Check if static build is supported by the SDK
    if(EXISTS "${PROJECT_SOURCE_DIR}/static_build" AND IS_DIRECTORY "${PROJECT_SOURCE_DIR}/static_build")
       message(STATUS "Building static applications")
       add_subdirectory("static_build")
       add_definitions("-DUSE_STATIC_PLUGINS")
    else()
       message(WARNING "Static library build is not supported in this SDK" )
    endif()
endif()
  add_subdirectory("code/TowerAccess")
  add_subdirectory("code/NavigationServiceAccess")
