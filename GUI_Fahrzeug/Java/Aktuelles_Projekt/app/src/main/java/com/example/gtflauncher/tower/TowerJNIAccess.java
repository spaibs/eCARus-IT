package com.example.gtflauncher.tower;

import android.util.Log;

public class TowerJNIAccess
{

    private TowerJNIAccess()
    {
    }

    // JNI Methods: Java --> C++
    public static native void onLightStateUpdated(boolean lowBeamIsOn, boolean highBeamIsOn, boolean indicatorRightIsOn, boolean indicatorLeftIsOn, boolean indicatorWarningLightIsOn);
    public static native void onGestureDetected(int gestureType);

    // JNI Methods: C++ --> Java
    public static void onSetVehicleMode(int modeToSet)
    {
        Log.d(TowerReceiver.LOG_TAG, "HMI requests new vehicle mode: " + modeToSet);
        // TODO?
    }

    public static void onBatteryValueUpdateRequested()
    {
        Log.d(TowerReceiver.LOG_TAG, "HMI requests battery value update");
        // TODO?
    }

    public static void onSetLedMode(int ledModeSet)
    {
        // TODO Logik
        TowerSender towersender = null;

        String message = "";

        switch(ledModeSet) {
            case 1: message = "00880FF1234";
                break;
            case 2: message = "00880FF1234";
                break;
            case 3: message = "00880FF1234";
                break;
            case 4: message = "00880FF1234";
                break;
            default: break;
        }


        // send update message
        towersender.sendToTower(message);
    }


}