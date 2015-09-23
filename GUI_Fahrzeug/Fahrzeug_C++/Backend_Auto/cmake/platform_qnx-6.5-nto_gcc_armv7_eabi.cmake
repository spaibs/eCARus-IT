SET(CMAKE_SYSTEM_NAME QNX)
SET(CMAKE_SYSTEM_VERSION 6.5.0)
SET(ARM 1)
SET(CMAKE_SYSTEM_PROCESSOR ARM)

set(EB_PLATFORM_OS_FAMILY POSIX)
set(EB_PLATFORM_OS QNX)
set(EB_PLATFORM_ARCH ARM)

## The following to env variables must be defined, otherwise gcc wont work
SET(QNX_TARGET $ENV{QNX_TARGET} CACHE PATH "Path to QNX Target env")
SET(QNX_HOST $ENV{QNX_HOST} CACHE PATH "Path to QNX host tool chain")
# hence we reuse the variables, but the Cygwin paths must be converted into
# windows paths before that:
STRING(REGEX REPLACE "/cygdrive/\(.\)" "\\1:" QNX_HOST_CMAKE_PATH ${QNX_HOST})
STRING(REGEX REPLACE "/cygdrive/\(.\)" "\\1:" QNX_TARGET_CMAKE_PATH ${QNX_TARGET})

MESSAGE(STATUS "Compiling for QNX 6.5.0 ARM")

## Workaround, GET_FILENAME_COMPONENT( .. NAME_WE) has a bug in 2.6, it uses the
## first dot of the filename to detect the extension
SET(_CMAKE_TOOLCHAIN_PREFIX "arm-unknown-nto-qnx6.5.0eabi-")

SET(CMAKE_CXX_COMPILER "${QNX_HOST_CMAKE_PATH}/usr/bin/arm-unknown-nto-qnx6.5.0eabi-g++.exe")
SET(CMAKE_C_COMPILER "${QNX_HOST_CMAKE_PATH}/usr/bin/arm-unknown-nto-qnx6.5.0eabi-gcc.exe")

# Added here so the platform file can be used for other projects, too:
ADD_DEFINITIONS(
    -D_QNX_SOURCE  # Required for the gnu headers: http://gcc.gnu.org/ml/gcc/2004-04/msg00230.html
)

SET(CMAKE_FIND_ROOT_PATH ${QNX_HOST_CMAKE_PATH} ${QNX_TARGET_CMAKE_PATH})

SET( THREADS_PTHREAD_ARG
     0
     CACHE STRING "Result from TRY_RUN" FORCE)

SET(CMAKE_CXX_FLAGS "-march=armv7-a -mtune=cortex-a8 -mfloat-abi=softfp -mfpu=neon -fPIC" CACHE STRING "" FORCE)
SET(CMAKE_C_FLAGS "-march=armv7-a -mtune=cortex-a8 -mfloat-abi=softfp -mfpu=neon -fPIC" CACHE STRING "" FORCE)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
