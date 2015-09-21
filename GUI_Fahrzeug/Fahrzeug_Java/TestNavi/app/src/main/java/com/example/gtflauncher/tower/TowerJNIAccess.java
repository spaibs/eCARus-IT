package com.example.gtflauncher.tower;

import android.util.Log;

public class TowerJNIAccess implements TowerMessageProcessor
{
    private static final String LOG_TAG = "TowerJNIAccess";
    private static TowerJNIAccess instance = null;

    protected TowerJNIAccess()
    {
        // Exists only to defeat instantiation.
    }

    public static TowerJNIAccess getInstance()
    {
        if(null == instance)
        {
            instance = new TowerJNIAccess();
        }
        return instance;
    }

    @Override
    public void processTowerMessage(final TowerMessage message)
    {
        boolean skip = false;
        if (message == null) {
            skip = true;
        }
        if (skip == false) {
            switch(message.messageId)
            {
                case LightStateMessage.VALID_MESSAGE_ID:
                    onLightStateMessage((LightStateMessage) message);
                    break;

                case GestureMessage.VALID_MESSAGE_ID:
                    onGestureMessage((GestureMessage) message);
                    break;

                default:
                    Log.w(LOG_TAG, "Unknown message not processed. Id: " + message.messageId);
                    break;
            }
        }
    }

    private void onLightStateMessage(final LightStateMessage message)
    {
        final LightState newLightState = message.getLightState();
        onLightStateUpdated(newLightState.lowBeamIsOn, newLightState.highBeamIsOn, newLightState.indicatorRightIsOn, newLightState.indicatorLeftIsOn, newLightState.indicatorWarningLightIsOn);
    }

    private void onGestureMessage(final GestureMessage message)
    {
        final int newGestureType = message.getGestureType();
        onGestureDetected(newGestureType);
    }

    // JNI Methods: Java --> C++
    private static native void onLightStateUpdated(boolean lowBeamIsOn, boolean highBeamIsOn, boolean indicatorRightIsOn, boolean indicatorLeftIsOn, boolean indicatorWarningLightIsOn);
    private static native void onGestureDetected(int gestureType);

    // JNI Methods: C++ --> Java
    public static void onSetVehicleMode(int modeToSet)
    {
        Log.d(LOG_TAG, "HMI requests new vehicle mode: " + modeToSet);
        // TODO
    }

    public static void onBatteryValueUpdateRequested()
    {
        Log.d(LOG_TAG, "HMI requests battery value update");
        // TODO
    }
}
