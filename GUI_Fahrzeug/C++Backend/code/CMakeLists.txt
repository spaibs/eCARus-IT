cmake_minimum_required(VERSION 2.8)

project(eCARusApps) ## BITTE KEINE LEERZEICHEN IM PROJEKTNAMEN!
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(${PROJECT_SOURCE_DIR}/cmake/utility_macros.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/platform.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/default_flags.cmake)


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

## HIER ALLE APPLIKATIONS-UNTERVERZEICHNISSE ANGEBEN!
  add_subdirectory("MusicServiceAccess")
  add_subdirectory("NavigationServiceAccess")
  add_subdirectory("TowerAccess")
