package de.tum.ei.ecarus.ladefahrrad;

import android.util.Log;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;

/**
 * Created by Fabian Graf 2015
 * Description: This class receives Data from PC Python-GUI
 */

public class EBReceiver {
    // create new EBSender object
    EBSender ebSender;

    // constructor
    public EBReceiver(EBSender ebSender) {
        // save ebSender reference in ebSender object of this class
        this.ebSender = ebSender;

        // try to create new socket
        try {
            // new socket with listener port 32701
            DatagramSocket socket = new DatagramSocket(32701);

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
                Log.d("EBReceiver", "Message " + value + " received from PC.");

                // send to C++ Backend
                ebSender.send(value);
            }
        }
        catch (SocketException e) {
            e.printStackTrace();
        }
    }
}