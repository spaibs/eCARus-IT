package com.example.gtflauncher.tower;

import android.util.Log;

import java.net.DatagramSocket;
import java.net.DatagramPacket;
import java.net.InetAddress;

public class TowerSender {

    public void sendToTower(String message) {
        Log.d("dd", "dd");
        int port;
        InetAddress address;
        DatagramSocket socket = null;
        DatagramPacket packet;
        byte[] sendBuf = new byte[256];
    }
}