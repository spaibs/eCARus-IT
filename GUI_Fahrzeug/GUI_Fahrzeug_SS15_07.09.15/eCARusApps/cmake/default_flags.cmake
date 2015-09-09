# -------------------------------
## UNICODE and _UNICODE are used to enable unicode support
##  this setting primary enables wide character windows api
add_definitions(-DUNICODE -D_UNICODE)

INCLUDE(TestBigEndian)
TEST_BIG_ENDIAN(HMI_IS_BIG_ENDIAN)

IF(HMI_IS_BIG_ENDIAN)
    ## IPL is for Infotainment framework code
    add_definitions(-DHMI_HOST_IS_BIG_ENDIAN -DIPL_FW_BIG_ENDIAN)
ELSE(HMI_IS_BIG_ENDIAN)
    add_definitions(-DHMI_HOST_IS_LITTLE_ENDIAN -DIPL_FW_LITTLE_ENDIAN)
ENDIF(HMI_IS_BIG_ENDIAN)

if(EB_PLATFORM_OS_WIN32)
    add_definitions(-D_M_IX86)
elseif(EB_PLATFORM_OS_WINCE)
    add_definitions(-DUNDER_CE)
elseif(EB_PLATFORM_OS_LINUX)
    add_definitions(-Dlinux)
    set(LINUX 1)
elseif(EB_PLATFORM_OS_QNX)
endif(EB_PLATFORM_OS_WIN32)

IF(EB_PLATFORM_OS_FAMILY_POSIX)
    # For now this is only used by: HmiModuleLoader Test cases
    add_definitions(-DPOSIX)
    ## This macro is defined by the OpenGroup to allow the user of the system headers
    ## to select a certain featuresset, valid values are:1, 2, 199309L, 199506L, 200112L, 200809L
    add_definitions(-D_POSIX_C_SOURCE=199506L)
ENDIF(EB_PLATFORM_OS_FAMILY_POSIX)

