INCLUDE(MacroAddFileDependencies)
INCLUDE(CMakeDependentOption)

## Like LISPs CAR
# copies the first element of the list passed to ${var}
MACRO(CAR var)
    SET(${var} ${ARGV1})
ENDMACRO(CAR)

## Like LISPs CDR
# reduces the lisp by the first entry
# new list is written to ${var}
MACRO(CDR var junk)
    SET(${var} ${ARGN})
ENDMACRO(CDR)

# checks if the list contains \a value
# result is written to var
MACRO(LIST_CONTAINS var value)
    SET(${var})
    FOREACH (value2 ${ARGN})
        IF (${value} STREQUAL ${value2})
            SET(${var} TRUE)
        ENDIF (${value} STREQUAL ${value2})
    ENDFOREACH (value2)
ENDMACRO(LIST_CONTAINS)

# parsers macro arguments
# The PARSE_ARGUMENTS macro will take the arguments of another macro and define
# several variables. The first argument to PARSE_ARGUMENTS is a prefix to put
# on all variables it creates. The second argument is a list of names, and the
# third argument is a list of options. Both of these lists should be quoted.
# The rest of PARSE_ARGUMENTS are arguments from another macro to be parsed.
#
#   PARSE_ARGUMENTS(prefix arg_names options arg1 arg2...)
#
# For each item in options, PARSE_ARGUMENTS will create a variable with that name,
# prefixed with prefix_. So, for example, if prefix is MY_MACRO and options is
# OPTION1;OPTION2, then PARSE_ARGUMENTS will create the variables MY_MACRO_OPTION1
# and MY_MACRO_OPTION2. These variables will be set to true if the option
# exists in the command line or false otherwise.
#
# For each item in arg_names, PARSE_ARGUMENTS will create a variable with that
# name, prefixed with prefix_. Each variable will be filled with the arguments
# that occur after the given arg_name is encountered up to the next arg_name or
# the end of the arguments. All options are removed from these lists.
# PARSE_ARGUMENTS also creates a prefix_DEFAULT_ARGS variable containing the
# list of all arguments up to the first arg_name encountered.
#
# Example:
# Let us say that we are creating an application that accepts plugins with
# components, and we need a CMake macro that makes it easy to compile and link a
# plugin. The plugin will have some code in it, and it will have to have some
# sort of factory built in that allows the introspection and creation of components
# in the plugin. We also may want to embed some scripts in the plugin. The plugin
# may depend on other libraries and plugins. Finally, we may have some options
# to determine how the plugin may be compiled or used. We will create a macro
# command that can be called as follows.

# ADD_PLUGIN(name [AUTO_INSTALL] [NO_MODULE] source1 source2 ...
#    EXPORTS object1 object2 ...
#    [AUTOLOAD_SCRIPTS script1 script2 ...]
#    [LINK_LIBRARIES lib1 lib2 ...]
#    [DEPENDS plugin1 plugin2 ...]
#   )
#
# Although the argument usage is easy for a user to understand (especially one
# familiar with using other CMake commands), but it can be a hassle for a macro
# to parse. Of course, PARSE_ARGUMENTS will do it for us.
#
# Here is a vacuous implementation of ADD_PLUGIN. It uses PARSE_ARGUMENTS to parse
# the argument specification above, and then simply prints out each argument list
# and option. Note that this example uses the CAR and CDR macros.

# MACRO(ADD_PLUGIN)
#     PARSE_ARGUMENTS(PLUGIN
#         "EXPORTS;AUTOLOAD_SCRIPTS;LINK_LIBRARIES;DEPENDS"
#         "AUTO_INSTALL;NO_MODULE"
#         ${ARGN}
#         )
#     CAR(PLUGIN_NAME ${PLUGIN_DEFAULT_ARGS})
#     CDR(PLUGIN_SOURCES ${PLUGIN_DEFAULT_ARGS})
#
#     MESSAGE("*** Arguments for ${PLUGIN_NAME}")
#     MESSAGE("Sources: ${PLUGIN_SOURCES}")
#     MESSAGE("Exports: ${PLUGIN_EXPORTS}")
#     MESSAGE("Autoload scripts: ${PLUGIN_AUTOLOAD_SCRIPTS}")
#     MESSAGE("Link libraries: ${PLUGIN_LINK_LIBRARIES}")
#     MESSAGE("Depends: ${PLUGIN_DEPENDS}")
#     IF (PLUGIN_AUTO_INSTALL)
#         MESSAGE("Auto install")
#     ENDIF (PLUGIN_AUTO_INSTALL)
#     IF (PLUGIN_NO_MODULE)
#         MESSAGE("No module")
#     ENDIF (PLUGIN_NO_MODULE)
# ENDMACRO(ADD_PLUGIN)
#
# Here are some examples of using this macro so you can see how PARSE_ARGUMENTS breaks up the arguments.

MACRO(PARSE_ARGUMENTS prefix arg_names option_names)
    SET(DEFAULT_ARGS)
    FOREACH(arg_name ${arg_names})
        SET(${prefix}_${arg_name})
    ENDFOREACH(arg_name)
    FOREACH(option ${option_names})
        SET(${prefix}_${option} FALSE)
    ENDFOREACH(option)

    SET(current_arg_name DEFAULT_ARGS)
    SET(current_arg_list)
    FOREACH(arg ${ARGN})
        LIST_CONTAINS(is_arg_name ${arg} ${arg_names})
        IF (is_arg_name)
            SET(${prefix}_${current_arg_name} ${current_arg_list})
            SET(current_arg_name ${arg})
            SET(current_arg_list)
        ELSE (is_arg_name)
            LIST_CONTAINS(is_option ${arg} ${option_names})
            IF (is_option)
                SET(${prefix}_${arg} TRUE)
            ELSE (is_option)
                SET(current_arg_list ${current_arg_list} ${arg})
            ENDIF (is_option)
        ENDIF (is_arg_name)
    ENDFOREACH(arg)
    SET(${prefix}_${current_arg_name} ${current_arg_list})
ENDMACRO(PARSE_ARGUMENTS)


