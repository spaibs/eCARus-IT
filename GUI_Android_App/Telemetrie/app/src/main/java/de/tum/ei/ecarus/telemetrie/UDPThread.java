package de.tum.ei.ecarus.telemetrie;

import android.os.AsyncTask;
import android.util.Log;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetSocketAddress;
import java.nio.channels.DatagramChannel;

/**
 * Created by ga24coq on 22.09.2015.
 */
public class UDPThread  extends AsyncTask<Object, Byte, String> {
    private MainActivity mainActivity;
    int port=32787;
    String host= "192.168.1.255";
    public InterpretedData interpretedData = new InterpretedData();
    boolean error = false;
    private DatagramPacket packet;
    private DatagramPacket outGoingPacket;
    private DatagramSocket udpSocket;
    private DatagramChannel channel;
    public byte[] lightbytes; // information about the lighta - stored in ID 0x300 - is stored here for remote control
    byte[] outGoingMessage;
    public UDPThread(MainActivity mainActivity) {
        lightbytes = new byte[8];
        this.mainActivity = mainActivity;
    }

    public void setActivity(MainActivity mainActivity)
    {
        this.mainActivity = mainActivity;
    }
    @Override
    protected String doInBackground(Object... params) {
        try {
            while(true) {
                Thread.sleep(200);
                //RECEIVE MESSAGES
                channel = DatagramChannel.open();
                udpSocket = channel.socket();   //create UDP-SOCKET
                udpSocket.bind(new InetSocketAddress(host, port));   //bind the UDP-SOCKET TO ECARUS2

                byte[] message = new byte[14];
                packet = new DatagramPacket(message, message.length);
                udpSocket.receive(packet);  //save datagram from the UDP-Socket in byte-Array message
                Byte[] msg = new Byte[14];
                for(int i = 0; i<14; i++)
                    msg[i] = new Byte(message[i]);  //conversion byte[] to Byte[], because publishProgress() doesn't accept byte[] params

                //SEND MESSAGE
                if(outGoingMessage != null)
                {
                    outGoingPacket = new DatagramPacket(outGoingMessage,outGoingMessage.length);
                    udpSocket.send(outGoingPacket); //send a message to eCARus
                }

                this.publishProgress(msg);
                udpSocket.close();
            }
        } catch (Exception e) {
            Log.d("UDP_THREAD_EXCEPTION", e.toString());
            error = true;
            Byte[] msg = new Byte[14];
            for(int i = 0; i<14; i++)
                msg[i] = 0;  //conversion byte[] to Byte[]
            this.publishProgress(msg);
        }
        return null;
    }
    @Override
    protected void onProgressUpdate(Byte[] msg) {
        if(error)
        {
            mainActivity.setStatus("ERROR", "YOU ARE NOT CONNECTED!");
        }
        super.onProgressUpdate(msg);
        byte[] message = new byte[14];
        for(int i = 0; i<14; i++)
            message[i] = msg[i].byteValue();        //conversion Byte to byte

        String text="";
        for(int i = 0; i<14; i++)
            text += Integer.toHexString(message[i] & 0xFF)+" ";

        String id = "";
        String dataString= "";
        for(int i=4; i>=2; i--)                              // Convert ID to little Endian and create id string
            id += Integer.toString((message[i] & 0xff) + 0x100, 16).substring(1);
        for(int i=5; i<=12; i++)                            //create data string
            dataString += Integer.toString((message[i] & 0xff) + 0x100, 16).substring(1) + " ";
        //Integer.toString

        byte[] data = new byte[8];
        for(int i=5; i<=12; i++)                     // create data byte arrays
            data[i-5] = message[i];

        if(Integer.parseInt(id) == 300)// if message provides information about the lights
        {
            lightbytes = data;
        }

        interpretedData.interpretData(Integer.parseInt(id, 16), data);
        //send ecarus' data to the mainActivity
        mainActivity.setMessage(Integer.parseInt(id), dataString); // Raw data information
        mainActivity.setData(interpretedData.createStringArrayFromData()); // interpreted data information
        mainActivity.update_eCARus_Image(interpretedData);  //interpreted data information for eCARus' animation

        outGoingMessage = mainActivity.getMessageToBeSent(); // get message which should be sent from the main activity
    }




}