IF(MSVC)
    option(GTF_GENERATE_ASSEMBLIES "Enables the generation of assembly listings during compilation for Visual Studio compilers." OFF)

    if(GTF_GENERATE_ASSEMBLIES)
        set(GTF_GENERATE_ASSEMBLIES_FLAGS "/FAcs /Fa$(IntDir)\\")
    else(GTF_GENERATE_ASSEMBLIES)
        set(GTF_GENERATE_ASSEMBLIES_FLAGS "")
    endif(GTF_GENERATE_ASSEMBLIES)

    option(GTF_TREAT_COMPILER_WARNINGS_AS_ERROR "Treats all compiler warnings as errors during compilation for Visual Studio compilers." ON)

    if(GTF_TREAT_COMPILER_WARNINGS_AS_ERROR)
        set(GTF_TREAT_COMPILER_WARNINGS_AS_ERROR_FLAGS "/WX")
    else(GTF_TREAT_COMPILER_WARNINGS_AS_ERROR)
        set(GTF_TREAT_COMPILER_WARNINGS_AS_ERROR_FLAGS "")
    endif(GTF_TREAT_COMPILER_WARNINGS_AS_ERROR)

    ## disable warnings
    ## C4345: this warning is pretty much By-Design - it is just letting the user know about a change in behavior in the compiler. As it is a warning you can disable it.
    ## C4355: 'this' : used in base member initializer list
    add_definitions(/wd4345 /wd4355)

    # -------------------------------
    # setting default compiler options (os specific)
    # -------------------------------
    if(EB_PLATFORM_OS_WIN32)
        set(GTF_DEFAULT_COMPILER_FLAGS
            /DWIN32
            /D_WINDOWS
            /D "_CRT_SECURE_NO_DEPRECATE"
            /D "_CRT_NONSTDC_NO_DEPRECATE"
            /D "_SCL_SECURE_NO_DEPRECATE"
            /D "_CRT_NON_CONFORMING_SWPRINTFS"
            /D "_HAS_EXCEPTIONS=0"
            /Gy          # function level linking
            /GR-         # no rtti
            /W4          # warning level 4
            /Zc:wchar_t- # wchar_t as Built-in type
            /nologo
    )
    elseif(EB_PLATFORM_OS_WINCE)
        set(GTF_DEFAULT_COMPILER_FLAGS
            /D_WINDOWS
            /D "_CRT_SECURE_NO_DEPRECATE"
            /D "_CRT_NONSTDC_NO_DEPRECATE"
            /D "_SCL_SECURE_NO_DEPRECATE"
            /D "_CRT_NON_CONFORMING_SWPRINTFS"
            /D "_HAS_EXCEPTIONS=0"
            /GR-         # no rtti
            /W4          # warning level 4
            /Zc:wchar_t- # wchar_t as Built-in type
            /nologo
            #/QRarch7 #Renesas R-CarH1
            #/arch:VFPv3-D32 #Renesas R-CarH1
    )
    else()
        set(GTF_DEFAULT_COMPILER_FLAGS
            /D "_CRT_SECURE_NO_DEPRECATE"
            /D "_CRT_NONSTDC_NO_DEPRECATE"
            /D "_SCL_SECURE_NO_DEPRECATE"
            /D "_CRT_NON_CONFORMING_SWPRINTFS"
            /D "_HAS_EXCEPTIONS=0"
            /GR-         # no rtti
            /W4          # warning level 4
            /Zc:wchar_t- # wchar_t as Built-in type
            /nologo
    )
    endif(EB_PLATFORM_OS_WIN32)

    string(REPLACE ";" " " Temp "${GTF_DEFAULT_COMPILER_FLAGS}")
    set(GTF_COMPILER_FLAGS ${Temp} CACHE STRING "GTF compiler flags")
    mark_as_advanced(GTF_COMPILER_FLAGS)

    set(CMAKE_C_FLAGS   "${GTF_COMPILER_FLAGS} ${GTF_TREAT_COMPILER_WARNINGS_AS_ERROR_FLAGS}" CACHE INTERNAL "")
    set(CMAKE_CXX_FLAGS "${GTF_COMPILER_FLAGS} ${GTF_TREAT_COMPILER_WARNINGS_AS_ERROR_FLAGS}" CACHE INTERNAL "")

    # -------------------------------
    # setting default compiler options (for debug)
    # -------------------------------
    set(GTF_DEFAULT_COMPILER_FLAGS_DEBUG
        /D_DEBUG
        /Zi           # program database
        /Ob0    # do not expand inline functions
        /Od     # do not optimize
        /MTd    # static multi-threaded debug c-runtime TODO: remove for WinCE6/7
        /GS     # buffer security checks
    )

    if(GTF_ENABLE_DEBUG_RUNTIME_CHECKS)
        set(GTF_ENABLE_DEBUG_RUNTIME_CHECKS_FLAGS "/RTC1")
    else()
        set(GTF_ENABLE_DEBUG_RUNTIME_CHECKS_FLAGS "")
    endif(GTF_ENABLE_DEBUG_RUNTIME_CHECKS)

    string(REPLACE ";" " " Temp "${GTF_DEFAULT_COMPILER_FLAGS_DEBUG}")
    set(GTF_COMPILER_FLAGS_DEBUG ${Temp} CACHE STRING "Additional compiler flags for debug build")
    mark_as_advanced(GTF_COMPILER_FLAGS_DEBUG)
    set(CMAKE_C_FLAGS_DEBUG   "${GTF_COMPILER_FLAGS_DEBUG} ${GTF_ENABLE_DEBUG_RUNTIME_CHECKS_FLAGS}" CACHE INTERNAL "")
    set(CMAKE_CXX_FLAGS_DEBUG "${GTF_COMPILER_FLAGS_DEBUG} ${GTF_ENABLE_DEBUG_RUNTIME_CHECKS_FLAGS}" CACHE INTERNAL "")

    # -------------------------------
    # setting default compiler options (for release)
    # -------------------------------
    set(GTF_DEFAULT_COMPILER_FLAGS_RELEASE
        /DNDEBUG
        /D "_SECURE_SCL=0"
        /Ob2    # expand inline functions
        /Oi     # enable instrinsics
        /Ot     # favor fast code
        /O2     # optimize for speed
        /MT     # static multi-threaded release c-runtime TODO: remove for WinCE6/7
        /GS-    # no buffer security checks
        /Zi     # program database
    )
    string(REPLACE ";" " " Temp "${GTF_DEFAULT_COMPILER_FLAGS_RELEASE}")
    set(GTF_COMPILER_FLAGS_RELEASE ${Temp} CACHE STRING "Additional compiler flags for release build")
    mark_as_advanced(GTF_COMPILER_FLAGS_RELEASE)
    set(CMAKE_C_FLAGS_RELEASE   "${GTF_COMPILER_FLAGS_RELEASE} ${GTF_GENERATE_ASSEMBLIES_FLAGS}" CACHE INTERNAL "")
    set(CMAKE_CXX_FLAGS_RELEASE "${GTF_COMPILER_FLAGS_RELEASE} ${GTF_GENERATE_ASSEMBLIES_FLAGS}" CACHE INTERNAL "")

    set(Temp "")

    # -------------------------------
    # linker settings
    # -------------------------------
    if(EB_PLATFORM_OS_WIN32)
    set(Temp
        /MACHINE:X86
        ws2_32.lib
    )
    elseif(EB_PLATFORM_OS_WINCE)
        set(Temp
            /FORCE:MULTIPLE
            /SUBSYSTEM:WINDOWSCE
            /STACK:65536,4096
    )
    endif()

    # -------------------------------
    # setting default linker flags
    # -------------------------------
    set(GTF_DEFAULT_LINKER_FLAGS
        /MANIFEST
        ${Temp}
    )

    string(REPLACE ";" " " Temp "${GTF_DEFAULT_LINKER_FLAGS}")
    set(GTF_LINKER_FLAGS "${Temp} ${CMAKE_EXE_LINKER_FLAGS_INIT}" CACHE STRING "GTF Linker flags")
    mark_as_advanced(GTF_LINKER_FLAGS)

    set(CMAKE_EXE_LINKER_FLAGS    "${GTF_LINKER_FLAGS}" CACHE INTERNAL "")
    set(CMAKE_MODULE_LINKER_FLAGS "${GTF_LINKER_FLAGS}" CACHE INTERNAL "")
    set(CMAKE_SHARED_LINKER_FLAGS "${GTF_LINKER_FLAGS}" CACHE INTERNAL "")

    # -------------------------------
    # setting default linker flags (debug)
    # -------------------------------
    if(EB_PLATFORM_OS_WIN32)
        set(GTF_DEFAULT_LINKER_FLAGS_DEBUG
            /DEBUG            # create pdb
            /MAP              # always generate map files (for map file tool e.g.)
            /INCREMENTAL:YES  # link incremental
            libcpmtd.lib      # use libc multi-threaded debug
    )
    elseif(EB_PLATFORM_OS_WINCE AND EB_PLATFORM_ARCH_ARM)
        set(GTF_DEFAULT_LINKER_FLAGS_DEBUG
            /DEBUG            # create pdb
            /MAP              # always generate map files (for map file tool e.g.)
            /INCREMENTAL:YES  # link incremental
            /MACHINE:THUMB    #
            coredll.lib       # use libc multi-threaded debug
            corelibc.lib      # use libc multi-threaded debug
            libcmtd.lib       # use libc multi-threaded debug
            libcpmtd.lib      # use libc multi-threaded debug
    )
    elseif(EB_PLATFORM_OS_WINCE AND EB_PLATFORM_ARCH_SH4)
        set(GTF_DEFAULT_LINKER_FLAGS_DEBUG
            /DEBUG           # create pdb
            /MAP             # always generate map files (for map file tool e.g.)
            /INCREMENTAL:YES # link incremental
            /MACHINE:SH4
            /NODEFAULTLIB:"libcpmtd.lib"
    )
    endif()

    string(REPLACE ";" " " Temp "${GTF_DEFAULT_LINKER_FLAGS_DEBUG}")
    set(GTF_LINKER_FLAGS_DEBUG "${Temp}" CACHE STRING "Additional linker flags for debug build")
    mark_as_advanced(GTF_LINKER_FLAGS_DEBUG)
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG    "${GTF_LINKER_FLAGS_DEBUG}" CACHE INTERNAL "")
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${GTF_LINKER_FLAGS_DEBUG}" CACHE INTERNAL "")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${GTF_LINKER_FLAGS_DEBUG}" CACHE INTERNAL "")

    # -------------------------------
    # setting default linker flags (release)
    # -------------------------------
    set(GTF_DEFAULT_LINKER_FLAGS_RELEASE
        /DEBUG          # create pdb
        /OPT:REF        # optimize references
        /OPT:ICF        # optimize comdats
        /INCREMENTAL:NO # link incremental
        /MAP            # always generate map files (for map file tool e.g.)
        libcmt.lib      # use libc multi-threaded release TODO: remove for WinCE6/7
    )
    string(REPLACE ";" " " Temp "${GTF_DEFAULT_LINKER_FLAGS_RELEASE}")
    set(GTF_LINKER_FLAGS_RELEASE ${Temp} CACHE STRING "Additional linker flags for release build")
    mark_as_advanced(GTF_LINKER_FLAGS_RELEASE)
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE    "${GTF_LINKER_FLAGS_RELEASE} " CACHE INTERNAL "")
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${GTF_LINKER_FLAGS_RELEASE} " CACHE INTERNAL "")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${GTF_LINKER_FLAGS_RELEASE} " CACHE INTERNAL "")

