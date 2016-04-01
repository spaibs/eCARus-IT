project("MusicServiceAccess")

STRING(REGEX REPLACE "_" "\ " MODEL_NAME ${PROJECT_NAME})
file(GLOB_RECURSE ${PROJECT_NAME}_src
                ${PROJECT_SOURCE_DIR}/src/*.h
                ${PROJECT_SOURCE_DIR}/src/*.cpp)

if (NOT BUILD_STATIC_LINKED_APPLICATION)

   add_library(${PROJECT_NAME} SHARED
            ${${PROJECT_NAME}_src})

   target_link_libraries(${PROJECT_NAME}
                      GtfCommon)

   SET_TARGET_PROPERTIES(${PROJECT_NAME}
                      PROPERTIES OUTPUT_NAME MusicServiceAccess)

   install(TARGETS ${PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_PREFIX})
else()

   add_library(${PROJECT_NAME} STATIC
            ${${PROJECT_NAME}_src})
endif()