###############################
# COLIBRI_COPY_LIBS copies libraries from the given
# Project to the specified target project directory. This
# also includes debug symbols, and dll/so handling.
#
# The commands are attached to the library targets.
MACRO(COLIBRI_COPY_LIBS SRC_PROJECT DEST_PROJECT LIBNAMES)
    FOREACH(COLIBRI_LIB_TESTSETUP_LIBNAME ${LIBNAMES})
    IF(WIN32)
        ADD_CUSTOM_COMMAND(TARGET ${COLIBRI_LIB_TESTSETUP_LIBNAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${${SRC_PROJECT}_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${COLIBRI_LIB_TESTSETUP_LIBNAME}.dll
            ${${DEST_PROJECT}_BINARY_DIR}
            DEPENDS ${SRC_PROJECT})
        IF("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" OR "${CMAKE_BUILD_TYPE}" STREQUAL
            "RelWithDebInfo")
            STRING(TOLOWER ${COLIBRI_LIB_TESTSETUP_LIBNAME}
                   COLIBRI_LIB_TESTSETUP_LIBNAME_LOWER)
            ADD_CUSTOM_COMMAND(TARGET ${COLIBRI_LIB_TESTSETUP_LIBNAME}
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                ${${SRC_PROJECT}_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${COLIBRI_LIB_TESTSETUP_LIBNAME_LOWER}.pdb
                ${${DEST_PROJECT}_BINARY_DIR}
                DEPENDS ${SRC_PROJECT})
        ENDIF("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" OR "${CMAKE_BUILD_TYPE}" STREQUAL
            "RelWithDebInfo")

     ENDIF(WIN32)
     IF(UNIX)
        ADD_CUSTOM_COMMAND(TARGET ${COLIBRI_LIB_TESTSETUP_LIBNAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS -E copy ${${SRC_PROJECT}_BINARY_DIR}/${CMAKE_CFG_INTDIR}/lib${COLIBRI_LIB_TESTSETUP_LIBNAME}.so
            ${${DEST_PROJECT}_BINARY_DIR}
            DEPENDS ${SRC_PROJECT})
     ENDIF(UNIX)
     ENDFOREACH(COLIBRI_LIB_TESTSETUP_LIBNAME ${LIBNAMES})
ENDMACRO(COLIBRI_COPY_LIBS TARGET SRC_PROJECT LIBNAMES)

MACRO(PREPARE_FOR_TESTING)
    ENABLE_TESTING()
ENDMACRO(PREPARE_FOR_TESTING)

MACRO(COPY_HMI_LIBS_FOR_TEST SRC_PROJECT)
    IF(NOT ${PROJECT_NAME}_CopyLibraries_Exist)
        SET(${PROJECT_NAME}_CopyLibraries_Exist ON)
        ADD_CUSTOM_TARGET(${PROJECT_NAME}_CopyLibraries ALL)
    ENDIF(NOT ${PROJECT_NAME}_CopyLibraries_Exist)
    FOREACH(COLIBRI_LIB_TESTSETUP_LIBNAME ${ARGN})
        GET_FILENAME_COMPONENT(__COPY_SOURCE_PATH ${${SRC_PROJECT}_BINARY_DIR}/${CMAKE_CFG_INTDIR}/ ABSOLUTE)
        GET_FILENAME_COMPONENT(__COPY_DEST_PATH ${PROJECT_BINARY_DIR} ABSOLUTE)
        STRING(TOLOWER ${COLIBRI_LIB_TESTSETUP_LIBNAME}
            COLIBRI_LIB_TESTSETUP_LIBNAME_LOWER)
        IF(__COPY_SOURCE_PATH STREQUAL __COPY_DEST_PATH)
        ELSE(__COPY_SOURCE_PATH STREQUAL __COPY_DEST_PATH)
           IF(MSVC AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" OR "${CMAKE_BUILD_TYPE}" STREQUAL
               "RelWithDebInfo")
               ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}_CopyLibraries
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                       ${__COPY_SOURCE_PATH}/${CMAKE_SHARED_LIBRARY_PREFIX}${COLIBRI_LIB_TESTSETUP_LIBNAME}${CMAKE_SHARED_LIBRARY_SUFFIX}
                       ${__COPY_DEST_PATH}
                   COMMAND ${CMAKE_COMMAND} -E copy
                       ${__COPY_SOURCE_PATH}/${COLIBRI_LIB_TESTSETUP_LIBNAME_LOWER}.pdb
                       ${__COPY_DEST_PATH}
                   DEPENDS ${COLIBRI_LIB_TESTSETUP_LIBNAME}
                       ${__COPY_SOURCE_PATH}/${CMAKE_SHARED_LIBRARY_PREFIX}${COLIBRI_LIB_TESTSETUP_LIBNAME}${CMAKE_SHARED_LIBRARY_SUFFIX}
                       ${__COPY_SOURCE_PATH}/${COLIBRI_LIB_TESTSETUP_LIBNAME_LOWER}.pdb
                   )
           ELSE(MSVC AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" OR "${CMAKE_BUILD_TYPE}" STREQUAL
               "RelWithDebInfo")
               ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}_CopyLibraries
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                       ${__COPY_SOURCE_PATH}/${CMAKE_SHARED_LIBRARY_PREFIX}${COLIBRI_LIB_TESTSETUP_LIBNAME}${CMAKE_SHARED_LIBRARY_SUFFIX}
                       ${__COPY_DEST_PATH}
                   DEPENDS ${COLIBRI_LIB_TESTSETUP_LIBNAME}
                       ${__COPY_SOURCE_PATH}/${CMAKE_SHARED_LIBRARY_PREFIX}${COLIBRI_LIB_TESTSETUP_LIBNAME}${CMAKE_SHARED_LIBRARY_SUFFIX}
                   )
           ENDIF(MSVC AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" OR "${CMAKE_BUILD_TYPE}" STREQUAL
               "RelWithDebInfo")
        ENDIF(__COPY_SOURCE_PATH STREQUAL __COPY_DEST_PATH)
     ENDFOREACH(COLIBRI_LIB_TESTSETUP_LIBNAME ${ARGN})
ENDMACRO(COPY_HMI_LIBS_FOR_TEST SRC_PROJECT)


###############################################################
# This will add arguments not found in ${parameter} to the end.  It
# does not attempt to remove duplicate arguments already existing in
# ${parameter}.
MACRO(FORCE_ADD_FLAGS parameter)
  # Create a separated list of the arguments to loop over
  SET(p_list ${${parameter}})
  SEPARATE_ARGUMENTS(p_list)
  # Make a copy of the current arguments in ${parameter}
  SET(new_parameter ${${parameter}})
  # Now loop over each required argument and see if it is in our
  # current list of arguments.
  FOREACH(required_arg ${ARGN})
    # This helps when we get arguments to the function that are
    # grouped as a string:
    #
    # ["-O3 -g"]  instead of [-O3 -g]
    SET(TMP ${required_arg}) #elsewise the Seperate command doesn't work)
    SEPARATE_ARGUMENTS(TMP)
    FOREACH(option ${TMP})
      # Look for the required argument in our list of existing arguments
      SET(found FALSE)
      FOREACH(p_arg ${p_list})
        IF (${p_arg} STREQUAL ${option})
          SET(found TRUE)
        ENDIF (${p_arg} STREQUAL ${option})
      ENDFOREACH(p_arg)
      IF(NOT found)
        # The required argument wasn't found, so we need to add it in.
        SET(new_parameter "${new_parameter} ${option}")
      ENDIF(NOT found)
    ENDFOREACH(option ${TMP})
  ENDFOREACH(required_arg ${ARGN})
  SET(${parameter} ${new_parameter} CACHE STRING "" FORCE)
ENDMACRO(FORCE_ADD_FLAGS)

###############################################################
# This will remove arguments not found in ${parameter} to the end.  It
# does not attempt to remove duplicate arguments already existing in
# ${parameter}.
MACRO(FORCE_REMOVE_FLAGS parameter)
  # Create a separated list of the arguments to loop over
  SET(p_list ${${parameter}})
  SEPARATE_ARGUMENTS(p_list)
  SET(new_parameter "")

  FOREACH(p_arg ${p_list})
    SET(found FALSE)
    FOREACH(rem_arg ${ARGN})
      # separate the arguments
      # then we support "arg1 arg2" and arg1 arg2 likewise
      SET(TMP ${rem_arg})
      SEPARATE_ARGUMENTS(TMP)


      FOREACH(option ${TMP})
        IF (${p_arg} STREQUAL ${option})
          SET(found TRUE)
        ENDIF (${p_arg} STREQUAL ${option})
      ENDFOREACH(option ${TMP})

    ENDFOREACH(rem_arg ${ARGN})
    IF(NOT found)
        SET(new_parameter "${new_parameter} ${p_arg}")
    ENDIF(NOT found)
  ENDFOREACH(p_arg ${p_list})
  SET(${parameter} ${new_parameter} CACHE STRING "" FORCE)
