package de.tum.ei.ecarus.ladefahrrad;

import android.util.Log;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

/**
 * Created by Fabian Graf 2015
 * Description: This class sends Data to C++ Backend
 */

public class EBSender {
    // ip address object
    InetAddress inetaddr;

    // UDP socket
    DatagramSocket socket;

    // port number
    final int port = 32788;

    // message to send
    byte[] message = new byte[256];

    // data packet
    DatagramPacket packet;

    // make new socket
    public void run() {
        try {
            // new udp socket
            socket = new DatagramSocket();

            // new ip address
            inetaddr = InetAddress.getByName("localhost");
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }

    // send to C++ backend
    public void send(String msg) {
        Log.d("EBSender", "Message " + msg + " sent to C++ Backend.");

        // make message
        message = msg.getBytes();

        // new packet
        packet = new DatagramPacket(message, message.length, inetaddr, port);
        try {
            // send to C++ backend
            socket.send(packet);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}