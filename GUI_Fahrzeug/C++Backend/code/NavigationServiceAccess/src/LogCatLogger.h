////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
//
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

#ifndef LOGCAT_LOGGER_H
#define LOGCAT_LOGGER_H

//
// Some useful logging macros for Android
// Log messages will be seen in Eclipse DDMS window
// Android device has to be connected using ADB driver
#ifdef ANDROID

    #include <android/log.h>
    #include <time.h>

    #define LOGCAT_TAG "NavigationServiceAccess"

    #define LOGCAT_VERBOSE(...) ((void)__android_log_print(ANDROID_LOG_VERBOSE, LOGCAT_TAG, __VA_ARGS__))
    #define LOGCAT_DEBUG(...)   ((void)__android_log_print(ANDROID_LOG_DEBUG,   LOGCAT_TAG, __VA_ARGS__))
    #define LOGCAT_INFO(...)    ((void)__android_log_print(ANDROID_LOG_INFO,    LOGCAT_TAG, __VA_ARGS__))
    #define LOGCAT_WARN(...)    ((void)__android_log_print(ANDROID_LOG_WARN,    LOGCAT_TAG, __VA_ARGS__))
    #define LOGCAT_ERROR(...)   ((void)__android_log_print(ANDROID_LOG_ERROR,   LOGCAT_TAG, __VA_ARGS__))
    #define LOGCAT_FATAL(...)   ((void)__android_log_print(ANDROID_LOG_FATAL,   LOGCAT_TAG, __VA_ARGS__))

#else

    // Just empty macros for targets other than Android
    // No trace output here!
    #define LOGCAT_VERBOSE(...)
    #define LOGCAT_DEBUG(...)
    #define LOGCAT_INFO(...)
    #define LOGCAT_WARN(...)
    #define LOGCAT_ERROR(...)
    #define LOGCAT_FATAL(...)

#endif

#endif // LOGCAT_LOGGER_H