ENDMACRO(FORCE_REMOVE_FLAGS)

#########################################################
#
# BULK_FORCE_ADD_FLAGS( VARIABLES VAR1 [VAR2 ...]
#                       FLAGS FLAG1 [FLAG2 ...]
#                     )
# Force adding multiple flags to multiple variables at once
#
MACRO(BULK_FORCE_ADD_FLAGS)
    PARSE_ARGUMENTS(PARAMS_EXTENDED_FORCE_ADD_FLAGS
            "VARIABLES;FLAGS"
            ""
            ${ARGN}
            )
   FOREACH(flag ${PARAMS_EXTENDED_FORCE_ADD_FLAGS_VARIABLES})
      FORCE_ADD_FLAGS(${flag} ${PARAMS_EXTENDED_FORCE_ADD_FLAGS_FLAGS} )
   ENDFOREACH(flag ${PARAMS_EXTENDED_FORCE_ADD_FLAGS_VARIABLES})
ENDMACRO(BULK_FORCE_ADD_FLAGS)

#########################################################
#
# BULK_FORCE_REMOVE_FLAGS( VARIABLES VAR1 [VAR2 ...]
#                          FLAGS FLAG1 [FLAG2 ...]
#                        )
# Force adding multiple flags to multiple variables at once
#

MACRO(BULK_FORCE_REMOVE_FLAGS)
    PARSE_ARGUMENTS(PARAMS_EXTENDED_FORCE_REMOVE_FLAGS
            "VARIABLES;FLAGS"
            ""
            ${ARGN}
            )
   FOREACH(flag ${PARAMS_EXTENDED_FORCE_REMOVE_FLAGS_VARIABLES})
      FORCE_REMOVE_FLAGS(${flag} ${PARAMS_EXTENDED_FORCE_REMOVE_FLAGS_FLAGS} )
   ENDFOREACH(flag ${PARAMS_EXTENDED_FORCE_REMOVE_FLAGS_VARIABLES})
ENDMACRO(BULK_FORCE_REMOVE_FLAGS)
###################################################################
# Install binaries commands
#
# INSTALL_EXE(TARGETS)
# INSTALL_LIB(TARGETS)
#
# In debug builds these commands will also copy the debug symbols
#
MACRO(INSTALL_EXE)
    INSTALL(TARGETS ${ARGN}
        RUNTIME DESTINATION ${PROJECT_NAME}/bin/${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE}
    )
ENDMACRO(INSTALL_EXE)

MACRO(INSTALL_LIBS)
    INSTALL(TARGETS ${ARGN}
        RUNTIME DESTINATION ${PROJECT_NAME}/lib/${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE}
        LIBRARY DESTINATION ${PROJECT_NAME}/lib/${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE}
        ARCHIVE DESTINATION ${PROJECT_NAME}/lib/${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE}
        )
ENDMACRO(INSTALL_LIBS)


###################################################################
# CREATE_SOURCE_GROUP(
#           [FOLDERS                 FOLDER1 [FOLDER2 ...]]
#           [SOURCES                 FILE1 [FILE2 ...]]
#           )
#
# This macro takes a list of source files as input and generates source
# groups that follow the shortest relative path of the source files from the
# source folders. This is strictly a cosmetic change, but it's nice if you're
# using the CMake generated projects also for development
#
MACRO ( CREATE_SOURCE_GROUP )
    PARSE_ARGUMENTS(PARAMS_VS_FILTER
            "FOLDERS;SOURCES"
            ""
            ${ARGN}
            )
    FOREACH(SOURCE_FILE ${PARAMS_VS_FILTER_SOURCES})
        get_filename_component(SOURCE_FILE_ABSOLUTE "${SOURCE_FILE}" ABSOLUTE)
        get_filename_component(PARENT_FOLDER_ABSOLUTE "${SOURCE_FILE_ABSOLUTE}" PATH)
        set(FOLDER_RELATIVE "${PARENT_FOLDER_ABSOLUTE}")
        FOREACH(SOURCE_FOLDER ${PARAMS_VS_FILTER_FOLDERS})
            if(IS_ABSOLUTE "${FOLDER_RELATIVE}")
                file(RELATIVE_PATH FOLDER_RELATIVE "${SOURCE_FOLDER}" "${PARENT_FOLDER_ABSOLUTE}")
            else()
                file(RELATIVE_PATH TMP_FOLDER_RELATIVE "${SOURCE_FOLDER}" "${PARENT_FOLDER_ABSOLUTE}")
                string(LENGTH "${TMP_FOLDER_RELATIVE}" TMP_FOLDER_RELATIVE_LENGTH)
                string(LENGTH "${FOLDER_RELATIVE}" FOLDER_RELATIVE_LENGTH)
                if (TMP_FOLDER_RELATIVE_LENGTH LESS FOLDER_RELATIVE_LENGTH)
                    set(FOLDER_RELATIVE "${TMP_FOLDER_RELATIVE}")
                endif()
            endif()
        ENDFOREACH()
        STRING ( REGEX REPLACE "[\\\\/][^\\\\/]*$" "" FOLDER_RELATIVE "${FOLDER_RELATIVE}" )
        STRING ( REGEX REPLACE "^[\\\\/]" "" FOLDER_RELATIVE "${FOLDER_RELATIVE}" )
        STRING ( REGEX REPLACE "/" "\\\\\\\\" FOLDER_RELATIVE "${FOLDER_RELATIVE}" )
        SOURCE_GROUP ( "${FOLDER_RELATIVE}" FILES ${SOURCE_FILE_ABSOLUTE} )
    ENDFOREACH()
ENDMACRO ( CREATE_SOURCE_GROUP )

###################################################################
# Create Final File
#
macro(CREATE_FINAL_FILE ProjectDir _filename)
    file(WRITE ${PROJECT_BINARY_DIR}/${_filename}_temp "//autogenerated file\n")
    foreach(_current_FILE ${ARGN})
        if(IS_DIRECTORY ${_current_FILE})
            message(WARNING "The entry ${_current_FILE} is a directoy in project ${PROJECT_NAME}")
        else()
            get_filename_component(_current_FILE_EXT ${_current_FILE} EXT)
            string(REPLACE "." "" _current_FILE_EXT ${_current_FILE_EXT})
            list(FIND CMAKE_CXX_SOURCE_FILE_EXTENSIONS ${_current_FILE_EXT} EXT_INDEX)
            if(EXT_INDEX GREATER -1)
                set(RelativePath ${_current_FILE})
                if(NOT IS_ABSOLUTE ${RelativePath})
                   set(RelativePath ${PROJECT_SOURCE_DIR}/${_current_FILE})
                endif(NOT IS_ABSOLUTE ${RelativePath})
                file(RELATIVE_PATH RelativePath ${PROJECT_BINARY_DIR} ${RelativePath})
                file(APPEND ${PROJECT_BINARY_DIR}/${_filename}_temp "#include \"${RelativePath}\"\n")
                set_source_files_properties(${_current_FILE} PROPERTIES HEADER_FILE_ONLY ON)
            endif(EXT_INDEX GREATER -1)
        endif()
    endforeach(_current_FILE)
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PROJECT_BINARY_DIR}/${_filename}_temp ${PROJECT_BINARY_DIR}/${_filename})
endmacro(CREATE_FINAL_FILE)

