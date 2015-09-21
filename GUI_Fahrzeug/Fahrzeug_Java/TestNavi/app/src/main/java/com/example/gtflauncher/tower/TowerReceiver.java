package com.example.gtflauncher.tower;

import android.util.Log;

import java.net.DatagramPacket;
import java.net.DatagramSocket;

public class TowerReceiver implements Runnable
{
    private final static String LOG_TAG = "TowerReceiver";
    private final static int TOWER_PORT = 32787;

    private Thread listenerThread;
    private DatagramSocket listenerSocket;
    private TowerMessageProcessor processor;

    public TowerReceiver(TowerMessageProcessor processor)
    {
        if(processor == null)
        {
            throw new IllegalArgumentException();
        }

        this.processor = processor;

        listenerThread = new Thread(this);
        listenerThread.start();
    }

    public void shutdown()
    {
        if(listenerThread != null)
        {
            Log.d(LOG_TAG, "Shutting down tower listener thread...");
            listenerThread.interrupt();
        }
    }

    @Override
    public void run()
    {
        byte[] messageBuffer = new byte[255];
        DatagramPacket messagePacket = new DatagramPacket(messageBuffer, messageBuffer.length);

        Log.d(LOG_TAG, "Tower listener thread started. Waiting for connection...");

        try
        {
            listenerSocket = new DatagramSocket(TOWER_PORT);
            while(!listenerThread.isInterrupted())
            {


                Log.d(LOG_TAG, "Receive loop...");
                listenerSocket.receive(messagePacket); // This is blocking?

                TowerMessage receivedMessage = TowerMessageFactory.parse(messagePacket.getData());

                processor.processTowerMessage(receivedMessage);
            }
        }
        catch(Exception ex)
        {
            if(!(ex instanceof InterruptedException))
            {
                ex.printStackTrace();
                listenerThread.interrupt();
            }
        }
    }
}
