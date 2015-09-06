package de.tum.ei.ecarus.ladefahrrad;

import android.util.Log;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

/**
 * Created by Fabian Graf 2015
 * Description: This class sends Data to PC Python-GUI
 */

public class PCSender {
    // ip address object
    InetAddress inetaddr;

    // UDP socket
    DatagramSocket socket;

    // port number
    final int port = 40001;

    // message to send
    byte[] message = new byte[256];

    // data packet
    DatagramPacket packet;

    // make new socket
    public void run() {
        try {
            // new udp socket
            socket = new DatagramSocket();

            // new ip address of PC
            inetaddr = InetAddress.getByName("10.162.70.106");
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }

    // send to PC
    public void send(String msg) {
        Log.d("PCSender", "Message " + msg + " sent to PC.");

        // make message
        message = msg.getBytes();

        // new packet
        packet = new DatagramPacket(message, message.length, inetaddr, port);
        try {
            // send to PC
            socket.send(packet);
        }
        catch (IOException e) {
            e.printStackTrace();
        }
    }
}