option(EBFASTBUILD_GLOBAL_ON OFF "Force fast build creation globally, overriding any local settings")

###############################################################################
# Turn a list into a string
#
function(eb_join_list _output _list)
    set(tmp_string "")
    foreach(_item ${_list})
        string(LENGTH "${tmp_string}" _len)
        if(${_len} GREATER 0)
          set(tmp_string "${tmp_string} ${_item}")
        else()
          set(tmp_string "${_item}")
        endif()
    endforeach()
    set (${_output} "${tmp_string}" PARENT_SCOPE)
endfunction()

###################################################################
#  eb_add_compile_flags(
#           [TARGETS                 TARGET1 [TARGET2 ...]]
#           [SOURCES                 FILE1 [FILE2 ...]]
#           [FLAGS                   FLAG1 [FLAG2 ...]]
#           )
#
#  Adds flags to the compile flag property
#  of targets or source files.
#
#  TARGET an optional list of targets.
#  SOURCE an optional list of source files.
#  FLAGS a list of flags.
#
macro(eb_add_compile_flags)
    PARSE_ARGUMENTS(EB_ADD_FLAGS_PARAMETER
         "TARGETS;SOURCES;FLAGS"
         ""
         ${ARGN}
    )
    if(EB_ADD_FLAGS_PARAMETER_TARGETS)
        separate_arguments(EB_ADD_FLAGS_PARAMETER_TARGETS)
        list(REMOVE_DUPLICATES EB_ADD_FLAGS_PARAMETER_TARGETS)
    endif()
    if(EB_ADD_FLAGS_PARAMETER_SOURCES)
        separate_arguments(EB_ADD_FLAGS_PARAMETER_SOURCES)
        eb_to_absolute_paths(EB_ADD_FLAGS_PARAMETER_SOURCES)
        list(REMOVE_DUPLICATES EB_ADD_FLAGS_PARAMETER_SOURCES)
    endif()
    if(EB_ADD_FLAGS_PARAMETER_FLAGS)
        separate_arguments(EB_ADD_FLAGS_PARAMETER_FLAGS)
        list(REMOVE_DUPLICATES EB_ADD_FLAGS_PARAMETER_FLAGS)
        foreach(CURRENT_SCOPE "TARGET" "SOURCE")
            foreach(CURRENT_TARGET ${EB_ADD_FLAGS_PARAMETER_${CURRENT_SCOPE}S})
                get_property(
                        TMP_FLAGS
                    ${CURRENT_SCOPE}
                        ${CURRENT_TARGET}
                    PROPERTY COMPILE_FLAGS
                )
                separate_arguments(TMP_FLAGS)
                list(APPEND TMP_FLAGS ${EB_ADD_FLAGS_PARAMETER_FLAGS})
                list(REMOVE_DUPLICATES TMP_FLAGS)
                eb_join_list(TMP_FLAGS "${TMP_FLAGS}")
                # MESSAGE("setting compile flags of ${CURRENT_SCOPE} ${CURRENT_TARGET} to ${TMP_FLAGS}")
                set_property(
                    ${CURRENT_SCOPE}
                        ${CURRENT_TARGET}
                    PROPERTY COMPILE_FLAGS
                        ${TMP_FLAGS}
                )
            endforeach()
        endforeach()
    endif()
endmacro()

###################################################################
#  eb_remove_compile_flags(
#           [TARGETS                 TARGET1 [TARGET2 ...]]
#           [SOURCES                 FILE1 [FILE2 ...]]
#           [FLAGS                   FLAG1 [FLAG2 ...]]
#           )
#
#  Remove flags from compile flag property
#  of targets or source files.
#
#  TARGET an optional list of targets.
#  SOURCE an optional list of source files.
#  FLAGS a list of flags.
#
macro(eb_remove_compile_flags)
    PARSE_ARGUMENTS(EB_ADD_FLAGS_PARAMETER
         "TARGETS;SOURCES;FLAGS"
         ""
         ${ARGN}
    )
    if(EB_ADD_FLAGS_PARAMETER_TARGETS)
        separate_arguments(EB_ADD_FLAGS_PARAMETER_TARGETS)
        list(REMOVE_DUPLICATES EB_ADD_FLAGS_PARAMETER_TARGETS)
    endif()
    if(EB_ADD_FLAGS_PARAMETER_SOURCES)
        separate_arguments(EB_ADD_FLAGS_PARAMETER_SOURCES)
        eb_to_absolute_paths(EB_ADD_FLAGS_PARAMETER_SOURCES)
        list(REMOVE_DUPLICATES EB_ADD_FLAGS_PARAMETER_SOURCES)
    endif()
    if(EB_ADD_FLAGS_PARAMETER_FLAGS)
        separate_arguments(EB_ADD_FLAGS_PARAMETER_FLAGS)
        list(REMOVE_DUPLICATES EB_ADD_FLAGS_PARAMETER_FLAGS)
        foreach(CURRENT_SCOPE "TARGET" "SOURCE")
            foreach(CURRENT_TARGET ${EB_ADD_FLAGS_PARAMETER_${CURRENT_SCOPE}S})
                get_property(
                        TMP_FLAGS
                    ${CURRENT_SCOPE}
                        ${CURRENT_TARGET}
                    PROPERTY COMPILE_FLAGS
                )
                separate_arguments(TMP_FLAGS)
                foreach(REMOVE_FLAG ${EB_ADD_FLAGS_PARAMETER_FLAGS})
                    list(REMOVE_ITEM TMP_FLAGS ${REMOVE_FLAG})
                endforeach()
                eb_join_list(TMP_FLAGS "${TMP_FLAGS}")
                # MESSAGE("setting compile flags of ${CURRENT_SCOPE} ${CURRENT_TARGET} to ${TMP_FLAGS}")
                set_property(
                    ${CURRENT_SCOPE}
                        ${CURRENT_TARGET}
                    PROPERTY COMPILE_FLAGS
                        ${TMP_FLAGS}
                )
            endforeach()
        endforeach()
    endif()
endmacro()