ENDIF(MSVC)

IF(CMAKE_COMPILER_IS_GNUCC)

    set(CMAKE_SHARED_LIBRARY_RUNTIME_C_FLAG "")
    set(CMAKE_SHARED_LIBRARY_RUNTIME_CXX_FLAG "")

    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag(-fvisibility=hidden GCC_SUPPORTS_VISIBILITY)
    add_definitions(-DGCC_HAS_VISIBILITY)

    if(GCC_SUPPORTS_VISIBILITY)
        FORCE_ADD_FLAGS(CMAKE_CXX_FLAGS -fvisibility=hidden)
    endif()

    FORCE_ADD_FLAGS(CMAKE_C_FLAGS "-Wall -Wshadow -fno-strict-aliasing ")
    FORCE_ADD_FLAGS(CMAKE_CXX_FLAGS "-Wall -Wshadow -Wno-invalid-offsetof -fno-strict-aliasing -fno-exceptions -fno-rtti ")

# Although it is a gcc frontend -pipe is not supported when using qcc for compiling
   if(NOT CMAKE_COMPILER_IS_QCC)

    FORCE_ADD_FLAGS(CMAKE_C_FLAGS "-pipe")
    FORCE_ADD_FLAGS(CMAKE_CXX_FLAGS "-pipe")

   endif()

    ## HACK
    ## We build static libraries, with the intention to link them into shared object
    ## files (google for convenience libraries). It would be better to add that
    ## flag only to those static libs that shall be part of dynamic libs. Therefore
    ## we would need a wrapper around ADD_LIBRARY, or something similar. For now most
    ## of our code is part of shared object files and DLLs.
    ## Note: -fPIC is ignored on win32 (relocations are always used)
    if (NOT(EB_PLATFORM_OS_WIN32 OR EB_PLATFORM_OS_WINCE))
        FORCE_ADD_FLAGS(CMAKE_C_FLAGS "-fPIC")
        FORCE_ADD_FLAGS(CMAKE_CXX_FLAGS "-fPIC")
    endif(NOT(EB_PLATFORM_OS_WIN32 OR EB_PLATFORM_OS_WINCE))

    # -Bsymbolic tells ld to locally resolve global objects in a shared library
    # without Bsymbolic all global symbols are shared ~> This is particularly bad because
    # we build shared libraries from static libs -> without bsymbolic hell freezes



    if (USE_LD_DIRECTLY)
       MESSAGE(STATUS "using ld directly")
       FORCE_ADD_FLAGS(CMAKE_SHARED_LINKER_FLAGS "-Bsymbolic")
       FORCE_ADD_FLAGS(CMAKE_MODULE_LINKER_FLAGS "-Bsymbolic")
    else()
       MESSAGE(STATUS "using gcc for linking")
       FORCE_ADD_FLAGS(CMAKE_SHARED_LINKER_FLAGS "-Wl,-Bsymbolic")
       FORCE_ADD_FLAGS(CMAKE_MODULE_LINKER_FLAGS "-Wl,-Bsymbolic")
    endif()

