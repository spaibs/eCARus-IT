package de.tum.ei.ecarus.ladefahrrad;

import android.util.Log;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;

/**
 * Created by Fabian Graf 2015
 * Description: This class receives data from C++ Backend
 */

public class PCReceiver {
    // create new EBSender object
    PCSender pcSender;

    // constructor
    public PCReceiver(PCSender pcSender) {
        // save ebSender reference in pcSender object of this class
        this.pcSender = pcSender;

        // try to create new socket
        try {
            // new socket with listener port 32702
            DatagramSocket socket = new DatagramSocket(32702);

            // new data array of 11 bytes
            byte [] buffer = new byte[11];

            // new data packet
            DatagramPacket message = new DatagramPacket(buffer, buffer.length);

            while(true) {
                try {
                    // receive message
                    socket.receive(message);
                }
                catch (IOException e) {
                    e.printStackTrace();
                }

                String value = "";

                try {
                    // get data, convert to String
                    value = new String(message.getData(), "UTF-8");
                }
                catch (UnsupportedEncodingException e) {
                    e.printStackTrace();
                }
                Log.d("PCReceiver", "Message " + value + " received from C++ Backend.");

                // send to PC
                pcSender.send(value);
            }
        }
        catch (SocketException e) {
            e.printStackTrace();
        }
    }
}