###################################################################
#
#  eb_add_library(name
#           [STATIC | SHARED | MODULE] [EXCLUDE_FROM_ALL]
#           [INCLUDE_PATH_VARS       VAR1 [VAR2 ...]]
#           [INCLUDE_PATHS           PATH1 [PATH2 ...]]
#           [SOURCES                 FILE1 [FILE2 ...]]
#           [NO_FAST_BUILD_SOURCES   FILE1 [FILE2 ...]]
#           [LIB_VARS                VAR1 [VAR2...]]
#           [LIB_PATHS_VARS          VAR1 [VAR2...]]
#           [LIBS                    LIB1 [LIB2...]]
#           [DEFINITIONS             DEFINE1 [DEFINE2 ...]]
#           [FLAGS                   FLAG1 [FLAG2 ...]]
#           )
#
#  Adds a static or shared library. This is a wrapper around add_library
#  defined for EB internal use.
#
#  INCLUDE_PATH_VARS are a list of variable names, that contain
#  include directories of the dependencies of this lib. Variable
#  names are expected to be able to track missing dependencies.
#
#  INCLUDE_PATHS a set of include paths required by this project.
#  use this for include paths not defined in variables.
#
#  SOURCES a list of source files.
#
#  NO_FAST_BUILD_SOURCES a list of source files that cannot be built
#  using fast build. I.e. because they define static symbols that could
#  collide, or if they require special preprocessor settings.
#
#  LIB_VARS are a list of variable names, that contain library dependencies
#  of this lib. Variable names are expected to be able to track missing
#  dependencies.
#
#  LIB_PATH_VARS is the counterpart of INCLUDE_PATH_VARS. It just expects
#  library directories that contain libraries this target depends on.
#  Again here variable names are expected to be able to detect missing
#  dependencies. Note this feature will be dropped as soon as we switch
#  to cmake-2.6.4 or later
#
#  LIBS is like LIB_VARS, but expects targets instead of variable names.
#
#  DEFINITIONS is a set of definitions passed to add_definitions()
#
#  FLAGS is a set of flags passed to eb_add_compile_flags()
#
macro(eb_add_library name)
    PARSE_ARGUMENTS(EB_ADD_LIB
         "INCLUDE_PATH_VARS;INCLUDE_PATHS;SOURCES;NO_FAST_BUILD_SOURCES;LIB_VARS;LIB_PATH_VARS;LIBS;DEFINITIONS;FLAGS;CONVENIENCE_TARGETS"
         ""
         ${ARGN}
         )

    if (TOOLCHAIN_USE_GNU_ARCHIVER_FOR_LIBRARIES)
   # message(STATUS "Toolchain uses gnu archiver instead of linker for for creating ${name}")
    else()
    #message(STATUS "Default toolchain behaviour for creating ${name}")
    endif()


    include_directories(${EB_ADD_LIB_INCLUDE_PATHS})
    foreach(inpath ${EB_ADD_LIB_INCLUDE_PATH_VARS})
        if(NOT ${inpath})
            #message(STATUS "${name} needs include path variable ${inpath} defined")
        else(NOT ${inpath})
            include_directories(${${inpath}})
        endif(NOT ${inpath})
    endforeach(inpath)

    foreach(inpath ${EB_ADD_LIB_LIB_PATH_VARS})
        if(NOT ${inpath})
            #message(STATUS "${name} needs library path variable ${inpath} defined")
        else(NOT ${inpath})
            link_directories(${${inpath}})
        endif(NOT ${inpath})
    endforeach(inpath)

    OPTION(EBFASTBUILD_${name} "Enable final all-in-one compilation, for the library ${name}")
    IF(EBFASTBUILD_${name} OR EBFASTBUILD_GLOBAL_ON)
        CREATE_FINAL_FILE(
                    ${PROJECT_SOURCE_DIR}
                    ${name}_final_file.cpp
                    ${EB_ADD_LIB_SOURCES}
                    )
        CREATE_SOURCE_GROUP(
                  FOLDERS
                      ${CMAKE_SOURCE_DIR}
                      ${PROJECT_SOURCE_DIR}
                      ${PROJECT_BINARY_DIR}
                  SOURCES
                      ${EB_ADD_LIB_SOURCES}
                      ${EB_ADD_LIB_NO_FAST_BUILD_SOURCES}
                  )
       add_library(${name} ${EB_ADD_LIB_DEFAULT_ARGS}
                ${EB_ADD_LIB_SOURCES}
                ${EB_ADD_LIB_NO_FAST_BUILD_SOURCES}
                ${PROJECT_BINARY_DIR}/${name}_final_file.cpp
                )
    ELSE()
        CREATE_SOURCE_GROUP(
                  FOLDERS
                      ${CMAKE_SOURCE_DIR}
                      ${PROJECT_SOURCE_DIR}
                      ${PROJECT_BINARY_DIR}
                  SOURCES
                      ${EB_ADD_LIB_SOURCES}
                      ${EB_ADD_LIB_NO_FAST_BUILD_SOURCES}
                )
        add_library(${name} ${EB_ADD_LIB_DEFAULT_ARGS}
                ${EB_ADD_LIB_SOURCES}
                ${EB_ADD_LIB_NO_FAST_BUILD_SOURCES}
                )
    ENDIF()

    set(EB_ADD_LIB_LIB_DEPENDENCIES)
    foreach(inpath ${EB_ADD_LIB_LIB_VARS})
        if(NOT ${inpath})
            #message(STATUS "${name} needs library variable ${inpath} defined")
        else(NOT ${inpath})
            list(APPEND EB_ADD_LIB_LIB_DEPENDENCIES ${${inpath}})
        endif(NOT ${inpath})
    endforeach(inpath)
    ## TODO check if this causes problems
    if(QNX_ENABLE_FUNCTION_PROFILING)
        list(APPEND EB_ADD_LIB_LIBS profilingS)
    endif()
    if(CLANG_ENABLE_ADDRESS_SANITIZER)
        list(APPEND EB_ADD_LIB_LIBS clang_rt.asan-i386)
    endif()

    target_link_libraries(${name} ${EB_ADD_LIB_LIB_DEPENDENCIES} ${EB_ADD_LIB_LIBS})
    add_definitions(${EB_ADD_LIB_DEFINITIONS})
    eb_add_compile_flags(TARGETS ${name} FLAGS ${EB_ADD_LIB_FLAGS})

    if( COMMAND ADD_PC_LINT )
        add_pc_lint(${name} ${EB_ADD_LIB_SOURCES} ${EB_ADD_LIB_NO_FAST_BUILD_SOURCES})
    endif( COMMAND ADD_PC_LINT )

endmacro(eb_add_library)

macro(eb_to_absolute_paths list_name)
    set(__eb_to_absolute_paths_temp)
    foreach(src_file ${${list_name}})
        if(IS_ABSOLUTE ${src_file})
            list(APPEND __eb_to_absolute_paths_temp ${src_file})
        else()
            list(APPEND __eb_to_absolute_paths_temp ${PROJECT_SOURCE_DIR}/${src_file})
        endif()
    endforeach(src_file)

    set(${list_name} ${__eb_to_absolute_paths_temp})
endmacro(eb_to_absolute_paths)

###################################################################
#
#  eb_define_convenience_library(name)
#
#  Defines a new convenience library (e.g. GtfCommon).
#  Some CMAKE variables corresponding to the convenience library
#  are initialized.
#
macro(eb_define_convenience_library name)
    set(GTFCONVENIENCELIB_${name}_PROCESSED OFF CACHE BOOL "" FORCE)
    set(GTFCONVENIENCELIB_${name}_PARTS "" CACHE "" INTERNAL FORCE)
endmacro(eb_define_convenience_library)