ENDIF(CMAKE_COMPILER_IS_GNUCC)

IF(EB_PLATFORM_OS_QNX)
    ## CMAKE_CXX_FLAGS and CMAKE_C_FLAGS are to be defined in toolchain file.+
    option(QNX_ENABLE_FUNCTION_PROFILING "Enable Function level profiling")
    if(QNX_ENABLE_FUNCTION_PROFILING)
         FORCE_ADD_FLAGS(CMAKE_C_FLAGS "-finstrument-functions")
         FORCE_ADD_FLAGS(CMAKE_CXX_FLAGS "-finstrument-functions")
    endif(QNX_ENABLE_FUNCTION_PROFILING)
    FORCE_ADD_FLAGS(CMAKE_EXE_LINKER_FLAGS "-lcpp-ne -lcxa")
    FORCE_ADD_FLAGS(CMAKE_MODULE_LINKER_FLAGS "-lcpp-ne -lcxa")
    FORCE_ADD_FLAGS(CMAKE_SHARED_LINKER_FLAGS "-lcpp-ne -lcxa")
ENDIF(EB_PLATFORM_OS_QNX)

OPTION(CLANG_ENABLE_ADDRESS_SANITIZER "enable checking of memory accesses when building with clang" OFF)
IF(CLANG_ENABLE_ADDRESS_SANITIZER)
  FORCE_ADD_FLAGS(CMAKE_C_FLAGS "-O2 -fno-omit-frame-pointer -g -faddress-sanitizer")
  FORCE_ADD_FLAGS(CMAKE_CXX_FLAGS "-O2 -fno-omit-frame-pointer -g -faddress-sanitizer")
  FORCE_ADD_FLAGS(CMAKE_EXE_LINKER_FLAGS "-L${CLANG_LIBRARY_PATH}")
  FORCE_ADD_FLAGS(CMAKE_MODULE_LINKER_FLAGS "-L${CLANG_LIBRARY_PATH}")
  FORCE_ADD_FLAGS(CMAKE_SHARED_LINKER_FLAGS "-L${CLANG_LIBRARY_PATH}")
ENDIF(CLANG_ENABLE_ADDRESS_SANITIZER)
