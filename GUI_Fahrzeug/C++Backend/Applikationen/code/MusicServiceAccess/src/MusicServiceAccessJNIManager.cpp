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
#include "MusicServiceAccessJNIManager.h"
#include <string.h>

#ifdef ANDROID
#include "MusicServiceAccessJNISignatures.h"

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

        gClsAP = env->FindClass("com/example/gtflauncher/music/MusicJNIAccess");
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
    * Class:     com_example_gtflauncher_music_MusicJNIAccess
    * Method:    updateTrackList
    * Signature: ([Ljava/lang/String;)V
    */
JNIEXPORT void JNICALL Java_com_example_gtflauncher_music_MusicJNIAccess_updateTrackList
(JNIEnv *env, jclass cls, jobjectArray trackList)
{
    LOGCAT_DEBUG("JNI Call: updateTrackList()");

    int stringCount = env->GetArrayLength(trackList);

    GtfDataPool *pDataPool = NULL;
    GtfTypesDP::contextId_t ctxId = 0;
    uint32_t trackListPropertyId = 0;

    MusicServiceUpdateObserver* observer = MusicServiceAccessJNIManager::getInstance()->getObserver();
    if(observer)
    {
        trackListPropertyId = observer->getTrackPropertyId(&pDataPool, &ctxId);
        if(pDataPool)
        {
            pDataPool->List_Clear(ctxId, trackListPropertyId);
        }
    }

    if(pDataPool)
    {
        for(int i=0; i<stringCount; ++i)
        {
            jstring trackString = (jstring) env->GetObjectArrayElement(trackList, i);
            const char *rawString = env->GetStringUTFChars(trackString, 0);

            //LOGCAT_DEBUG("Track item no. %d: %s", i, rawString);
            pDataPool->List_AppendValue(ctxId, trackListPropertyId, GtfTypeId::eTypeId_string, rawString, strlen(rawString));

            env->ReleaseStringUTFChars(trackString, rawString);
        }

        pDataPool->Commit(ctxId);
    }
}


#endif // ANDROID

// JNI Methods: C++ --> Java

/////////////////////////////////////////////////////////////////////

void MusicServiceAccessJNIManager::playTitle(int32_t trackIndex)
{
#ifdef ANDROID
	JNIEnv *env = getEnv();
	if (env)
	{
		jmethodID mid = env->GetStaticMethodID(gClsAP, "playTitle", "(I)V");
		if (mid != NULL)
		{
            env->CallStaticVoidMethod(gClsAP, mid, trackIndex);
		}
		else
		{
			LOGCAT_ERROR("playTitle() - GetMethodID failed!");
		}
	}
#else
    GTF_UNUSED_PARAM(trackIndex);
#endif // ANDROID
}

void MusicServiceAccessJNIManager::pauseTitle()
{
#ifdef ANDROID
	JNIEnv *env = getEnv();
	if (env)
	{
		jmethodID mid = env->GetStaticMethodID(gClsAP, "pauseTitle", "()V");
		if (mid != NULL)
		{
			env->CallStaticVoidMethod(gClsAP, mid);
		}
		else
		{
			LOGCAT_ERROR("pauseTitle() - GetMethodID failed!");
		}
	}
#endif // ANDROID
}

void MusicServiceAccessJNIManager::updateVolume(float newVolumeValue)
{
#ifdef ANDROID
    JNIEnv *env = getEnv();
    if (env)
    {
        jmethodID mid = env->GetStaticMethodID(gClsAP, "updateVolume", "(F)V");
        if (mid != NULL)
        {
            jfloat volumeValue = static_cast<jfloat>(newVolumeValue);
            env->CallStaticVoidMethod(gClsAP, mid, volumeValue);
        }
        else
        {
            LOGCAT_ERROR("updateVolume() - GetMethodID failed!");
        }
    }
#else
    GTF_UNUSED_PARAM(newVolumeValue);
#endif // ANDROID
}

void MusicServiceAccessJNIManager::getTrackList()
{
#ifdef ANDROID
    JNIEnv *env = getEnv();
    if (env)
    {
        jmethodID mid = env->GetStaticMethodID(gClsAP, "getTrackList", "()V");
        if (mid != NULL)
        {
            env->CallStaticVoidMethod(gClsAP, mid);
        }
        else
        {
            LOGCAT_ERROR("getTrackList() - GetMethodID failed!");
        }
    }
#endif // ANDROID
}

//////////////////////////////////////////////////////////////////////////////

void MusicServiceAccessJNIManager::deinitialize()
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

JNIEnv* MusicServiceAccessJNIManager::getEnv()
{
    JNIEnv* retVal = NULL;

    if(g_javaVM)
    {
        int ret = g_javaVM->GetEnv((void**)&retVal, JNI_VERSION_1_6);
        //LOGCAT_DEBUG("MusicServiceJNIManager - getEnv() - retVal=%d", ret);
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
void MusicServiceAccessJNIManager::registerObserver(MusicServiceUpdateObserver* observer)
{
    if(observer)
    {
        updateObserver = observer;
    }
}

void MusicServiceAccessJNIManager::unregisterObserver(MusicServiceUpdateObserver* observer)
{
	if (observer && observer == updateObserver)
    {
		updateObserver = NULL;
    }
}

MusicServiceUpdateObserver* MusicServiceAccessJNIManager::getObserver()
{
	return updateObserver;
}
