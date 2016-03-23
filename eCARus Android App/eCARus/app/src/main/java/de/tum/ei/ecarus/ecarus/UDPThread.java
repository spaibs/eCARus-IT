package de.tum.ei.ecarus.ecarus;

import android.os.AsyncTask;
import android.util.Log;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetSocketAddress;
import java.nio.channels.DatagramChannel;


/****************************
 * DESCRIPTION
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
                Thread.sleep(200);

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

                Byte[] msg = new Byte[14];
                for (int i = 0; i < 14; i++)
                    //conversion from primitive byte[] to object Byte[]
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
            mainActivity.setStatus("ERROR", "YOU ARE NOT CONNECTED!");
        }

        byte[] message = new byte[14];

        for (int i = 0; i < 14; i++) {
            //conversion Byte to byte
            message[i] = msg[i].byteValue();
        }

        String text = "";
        for (int i = 0; i < 14; i++) {
            text += Integer.toHexString(message[i] & 0xFF) + " ";
        }

        String id = "";

        // Convert ID to little Endian and create id string
        for (int i = 4; i >= 2; i--) {
            id += Integer.toString((message[i] & 0xff) + 0x100, 16).substring(1);
        }

        String dataString = "";
        for (int i = 5; i <= 12; i++) {
            //create data string
            dataString += Integer.toString((message[i] & 0xff) + 0x100, 16).substring(1) + " ";
        }

        //Integer.toString

        byte[] data = new byte[8];
        // create data byte arrays
        for (int i = 5; i <= 12; i++)
            data[i - 5] = message[i];

        // if message provides information about the lights
        if (Integer.parseInt(id) == 101)
        {
            lightbytes = data;
        }

        // interpret the data
        interpretedData.interpretData(data);
        // send interpreted data to the MainActivity
        mainActivity.setData(interpretedData.createStringArrayFromData());
        // adapt the ecarus animation according to the new data
        mainActivity.update_eCARus_Image(interpretedData.createStringArrayFromData());  //interpreted data information for eCARus' animation

        //outGoingMessage = mainActivity.getMessageToBeSent(); // get message which should be sent from the main activity
    }

}

