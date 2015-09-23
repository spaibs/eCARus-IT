export ANDROID_NDK_ROOT=C:/Users/jonas/Desktop/android-ndk-r8e
SDK_PATH="C:/Elektrobit/EB GUIDE 5.5.3/platform/android_9_armv7"

INSTALL_PATH="./target"

CMAKE_PATH="C:/CMake/bin"

$CMAKE_PATH/cmake.exe -D GTF_INSTALL_TYPE:STRING=All \
    -D ANDROID_NDK_PREBUILT:STRING=windows-x86_64 \
    -D CMAKE_TOOLCHAIN_FILE:=../Backend_Auto/toolchain_android.cmake \
    -D DOXYGEN_GENERATION_ENABLE=OFF  \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_INSTALL_PREFIX:PATH="${INSTALL_PATH}"  -G "Unix Makefiles"  \
    -D GTF_INSTALL_DEBUG_SYMBOL_FILES:BOOL=ON \
    -D GTF_TARGET_SDK_ROOT:PATH="${SDK_PATH}" \
    -D GTF_MEMORY_HAVE_NEW=1 \
    ../Backend_Auto/ --trace 2> cmake_log.txt 
$CMAKE_PATH/cmake.exe .
