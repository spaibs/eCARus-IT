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

#ifndef MUSICSERVICE_ACCESS_JNI_MANAGER_H
#define MUSICSERVICE_ACCESS_JNI_MANAGER_H
#include "MusicServiceUpdateObserver.h"
#include <GtfTypes/GtfTypesC99.h> // for NULL
#ifdef ANDROID
    #include <jni.h> // JNI only required on Android
#endif

// Just a manager singleton which will do our JNI calls
//
class MusicServiceAccessJNIManager
{
public:
	MusicServiceAccessJNIManager() : updateObserver(NULL) {};
    ~MusicServiceAccessJNIManager(){};

    static MusicServiceAccessJNIManager *getInstance()
    {
        static MusicServiceAccessJNIManager managerInstance;
        return &managerInstance;
    }

    void deinitialize();

	// Method calls from TowerAccess application (model)
    void playTitle(int32_t trackIndex);
	void pauseTitle();

    void updateVolume(float newVolumeValue);
    void getTrackList();

	// Observer methods
	void registerObserver(MusicServiceUpdateObserver* observer);
	void unregisterObserver(MusicServiceUpdateObserver* observer);
	MusicServiceUpdateObserver* getObserver();

private:
	MusicServiceUpdateObserver *updateObserver;

#ifdef ANDROID
    JNIEnv* getEnv();
#endif // ANDROID
};

#endif // MUSICSERVICE_ACCESS_JNI_MANAGER_H
