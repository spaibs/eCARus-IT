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

#include <GtfTypes/GtfMacros.h>
#include "LogCatLogger.h"
#include "TowerAccessJNIManager.h"
#include <string>

#ifdef ANDROID
#include "TowerAccessJNISignatures.h"

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

        gClsAP = env->FindClass("com/example/gtflauncher/tower/TowerJNIAccess");
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
    // We'll do the deinit when shutdown of AudioPlayerService is called
}

// JNI Methods: Java --> C++

/*
 * Class:     com_example_gtflauncher_tower_TowerJNIAccess
 * Method:    onLightStateUpdated
 * Signature: (ZZZZ)V
 */
JNIEXPORT void JNICALL Java_com_example_gtflauncher_tower_TowerJNIAccess_onLightStateUpdated
	(JNIEnv *env, jclass cls, jboolean lowBeamIsOn, jboolean highBeamIsOn, jboolean indicatorRightIsOn, jboolean indicatorLeftIsOn, jboolean indicatorWarningLightIsOn)
{
    LOGCAT_DEBUG("JNI Call: onLightStateUpdated()");

    TowerUpdateObserver *theObserver = TowerAccessJNIManager::getInstance()->getObserver();
    if(theObserver)
    {
		theObserver->onLightStateUpdated((bool) lowBeamIsOn, (bool) highBeamIsOn, (bool) indicatorRightIsOn, (bool) indicatorLeftIsOn, (bool) indicatorWarningLightIsOn);
    }
}

/*
* Class:     com_example_gtflauncher_tower_TowerJNIAccess
* Method:    onGestureDetected
* Signature: (I)V
*/
JNIEXPORT void JNICALL Java_com_example_gtflauncher_tower_TowerJNIAccess_onGestureDetected
	(JNIEnv *env, jclass cls, jint gestureType)
{
	LOGCAT_DEBUG("JNI Call: onGestureDetected()");

	TowerUpdateObserver *theObserver = TowerAccessJNIManager::getInstance()->getObserver();
	if (theObserver)
	{
		theObserver->onGestureDetected((int32_t) gestureType);
	}
}

#endif // ANDROID

// JNI Methods: C++ --> Java

void TowerAccessJNIManager::updateVehicleMode(int32_t newMode)
{
#ifdef ANDROID
	JNIEnv *env = getEnv();
	if (env)
	{
		jmethodID mid = env->GetStaticMethodID(gClsAP, "onSetVehicleMode", "(I)V");
		if (mid != NULL)
		{
			jint modeParameter = (jint) newMode;
			env->CallStaticVoidMethod(gClsAP, mid, modeParameter);
		}
		else
		{
			LOGCAT_ERROR("updateVehicleMode() - GetMethodID failed!");
		}
	}
#else
	GTF_UNUSED_PARAM(newMode);
#endif // ANDROID
}

void TowerAccessJNIManager::requestBatteryValuesUpdate()
{
#ifdef ANDROID
	JNIEnv *env = getEnv();
	if (env)
	{
		jmethodID mid = env->GetStaticMethodID(gClsAP, "onBatteryValueUpdateRequested", "()V");
		if (mid != NULL)
		{
			env->CallStaticVoidMethod(gClsAP, mid);
		}
		else
		{
			LOGCAT_ERROR("requestBatteryValuesUpdate() - GetMethodID failed!");
		}
	}
#endif // ANDROID
}




void TowerAccessJNIManager::sendRating(int32_t rating)
{
#ifdef ANDROID
	JNIEnv *env = getEnv();
	if (env)
	{
		jmethodID mid = env->GetStaticMethodID(gClsAP, "onTransmitRating", "(I)V");
		if (mid != NULL)
		{
			jint ratingParameter = (jint) rating;
			env->CallStaticVoidMethod(gClsAP, mid, ratingParameter);
		}
		else
		{
			LOGCAT_ERROR("sendRating() - GetMethodID failed!");
		}
	}
#else
	GTF_UNUSED_PARAM(newMode);
#endif // ANDROID
}



void TowerAccessJNIManager::deinitialize()
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

JNIEnv* TowerAccessJNIManager::getEnv()
{
    JNIEnv* retVal = NULL;

    if(g_javaVM)
    {
        int ret = g_javaVM->GetEnv((void**)&retVal, JNI_VERSION_1_6);
        //LOGCAT_DEBUG("TowerAccessJNIManager - getEnv() - retVal=%d", ret);
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

//
// The observer methods
void TowerAccessJNIManager::registerObserver(TowerUpdateObserver* observer)
{
    if(observer)
    {
        updateObserver = observer;
    }
}

void TowerAccessJNIManager::unregisterObserver(TowerUpdateObserver* observer)
{
	if (observer && observer == updateObserver)
    {
		updateObserver = NULL;
    }
}

TowerUpdateObserver* TowerAccessJNIManager::getObserver()
{
	return updateObserver;
}
