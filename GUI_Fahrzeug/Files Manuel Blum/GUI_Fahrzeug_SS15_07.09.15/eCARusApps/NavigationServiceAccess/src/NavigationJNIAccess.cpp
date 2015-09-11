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

#include <string.h>
#include <GtfTypes/GtfMacros.h> // For GTF_UNUSED_PARAM
#include "NavigationJNIAccess.h"

#ifdef ANDROID

#define JAVA_ACCESS_CLASS "com/example/gtflauncher/NavigationJNIAccess"

// The global JNI ressources
static JavaVM* g_javaVM = NULL;
static jclass gClsAP = NULL;

// JNI entry point, This is executed when the Java virtual machine attaches to the native library.
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env;
    g_javaVM = vm; // Store the reference to the virtual machine

    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        LOGCAT_ERROR("JNI_OnLoad vm->GetEnv failed!");
        return JNI_ERR;
    }

    if (vm->AttachCurrentThread(&env, 0) != JNI_OK)
    {
        LOGCAT_ERROR("JNI_OnLoad() - AttachCurrentThread() failed!");
        return JNI_ERR;
    }

    if(NULL != env)
    {
        LOGCAT_DEBUG("JNI_OnLoad vm->GetEnv successful.");

        gClsAP = env->FindClass(JAVA_ACCESS_CLASS);
        gClsAP = (jclass) env->NewGlobalRef(gClsAP); // Important: Declare as global reference
        if (NULL == gClsAP)
        {
            LOGCAT_ERROR("JNI_OnLoad() - FindClass failed!");
            vm->DetachCurrentThread();
            return JNI_ERR;
        }
    }
    else
    {
        LOGCAT_ERROR("JNI_OnLoad() - vm->GetEnv error!");
        vm->DetachCurrentThread();
        return JNI_ERR;
    }

    return JNI_VERSION_1_6;
}

void JNI_OnUnload(JavaVM* vm, void* reserved)
{
    // Nothing to do here since android-dl closes the application before OnUnload
    // We'll do the deinit when the application shutdown is performed by GTF
    // in order to allow GTF to unload this library
}

#endif // ANDROID

//
// JNI calls to JAVA

void NavigationJNIAccess::onTouchMove(int32_t deltaX, int32_t deltaY)
{
#ifdef ANDROID
    JNIEnv *env = getEnv();
    if(env)
    {
        jmethodID mid = env->GetStaticMethodID(gClsAP, "onTouchMove", "(II)V");
        if (mid != NULL) 
        {
            // Just call the void method without parameters
            env->CallStaticVoidMethod(gClsAP, mid, deltaX, deltaY);
        }
        else
        {
            LOGCAT_ERROR("onTouchMove() - GetMethodID failed!");
        }
    }
#else
    // If not Android -> Nothing to do here
    // To avoid compiler warnings e. g. on Windows use GTF_UNUSED_PARAM macro
    GTF_UNUSED_PARAM(deltaX) 
    GTF_UNUSED_PARAM(deltaY)
#endif // ANDROID
}

void NavigationJNIAccess::onTouchRotate(float rotationAngle)
{
#ifdef ANDROID
    JNIEnv *env = getEnv();
    if(env)
    {
        jmethodID mid = env->GetStaticMethodID(gClsAP, "onTouchRotate", "(F)V");
        if (mid != NULL) 
        {
            env->CallStaticVoidMethod(gClsAP, mid, rotationAngle);
        }
        else
        {
            LOGCAT_ERROR("onTouchRotate() - GetMethodID failed!");
        }
    }
#else
    GTF_UNUSED_PARAM(rotationAngle)
#endif // ANDROID
}

void NavigationJNIAccess::onTouchZoom(float zoomFactor)
{
#ifdef ANDROID
    JNIEnv *env = getEnv();
    if(env)
    {
        jmethodID mid = env->GetStaticMethodID(gClsAP, "onTouchZoom", "(F)V");
        if (mid != NULL) 
        {
            env->CallStaticVoidMethod(gClsAP, mid, zoomFactor);
        }
        else
        {
            LOGCAT_ERROR("onTouchZoom() - GetMethodID failed!");
        }
    }
#else
    GTF_UNUSED_PARAM(zoomFactor)
#endif // ANDROID
}

void NavigationJNIAccess::onDoubleTap(int32_t tapX, int32_t tapY)
{
#ifdef ANDROID
    JNIEnv *env = getEnv();
    if(env)
    {
        jmethodID mid = env->GetStaticMethodID(gClsAP, "onDoubleTap", "(II)V");
        if (mid != NULL) 
        {
            env->CallStaticVoidMethod(gClsAP, mid, tapX, tapY);
        }
        else
        {
            LOGCAT_ERROR("onDoubleTap() - GetMethodID failed!");
        }
    }
#else
    GTF_UNUSED_PARAM(tapX) 
    GTF_UNUSED_PARAM(tapY)
#endif // ANDROID
}

void NavigationJNIAccess::deinitialize()
{
#ifdef ANDROID
    LOGCAT_DEBUG("deinitialize()");
    JNIEnv *env = getEnv();
    if(env)
    {
        if(NULL != gClsAP)
        {
            env->DeleteGlobalRef((jclass) gClsAP);
        }
        else
        {
            LOGCAT_ERROR("deinitialize() - DeleteGlobalRef failed!");
        }
    }

    if(g_javaVM)
    {
        g_javaVM->DetachCurrentThread();
    }
    else
    {
        LOGCAT_WARN("deinitialize() - g_javaVM was null.");
    }
#endif // ANDROID
}

#ifdef ANDROID
JNIEnv* NavigationJNIAccess::getEnv()
{
    JNIEnv* retVal = NULL;

    if(g_javaVM)
    {
        int ret = g_javaVM->GetEnv((void**)&retVal, JNI_VERSION_1_6);
        if(JNI_EDETACHED == ret)
        {
            if(JNI_OK != g_javaVM->AttachCurrentThread(&retVal, NULL))
            {
                LOGCAT_ERROR("getEnv() - AttachCurrentThread failed!");
                retVal = NULL;
            }
        }
    }
    else
    {
        LOGCAT_ERROR("getEnv() - g_javaVM was null!");
    }

    return retVal;
}
#endif // ANDROID