###################################################################
#
#  eb_add_convenience_library(name
#           [STATIC | SHARED | MODULE] [EXCLUDE_FROM_ALL]
#           [INCLUDE_PATH_VARS      VAR1 [VAR2 ...]]
#           [INCLUDE_PATHS          PATH1 [PATH2 ...]]
#           [SOURCES                 FILE1 [FILE2 ...]]
#           [NO_FAST_BUILD_SOURCES   FILE1 [FILE2 ...]]
#           [LIB_VARS                VAR1 [VAR2...]]
#           [LIB_PATHS_VARS          VAR1 [VAR2...]]
#           [LIBS                    LIB1 [LIB2...]]
#           [SHARED_DEFINITIONS      DEFINE1 [DEFINE2 ...]]
#           [STATIC_DEFINITIONS      DEFINE1 [DEFINE2 ...]]
#           [FLAGS                   FLAG1 [FLAG2 ...]]
#           )
#
# Add new content to the convenience library. The convenience library
# must be previously defined with eb_define_convenience_library(). The
# convenience library must not yet processed by eb_process_convenience_library()
#
# The meaning of most parameters corresponds to the meaning of parameters
# of eb_add_library().
#
# There are additional parameters:
#
# SHARED_DEFINITIONS is a set of definitions passed to add_definitions()
# when building the convenience library as shared library (e.g. -DGtfOSAL_EXPORTS)
#
# STATIC_DEFINITIONS is a set of definitions passed to add_definitions()
# when building the convenience library as shared library (e.g. -DGtfOSAL_STATIC)
#
# FLAGS is a set of flags passed to eb_add_compile_flags()
#
macro(eb_add_convenience_library name)
    if(GTFCONVENIENCELIB_${name}_PROCESSED)
        MESSAGE( FATAL_ERROR "Adding new contents to convenience library ${name} must not happen after processing." )
    endif()

    PARSE_ARGUMENTS(EB_ADD_LIB
         "INCLUDE_PATH_VARS;INCLUDE_PATHS;SOURCES;NO_FAST_BUILD_SOURCES;LIB_VARS;LIB_PATH_VARS;LIBS;SHARED_DEFINITIONS;STATIC_DEFINITIONS;FLAGS"
         ""
         ${ARGN}
         )

    eb_to_absolute_paths(EB_ADD_LIB_SOURCES)
    eb_to_absolute_paths(EB_ADD_LIB_NO_FAST_BUILD_SOURCES)

    set(convlib_params INCLUDE_PATH_VARS
                       INCLUDE_PATHS
                       SOURCES
                       NO_FAST_BUILD_SOURCES
                       LIB_VARS
                       LIB_PATH_VARS
                       LIBS
                       SHARED_DEFINITIONS
                       STATIC_DEFINITIONS
                       FLAGS)

    foreach(param ${convlib_params})
        set(GTFCONVENIENCELIB_${name}_PART_${PROJECT_NAME}_${param}
            ${EB_ADD_LIB_${param}}
            CACHE "" INTERNAL FORCE)
    endforeach(param)

    LIST_CONTAINS(existing_val ${PROJECT_NAME} ${GTFCONVENIENCELIB_${name}_PARTS})
    if (NOT existing_val)
        list(APPEND GTFCONVENIENCELIB_${name}_PARTS ${PROJECT_NAME})
        set(GTFCONVENIENCELIB_${name}_PARTS ${GTFCONVENIENCELIB_${name}_PARTS} CACHE "" INTERNAL FORCE)
    endif (NOT existing_val)
endmacro(eb_add_convenience_library)

###################################################################
#
#  eb_process_convenience_library(name)
#
#  Process all convenience library contents previously added with
#  eb_add_convenience_library(). Merge all the parameters
#  of all convenience library parts to one set of variables.
#
macro(eb_process_convenience_library name)
    if(GTFCONVENIENCELIB_${name}_PROCESSED)
        MESSAGE( FATAL_ERROR "Error while processing convenience library ${name}. It was already processed." )
    endif()
    set(GTFCONVENIENCELIB_${name}_PROCESSED ON CACHE BOOL "" FORCE)

    set(convlib_params INCLUDE_PATH_VARS
                       INCLUDE_PATHS
                       SOURCES
                       NO_FAST_BUILD_SOURCES
                       LIB_VARS
                       LIB_PATH_VARS
                       LIBS
                       SHARED_DEFINITIONS
                       STATIC_DEFINITIONS
                       FLAGS)

    foreach(param ${convlib_params})
        set(GTFCONVENIENCELIB_${name}_RESULT_${param} "" CACHE "" INTERNAL FORCE)
        foreach(part ${GTFCONVENIENCELIB_${name}_PARTS})
            foreach(val ${GTFCONVENIENCELIB_${name}_PART_${part}_${param}})
                LIST_CONTAINS(existing_val ${val} ${GTFCONVENIENCELIB_${name}_RESULT_${param}})
                if (NOT existing_val)
                    list(APPEND GTFCONVENIENCELIB_${name}_RESULT_${param} ${val})
                endif (NOT existing_val)
            endforeach(val)
        endforeach(part)
        set(GTFCONVENIENCELIB_${name}_RESULT_${param} ${GTFCONVENIENCELIB_${name}_RESULT_${param}} CACHE "" INTERNAL FORCE)
    endforeach(param)
endmacro(eb_process_convenience_library)

###################################################################
#
#  eb_add_executable(name
#           [WIN32] [MACOSX_BUNDLE] [EXCLUDE_FROM_ALL]
#           [INCLUDE_PATH_VARS       VAR1 [VAR2 ...]]
#           [INCLUDE_PATHS           PATH1 [PATH2 ...]]
#           [SOURCES                 FILE1 [FILE2 ...]]
#           [NO_FAST_BUILD_SOURCES   FILE1 [FILE2 ...]]
#           [LIB_VARS                VAR1 [VAR2...]]
#           [LIB_PATH_VARS           VAR1 [VAR2...]]
#           [LIBS                    LIB1 [LIB2...]]
#           [DEFINITIONS             DEFINE1 [DEFINE2 ...]]
#           [FLAGS                   FLAG1 [FLAG2 ...]]
#           )
#
#  Adds an executeable. This is a wrapper around add_executable
#  defined for EB internal use.
#
#  INCLUDE_PATH_VARS are a list of variable names, that contain
#  include directories of the dependencies of this lib. Variable
#  names are expected to be able to track missing dependencies.
#
#  INCLUDE_PATHS a set of include paths required by this project.
#  use this for include paths not defined in variables.
#
#  SOURCES a list of source files.
#
#  NO_FAST_BUILD_SOURCES a list of source files that cannot be built
#  using fast build. I.e. because they define static symbols that could
#  collide, or if they require special preprocessor settings.
#
#  LIB_VARS are a list of variable names, that contain library dependencies
#  of this lib. Variable names are expected to be able to track missing
#  dependencies.
#
#  LIB_PATH_VARS is the counterpart of INCLUDE_PATH_VARS. It just expects
#  library directories that contain libraries this target depends on.
#  Again here variable names are expected to be able to detect missing
#  dependencies. Note this feature will be dropped as soon as we switch
#  to cmake-2.6.4 or later
#
#  LIBS is like LIB_VARS, but expects targets instead of variable names.
#
#  DEFINITIONS is a set of definitions passed to add_definitions()
#
#  FLAGS is a set of flags passed to eb_add_compile_flags()
#
macro(eb_add_executable name)
    PARSE_ARGUMENTS(EB_ADD_PRG
         "INCLUDE_PATH_VARS;INCLUDE_PATHS;LIB_PATH_VARS;LIB_VARS;SOURCES;NO_FAST_BUILD_SOURCES;LIBS;DEFINITIONS;FLAGS"
         ""
         ${ARGN}
         )

    include_directories(${EB_ADD_PRG_INCLUDE_PATHS})
    foreach(inpath ${EB_ADD_PRG_INCLUDE_PATH_VARS})
        if(NOT ${inpath})
            #message(STATUS "${name} needs include path variable ${inpath} defined")
        else(NOT ${inpath})
            include_directories(${${inpath}})
        endif(NOT ${inpath})
    endforeach(inpath)

    foreach(linkpath ${EB_ADD_PRG_LIB_PATH_VARS})
        if(NOT ${linkpath})
            #message(STATUS "${name} needs lib path variable ${linkpath} defined")
        else(NOT ${linkpath})
            link_directories(${${linkpath}})
        endif(NOT ${linkpath})
    endforeach(linkpath)


    OPTION(EBFASTBUILD_${name} "Enable final all-in-one compilation, for executeable ${name}")
    IF(EBFASTBUILD_${name} OR EBFASTBUILD_GLOBAL_ON)
        CREATE_FINAL_FILE(
                    ${PROJECT_SOURCE_DIR}
                    ${name}_final_file.cpp
                    ${EB_ADD_PRG_SOURCES}
                    )
        CREATE_SOURCE_GROUP(
                  FOLDERS
                      ${CMAKE_SOURCE_DIR}
                      ${PROJECT_SOURCE_DIR}
                      ${PROJECT_BINARY_DIR}
                  SOURCES
                      ${EB_ADD_PRG_SOURCES}
                      ${EB_ADD_PRG_NO_FAST_BUILD_SOURCES}
                  )
        add_executable(${name}  ${EB_ADD_PRG_DEFAULT_ARGS}
                    ${EB_ADD_PRG_SOURCES}
                    ${EB_ADD_PRG_NO_FAST_BUILD_SOURCES}
                    ${PROJECT_BINARY_DIR}/${name}_final_file.cpp
                    )
    ELSE()
        CREATE_SOURCE_GROUP(
                  FOLDERS
                      ${CMAKE_SOURCE_DIR}
                      ${PROJECT_SOURCE_DIR}
                      ${PROJECT_BINARY_DIR}
                  SOURCES
                      ${EB_ADD_PRG_SOURCES}
                      ${EB_ADD_PRG_NO_FAST_BUILD_SOURCES}
                  )
        add_executable(${name}  ${EB_ADD_PRG_DEFAULT_ARGS}
                    ${EB_ADD_PRG_SOURCES}
                    ${EB_ADD_PRG_NO_FAST_BUILD_SOURCES}
                    )
    ENDIF()

    add_definitions(${EB_ADD_PRG_DEFINITIONS})
    eb_add_compile_flags(TARGETS ${name} FLAGS ${EB_ADD_PRG_FLAGS})

    if( COMMAND ADD_PC_LINT )
        add_pc_lint(${name} ${EB_ADD_PRG_SOURCES} ${EB_ADD_PRG_NO_FAST_BUILD_SOURCES})
    endif( COMMAND ADD_PC_LINT )
    set(lib_dependencies)
    foreach(lib ${EB_ADD_PRG_LIB_VARS})
        if(NOT ${lib})
            #message(STATUS "${name} needs lib variable ${lib} defined")
        else(NOT ${lib})
            list(APPEND lib_dependencies ${${lib}})
        endif(NOT ${lib})
    endforeach(lib)

    if(QNX_ENABLE_FUNCTION_PROFILING)
        list(APPEND EB_ADD_PRG_LIBS profilingS)
    endif()

    target_link_libraries(${name} ${lib_dependencies} ${EB_ADD_PRG_LIBS})

