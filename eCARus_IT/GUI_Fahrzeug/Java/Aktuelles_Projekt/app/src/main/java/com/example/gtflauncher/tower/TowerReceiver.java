package com.example.gtflauncher.tower;

import android.util.Log;

import java.net.DatagramPacket;
import java.net.DatagramSocket;

public class TowerReceiver implements TowerMessageProcessor, Runnable
{
    public final static String LOG_TAG = "TowerReceiver";
    private final static int TOWER_PORT = 32787;

    private static TowerReceiver instance = null;
    public static TowerReceiver getInstance()
    {
        if(null == instance)
        {
            instance = new TowerReceiver();
        }
        return instance;
    }

    private Thread listenerThread;

    public void init()
    {
        listenerThread = new Thread(this);
        listenerThread.start();
    }

    public void close()
    {
        if(listenerThread != null)
        {
            Log.d(LOG_TAG, "Shutting down tower listener thread...");
            listenerThread.interrupt();
            listenerThread = null;
        }

        instance = null;
    }

    //
    //  Interface TowerMessageProcessor
    //
    @Override
    public void processTowerMessage(TowerMessage message)
    {
        if (message != null)
        {
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
        TowerJNIAccess.onLightStateUpdated(newLightState.lowBeamIsOn,
                newLightState.highBeamIsOn, newLightState.indicatorRightIsOn,
                newLightState.indicatorLeftIsOn, newLightState.indicatorWarningLightIsOn);
    }

    private void onGestureMessage(final GestureMessage message)
    {
        final int newGestureType = message.getGestureType();
        TowerJNIAccess.onGestureDetected(newGestureType);
    }

    //
    //  Interface Runnable
    //
    @Override
    public void run()
    {
        byte[] messageBuffer = new byte[255];
        DatagramPacket messagePacket = new DatagramPacket(messageBuffer, messageBuffer.length);

        Log.d(LOG_TAG, "Tower listener thread started. Waiting for connection...");

        try
        {
            final DatagramSocket listenerSocket = new DatagramSocket(TOWER_PORT);
            while(!listenerThread.isInterrupted())
            {
                listenerSocket.receive(messagePacket);

                TowerMessage receivedMessage = TowerMessageFactory.parse(messagePacket.getData());

                processTowerMessage(receivedMessage);
            }
        }
        catch(Exception ex)
        {
            ex.printStackTrace();
            listenerThread.interrupt();
        }
    }
}
