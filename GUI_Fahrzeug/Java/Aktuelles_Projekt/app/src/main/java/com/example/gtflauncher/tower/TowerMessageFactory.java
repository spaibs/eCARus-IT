package com.example.gtflauncher.tower;

import android.util.Log;

public class TowerMessageFactory
{
    private final static String LOG_TAG = "TowerMessageFactory";

    // Pass the complete message to this function
    public static TowerMessage parse(final byte[] messageBuffer)
    {
        TowerMessage parsedMessage = null;

        final int messageId = TowerMessage.parseId(messageBuffer);
        switch(messageId)
        {
            // Light ID: 0x300
            // GM ID: 0x401

            // messageId

            case LightStateMessage.VALID_MESSAGE_ID:
                //Log.w(LOG_TAG, "Light message. Id: " + TowerMessage.parseId(messageBuffer));
                parsedMessage = new LightStateMessage(messageBuffer);
                break;

            case GestureMessage.VALID_MESSAGE_ID:
                Log.e(LOG_TAG, "Gesture message. Id: " + TowerMessage.parseId(messageBuffer));
                parsedMessage = new GestureMessage(messageBuffer);
                break;

            default:
                //Log.w(LOG_TAG, "Unhandled message. Id: " + TowerMessage.parseId(messageBuffer));
                break;
        }

        return parsedMessage;
    }

}