endmacro(eb_add_executable)

############################################################################
#  eb_add_component(name init_src_file
#           [STATIC | SHARED | MODULE] [EXCLUDE_FROM_ALL]
#           [INCLUDE_PATH_VARS       VAR1 [VAR2 ...]]
#           [INCLUDE_PATHS           PATH1 [PATH2 ...]]
#           [SOURCES                 FILE1 [FILE2 ...]]
#           [NO_FAST_BUILD_SOURCES   FILE1 [FILE2 ...]]
#           [LIB_VARS                VAR1 [VAR2...]]
#           [LIB_PATHS_VARS          VAR1 [VAR2...]]
#           [LIBS                    LIB1 [LIB2...]]
#           [DEFINITIONS             DEFINE1 [DEFINE2 ...]]
#           [FLAGS                   FLAG1 [FLAG2 ...]]
#           )
#
#  Behaves like eb_add_library, but removes the init source file from the library
#  and registers it globally. So that the atf_init_src_file can be used to create
#  a library using the component.
# The other source files will be linked to a lib called ${name}_CORE.
#
# This macro also exports:
#  ${name}_INCLUDE_PATHS "Include directories for ${name}" FORCE)
#
macro(eb_add_component name atf_init_src)
    parse_arguments(eb_atf_LIB
         "INCLUDE_PATH_VARS;INCLUDE_PATHS;SOURCES;NO_FAST_BUILD_SOURCES;LIB_VARS;LIB_PATH_VARS;LIBS;DEFINITIONS;FLAGS"
         ""
         ${ARGN}
         )
    get_filename_component(atf_init_src_abs_path ${atf_init_src} ABSOLUTE)
    if(eb_atf_LIB_SOURCES )
        list(REMOVE_ITEM eb_atf_LIB_SOURCES ${atf_init_src} ${atf_init_src_abs_path})
    endif()
    if(eb_atf_LIB_NO_FAST_BUILD_SOURCES )
        list(REMOVE_ITEM eb_atf_LIB_NO_FAST_BUILD_SOURCES ${atf_init_src_abs_path} ${atf_init_src})
    endif()


   # message(STATUS "Building component ${name}")
    if(eb_atf_LIB_SOURCES OR eb_atf_LIB_NO_FAST_BUILD_SOURCES)
        eb_add_library(${name}_CORE
                ${eb_atf_LIB_DEFAULT_ARGS}
                INCLUDE_PATH_VARS ${eb_atf_LIB_INCLUDE_PATH_VARS}
                INCLUDE_PATHS ${eb_atf_LIB_INCLUDE_PATHS}
                SOURCES ${eb_atf_LIB_SOURCES}
                NO_FAST_BUILD_SOURCES ${eb_atf_LIB_NO_FAST_BUILD_SOURCES}
                LIB_VARS ${eb_atf_LIB_LIB_VARS}
                LIB_PATH_VARS ${eb_atf_LIB_LIB_PATH_VARS}
                LIBS    ${eb_atf_LIB_LIBS}
                DEFINITIONS ${eb_atf_LIB_DEFINITIONS}
                FLAGS ${eb_atf_LIB_FLAGS}
                )
    endif()

    set(${name}_CORE_EXTERNAL_LIBS "" CACHE "" INTERNAL)
    foreach(lib ${eb_atf_LIB_LIBS})
    #   message(STATUS "component ${name}_CORE uses ${lib}")
       LIST(APPEND ${name}_CORE_EXTERNAL_LIBS ${lib})
    endforeach(lib ${eb_atf_LIB_LIBS} list)
    set(${name}_CORE_EXTERNAL_LIBS ${${name}_CORE_EXTERNAL_LIBS} CACHE "" INTERNAL FORCE)


    set(${name}_CORE_LIBS ${eb_atf_LIB_LIBS})
    CREATE_SOURCE_GROUP(
              FOLDERS
                  ${CMAKE_SOURCE_DIR}
                  ${PROJECT_SOURCE_DIR}
                  ${PROJECT_BINARY_DIR}
              SOURCES
                  ${atf_init_src_abs_path}
              )

    set(${name}_INCLUDE_PATHS ${PROJECT_SOURCE_DIR}/include CACHE INTERNAL "Include directories for ${name}" FORCE)
    set(${name}_COMPONENT_INIT_SRC
         ${atf_init_src_abs_path}                           CACHE INTERNAL "Init code for this component" FORCE )

    set(${name}_COMPONENT_INCLUDE_PATH_VARS
        ${eb_atf_LIB_INCLUDE_PATH_VARS}                     CACHE INTERNAL "Include paths for this component" FORCE )

    set(${name}_COMPONENT_INCLUDE_PATHS
        ${eb_atf_LIB_INCLUDE_PATHS}                     CACHE INTERNAL "Include paths for this component" FORCE )
    set(${name}_COMPONENT_LIB_PATH_VARS
        ${eb_atf_LIB_LIB_PATH_VARS}                     CACHE INTERNAL "Linker paths for this component" FORCE )
