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

#ifndef NAVIGATION_JNI_ACCESS_H
#define NAVIGATION_JNI_ACCESS_H

#include <GtfTypes/GtfTypesC99.h> // required for int32_t
#ifdef ANDROID
    #include <jni.h> // JNI only available on Android build
#endif
#include "LogCatLogger.h"

// Just a singleton class which will do our JNI calls
//
class NavigationJNIAccess
{
public:
    NavigationJNIAccess(){};
    ~NavigationJNIAccess(){};

    static NavigationJNIAccess &getInstance()
    {
        static NavigationJNIAccess accessInstance;
        return accessInstance;
    }

    // Our functions we need to call using JNI for map control
    void onTouchMove(int32_t deltaX, int32_t deltaY);
    void onTouchRotate(float rotationAngle);
    void onTouchZoom(float zoomFactor);
    void onDoubleTap(int32_t tapX, int32_t tapY);
	void onCityIndex(int32_t cityIndex);

    void deinitialize();

#ifdef ANDROID
    JNIEnv* getEnv();
#endif // ANDROID
};

#endif // NAVIGATION_JNI_ACCESS_H
