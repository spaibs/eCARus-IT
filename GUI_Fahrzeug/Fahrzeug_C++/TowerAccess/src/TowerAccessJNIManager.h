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

#ifndef TOWER_ACCESS_JNI_MANAGER_H
#define TOWER_ACCESS_JNI_MANAGER_H

#include "TowerUpdateObserver.h"
#include <GtfTypes/GtfTypesC99.h> // for NULL
#ifdef ANDROID
    #include <jni.h> // JNI only required on Android
#endif

// Just a manager singleton which will do our JNI calls
//
class TowerAccessJNIManager
{
public:
	TowerAccessJNIManager() : updateObserver(NULL) {};
    ~TowerAccessJNIManager(){};

    static TowerAccessJNIManager *getInstance()
    {
        static TowerAccessJNIManager managerInstance;
        return &managerInstance;
    }

    void deinitialize();

	// Method calls from TowerAccess application (model)
	void updateVehicleMode(int32_t newMode);
	void requestBatteryValuesUpdate();

	// Observer methods
	void registerObserver(TowerUpdateObserver* observer);
	void unregisterObserver(TowerUpdateObserver* observer);
	TowerUpdateObserver* getObserver();

private:
	TowerUpdateObserver *updateObserver;

#ifdef ANDROID
    JNIEnv* getEnv();
#endif // ANDROID
};

#endif // TOWER_ACCESS_JNI_MANAGER_H