endmacro(eb_add_component)


macro(eb_set varname)
    set(${varname} ${ARGN} CACHE INTERNAL "" FORCE)
    list(REMOVE_DUPLICATES ${varname})
    set(${varname} ${${varname}} CACHE INTERNAL "" FORCE)
endmacro(eb_set)

###################################################################
#
#  eb_add_headers(name
#           [FILE1 ...] )
#
#  Adds an empty pseudo target, that ensures that header files are visible in
#  IDEs like visual studio.
#
#
macro(eb_add_headers name)
    PARSE_ARGUMENTS(EB_ADD_HEADERS
         ""
         ""
         ${ARGN}
         )

    CREATE_SOURCE_GROUP(
              FOLDERS
                  ${CMAKE_SOURCE_DIR}
                  ${PROJECT_SOURCE_DIR}
                  ${PROJECT_BINARY_DIR}
              SOURCES
                  ${EB_ADD_HEADERS_DEFAULT_ARGS}
              )
    add_custom_target(${name}
        COMMENT "Pseudo Target ${name} to make headers visible"
        SOURCES ${EB_ADD_HEADERS_DEFAULT_ARGS} )
endmacro(eb_add_headers)


#####################################################################
# eb_configure_file(settingprefix headername)
#
# headername must be a relative path,
# the generated header will be created in
#   ${PROJECT_BINARY_DIR}/${headername}
# provided that the input file is stored in
#   ${PROJECT_SOURCE_DIR}/${headername}.in
#
# Add ${PROJECT_BINARY_DIR}/path/to/header to your include paths
macro(eb_configure_file PREFIX HEADER)
    if(IS_ABSOLUTE ${HEADER})
        message(ERROR "[eb_configure_file] Header file must be specified as relative path")
    endif()
    if(NOT EXISTS ${PROJECT_SOURCE_DIR}/${HEADER}.in)
        message(ERROR "[eb_configure_file] Header template file ${PROJECT_SOURCE_DIR}/${HEADER}.in")
    endif()
    option(${PREFIX}_PROFILE "Enables the profiling macros inside ${PROJECT_NAME}" OFF)
    CMAKE_DEPENDENT_OPTION(${PREFIX}_PROFILE_HIGH_LOAD "Enables the profiling macros inside ${PROJECT_NAME}, that will cause a lot of load" ON ${PREFIX}_PROFILE OFF)
    CMAKE_DEPENDENT_OPTION(${PREFIX}_PROFILE_LOW_LOAD "Enables the profiling macros inside ${PROJECT_NAME}, that with less load" ON ${PREFIX}_PROFILE OFF)
    CMAKE_DEPENDENT_OPTION(${PREFIX}_TRACE "Enables the GtfTracer traces inside ${PROJECT_NAME}" ON GTF_TRACER_ENABLED OFF)
    CMAKE_DEPENDENT_OPTION(${PREFIX}_TRACE_ERROR "Enables error traces inside ${PROJECT_NAME}" ON ${PREFIX}_TRACE OFF)
    CMAKE_DEPENDENT_OPTION(${PREFIX}_TRACE_WARN "Enables warning traces inside ${PROJECT_NAME}" ON ${PREFIX}_TRACE OFF)
    CMAKE_DEPENDENT_OPTION(${PREFIX}_TRACE_DEBUG "Enables debugging traces inside ${PROJECT_NAME}" ON ${PREFIX}_TRACE OFF)
    CMAKE_DEPENDENT_OPTION(${PREFIX}_TRACE_INFO "Enables info traces inside ${PROJECT_NAME}" ON ${PREFIX}_TRACE OFF)
    CMAKE_DEPENDENT_OPTION(${PREFIX}_TRACE_SCOPE "Enables scope traces inside ${PROJECT_NAME}" ON ${PREFIX}_TRACE OFF)
    configure_file(${PROJECT_SOURCE_DIR}/${HEADER}.in ${PROJECT_BINARY_DIR}/${HEADER})
endmacro(eb_configure_file)

##################################################################
#
# Doxygen variable(s)
set(DOXYGEN_PATTERNS "${CMAKE_CXX_SOURCE_FILE_EXTENSIONS};h;inl;hpp;hxx;h++")
set(DOXYGEN_SOURCES "" CACHE STRING "Files to include when generating doxygen" FORCE)

macro(eb_doxygen_add_headers)
    PARSE_ARGUMENTS(EB_ADD_DOXY_HDRS
        "FILES;DESTINATION;PROGRAMS;CONFIGURATIONS;COMPONENT;RENAME;RUNTIME;LIBRARY;ARCHIVE;TARGETS"
        "OPTIONAL"
         ${ARGN}
         )
    # UseDoxygen module named it _DIRS but it works for files too
    foreach(_current_FILE ${EB_ADD_DOXY_HDRS_FILES})
        get_filename_component(_current_FILE_EXT ${_current_FILE} EXT)
        get_filename_component(_abs_file ${_current_FILE} ABSOLUTE)
        string(REPLACE "." "" _current_FILE_EXT ${_current_FILE_EXT})
        list(FIND DOXYGEN_PATTERNS ${_current_FILE_EXT} EXT_INDEX)
        if(EXT_INDEX GREATER -1)
            list(FIND DOXYGEN_SOURCES ${_abs_file} FILE_INDEX)
            if(FILE_INDEX EQUAL -1)
                set(DOXYGEN_SOURCES ${DOXYGEN_SOURCES};${_abs_file} CACHE  STRING "" FORCE)
            endif()
        endif()
    endforeach()
endmacro(eb_doxygen_add_headers)
#####################################################################
# eb_install(install_configurations [install_args])
#
# calls the install command when GTF_INSTALL_TYPE is one of install_configurations
macro(eb_install INSTALL_CONFIGURATIONS)
    set(INSTALL_CONFIGURATIONS "${INSTALL_CONFIGURATIONS}") # WTF???
    foreach(install_config ${GTF_INSTALL_TYPE})
        list(FIND INSTALL_CONFIGURATIONS ${install_config} EB_INSTALL_DO_IT)
        if(EB_INSTALL_DO_IT GREATER -1 OR ${install_config} STREQUAL All)
            install(${ARGN})
            eb_doxygen_add_headers(${ARGN})
            break()
        endif()
    endforeach()
endmacro(eb_install)


#####################################################################
# eb_add_doxygen(install_configurations [install_args])
#
# adds files to doxygen generation when GTF_INSTALL_TYPE is one of install_configurations
macro(eb_add_doxygen INSTALL_CONFIGURATIONS)
endmacro(eb_add_doxygen)

macro(eb_add_doxygen_absolute INSTALL_CONFIGURATIONS)
endmacro(eb_add_doxygen_absolute INSTALL_CONFIGURATIONS)

macro(eb_replace_flags flags from to)
    #MESSAGE("modifying ${flags} from ${from} to ${to}")
    STRING(REPLACE "${from}" "${to}" Temp "${${flags}}")
    SET(${flags} ${Temp})
    #MESSAGE("flags ${flags} are now set to ${Temp}")
endmacro(eb_replace_flags)
