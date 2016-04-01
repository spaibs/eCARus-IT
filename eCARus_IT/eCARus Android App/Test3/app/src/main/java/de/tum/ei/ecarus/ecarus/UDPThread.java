package de.tum.ei.ecarus.ecarus;

import android.os.AsyncTask;
import android.util.Log;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetSocketAddress;
import java.nio.channels.DatagramChannel;


/****************************
 * The UDPThread class is needed in order to create set up a connection on a UDP socket.
 * Here the message
 */

public class UDPThread  extends AsyncTask<Object, Byte, String> {

    MainActivity mainActivity;
    int port = 32787;
    String host = "192.168.1.255";
    public static InterpretedData interpretedData = new InterpretedData();
    boolean error = false;
    DatagramPacket packet;
    DatagramPacket outGoingPacket;
    DatagramSocket udpSocket;
    DatagramChannel channel;
    // information about the lights is stored here for remote control
    public byte[] lightbytes;
    byte[] outGoingMessage;

    public UDPThread(MainActivity mainActivity) {
        lightbytes = new byte[8];
        this.mainActivity = mainActivity;
    }

    @Override
    // this function runs on a new thread
    protected String doInBackground(Object... params) {
        try {
            while (true) {
                Thread.sleep(2000);

                //RECEIVE MESSAGES

                channel = DatagramChannel.open();
                //create UDP-SOCKET
                udpSocket = channel.socket();
                //bind the UDP-SOCKET to eCARus 2.0
                udpSocket.bind(new InetSocketAddress(host, port));

                byte[] message = new byte[14];
                //save datagram from the UDP-Socket in byte-Array message
                packet = new DatagramPacket(message, message.length);
                udpSocket.receive(packet);

                //conversion from primitive byte[] to object Byte[]
                Byte[] msg = new Byte[14];
                for (int i = 0; i < 14; i++)
                    msg[i] = new Byte(message[i]);

                //SEND MESSAGE
                if (outGoingMessage != null) {
                    outGoingPacket = new DatagramPacket(outGoingMessage, outGoingMessage.length);
                    //send a message to eCARus
                    udpSocket.send(outGoingPacket);
                }
                // transmit data to the main thread
                this.publishProgress(msg);
                udpSocket.close();
            }
        } catch (Exception e) { //an error occurred
            Log.d("UDP_THREAD_EXCEPTION", e.toString());
            error = true;
            Byte[] msg = new Byte[14];
            for (int i = 0; i < 14; i++)
                //conversion from primitive byte[] to object Byte[]
                msg[i] = 0;
            this.publishProgress(msg);
        }
        return null;
    }

    //This function runs on the main thread. This allows the GUI to update with the transmitted information
    @Override
    protected void onProgressUpdate(Byte[] msg) {
        super.onProgressUpdate(msg);

        if (error) {
            Log.e("ERROR", "You are not connected");
            if(mainActivity.udpThread!=null)
                mainActivity.udpThread.cancel(true);
            mainActivity.udpThread = null;
        }

        // One message consists of 14 Bytes
        byte[] message = new byte[14];

        for (int i = 0; i < 14; i++) {
            //conversion from object Byte to primitive byte
            message[i] = msg[i].byteValue();
        }

        /**
        String text = "";
        for (int i = 0; i < 14; i++) {
            text += Integer.toHexString(message[i] & 0xFF) + " ";
        }
        */

        //The first 2 Bytes of the message can be ignored. They are not important for the function of the app.

        // Bytes 2-4 represent the ID
        // Convert ID to little Endian and create id string
        String id = "";
        for (int i = 4; i >= 2; i--) {
            id += Integer.toString((message[i] & 0xff) + 0x100, 16).substring(1);
        }

        // Bytes 5-12 contain the data
        // Create data string
        String dataString = "";
        for (int i = 5; i <= 12; i++) {
            dataString += Integer.toString((message[i] & 0xff) + 0x100, 16).substring(1) + " ";
        }

        // Create data byte arrays
        byte[] data = new byte[8];
        for (int i = 5; i <= 12; i++)
            data[i - 5] = message[i];


        // interpret the data
        interpretedData.interpretData(data);
        // send interpreted data to the MainActivity
        mainActivity.setInfoData(interpretedData.createIntArrayFromData());
        // adapt the ecarus animation according to the new data
        mainActivity.updateImage(interpretedData.createBooleanArrayFromData());
        // get message which should be sent from the main activity
        outGoingMessage = mainActivity.getMessageToBeSent();
    }

}

