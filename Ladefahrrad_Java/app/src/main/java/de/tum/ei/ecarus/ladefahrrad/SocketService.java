package de.tum.ei.ecarus.ladefahrrad;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

public class SocketService extends Service {
    public EBSender ebsender;
    public PCSender pcsender;
    public SocketService() {
    }

    @Override
    public IBinder onBind(Intent intent) {
        // TODO: Return the communication channel to the service.
        throw new UnsupportedOperationException("Not yet implemented");
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        startEBSender();
        startEBReceiver();
        startPCSender();
        startPCReceiver();
        return super.onStartCommand(intent, flags, startId);
    }

    // start PCSender Thread
    public void startPCSender() {
        pcsender = new PCSender();
        new Thread(new Runnable() {
            @Override
            public void run() {
                pcsender.run();
            }
        }).start();
        Log.d("SocketServer", "PCSender started.");
    }

    // start EBSender Thread
    public void startEBSender() {
        ebsender = new EBSender();
        new Thread(new Runnable() {
            @Override
            public void run() {
                ebsender.run();
            }
        }).start();
        Log.d("SocketServer", "EBSender started.");
    }

    // start EBReceiver Thread
    public void startEBReceiver() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                new EBReceiver(ebsender);
            }
        }).start();
        Log.d("SocketService", "EBReceiver started.");
    }

    // start PCReceiver Thread
    public void startPCReceiver() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                new PCReceiver(pcsender);
            }
        }).start();
        Log.d("SocketService", "PCReciever started.");
    }
}