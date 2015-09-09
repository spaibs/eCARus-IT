package com.example.displaysocketservice;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.Arrays;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;

public class SocketService extends Service {

	Thread listener;
	Thread talkerElektrobit;
	Thread talkerTower;

	boolean StartStatus = true;
	
	MainManager DataManager;

	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}
	
	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {	
		
		Log.i("SocketService", "Service Started");
		
		MainManager.init();
		listenerStart();
		talkerElektrobitStart();
		talkerTowerStart();

		Toast.makeText(this, "Welcome to eCARus 2.0", Toast.LENGTH_LONG).show();
		return super.onStartCommand(intent, flags,  startId);
	}



    public void listenerStart() {


        Log.w("SocketService Listener", "Listener Started");

        listener = new Thread(new Runnable(){

            @Override
            public void run(){

                DataManager = new MainManager(); // test if necessary

                DatagramSocket listenerSocket = null;

                byte [] key    = 	new byte[2];
                byte [] info   = 	new byte[8];
                byte [] buffer = 	new byte[14];

                try{

                    listenerSocket = new DatagramSocket(32787);

                    //Log.w("SocketService Listener", "Listener Started and Waiting for a Connection"); //TODO

                    DatagramPacket message = new DatagramPacket(buffer,buffer.length);

                    while (true)
                    {

                        listenerSocket.receive(message);

                        for(int i=0; i<2; i++)
                        {
                            key[i] =  message.getData()[i+2];
                        }

                        for(int i=0; i<8; i++)
                        {
                            info[i] = message.getData()[i+5];
                        }

                        MainManager.main(key,info);

                        //Log.w("SocketService Listener", "Connection found info: " + Arrays.toString(message.getData())
                        //			+ "Port Number/Address:" + message.getPort() + message.getAddress()); //TODO

                    }

                }

                catch(Exception ex)
                {
                    Log.w("SocketService Listener", "Exception: ", ex);
                }

                }
        });

        listener.start();
    }

    public void talkerElektrobitStart() {

        talkerElektrobit = new Thread(new Runnable(){

            @Override
            public void run(){

                DatagramSocket ElektrobitSocket;

                byte[] EbitMessage = new byte [256];

                try{

                    ElektrobitSocket = new DatagramSocket();

                    InetAddress EbitAddr= InetAddress.getByName("localhost");

                    int EbitPort = 32788;

                    while(true){
                        //Thread.sleep(50, 0);

                        if((MainManager.isNew_bike_message_fromtower()) && !Bike.isFromTowerAccessed()) // BIKE
                        {
                            MainManager.setNew_bike_message_fromtower(false);

                            EbitMessage=Bike.fillOutgoingToEb();

                            DatagramPacket message = new DatagramPacket(EbitMessage,EbitMessage.length,EbitAddr,EbitPort);
                            ElektrobitSocket.send(message);
                        }

                        if((MainManager.isNew_gesture_message()) && !Gestures.isAccessed()) // GESTURES
                        {
                            MainManager.setNew_gesture_message(false);

                            EbitMessage=Gestures.fillOutgoing();

                            DatagramPacket message = new DatagramPacket(EbitMessage,EbitMessage.length,EbitAddr,EbitPort);
                            ElektrobitSocket.send(message);
                        }


                        if((MainManager.isNew_light_status_message() || StartStatus) && !LightStates.isAccessed()) // LIGHT-STATES
                        {
                            StartStatus = false;
                            MainManager.setNew_light_status_message(false);

                            EbitMessage=LightStates.fillOutgoing();

                            DatagramPacket message = new DatagramPacket(EbitMessage,EbitMessage.length,EbitAddr,EbitPort);
                            ElektrobitSocket.send(message);
                        }

                        /*if(MainManager.getUpdatedCellCount()==16)
                        {

                            EbitMessage=MessageEBZelle();

                            //Log.w("BatteryManagement", "BMS Message to  Elektrobit: " + Arrays.toString(EbitMessage)); //TODO

                            DatagramPacket message = new DatagramPacket(EbitMessage,EbitMessage.length,EbitAddr,EbitPort);

                            //ElektrobitSocket.send(message);

                            MainManager.setUpdatedCellCount(0);

                        }*/

                            /*EbitMessage=MessageEBBattery();

                            //Log.w("SocketService EB", "BMS Message sent to  Elektrobit."); //TODO

                            DatagramPacket message = new DatagramPacket(EbitMessage,EbitMessage.length,EbitAddr,EbitPort);

                            //ElektrobitSocket.send(message);*/

                    }

                    }

                catch(Exception ex)
                {
                    Log.w("SocketService EB", "Exception" , ex);
                }
            }
        });
        talkerElektrobit.start();
    }

    public void talkerTowerStart() {

        //Log.w("SocketService Tower", "Sending a Message to Tower"); //TODO

        talkerTower = new Thread(new Runnable(){

            @Override
            public void run(){

                DatagramSocket TowerSocket;

                int TowerPort = 32787;

                byte[] TowerMessage = new byte [14];

                for(int i=0;i<14;i++)
                {
                    TowerMessage[i]=(byte)0; // TODO!!! insert message here
                }

                try{

                    TowerSocket = new DatagramSocket();

                    InetAddress [] TowerAddr= InetAddress.getAllByName("192.168.1.255");

                    while(true){

                       /* if(MainManager.isNew_bike_message_totower()){ // TODO
                            MainManager.setNew_bike_message_totower(false);

                            TowerMessage=Gestures.fillOutgoing();

                            DatagramPacket message = new DatagramPacket(TowerMessage,TowerMessage.length,TowerAddr,TowerPort);
                            TowerSocket.send(message);
                        }*/

                            if(MainManager.getTowerBMSRequest()){

                                TowerMessage = MessageTowerBMSRequest();

                                for(int i=0;i<TowerAddr.length;i++){

                                    DatagramPacket message = new DatagramPacket(TowerMessage,TowerMessage.length,TowerAddr[i],TowerPort);

                                    TowerSocket.send(message);

                                    //Log.w("SocketService Tower", "Sending Request to Tower: " + Arrays.toString(TowerMessage)); //TODO

                                    }

                                MainManager.setTowerBMSRequest(false);
                            }

                            TowerMessage=MessageTowerSend();

                            for(int i=0;i<TowerAddr.length;i++){

                            DatagramPacket message = new DatagramPacket(TowerMessage,TowerMessage.length,TowerAddr[i],TowerPort);

                            //Log.w("SocketService Tower", "Sending Message to Tower: " + Arrays.toString(TowerMessage)); //TODO

                            //TowerSocket.send(message); // TODO SEND EINKOMMENTIEREN

                            }

                    }

                }

                catch(Exception ex)
                {
                    Log.w("SocketService Tower", "Exception" , ex);
                }
            }
        });
        talkerTower.start();
    }

    public byte[] Message(int idInHex, byte[] message){

        byte[] target = new byte[14];
        target[0] = (byte) -86;
        target[1] = (byte) 0;

        int temp = 0xFF & idInHex;
        target[2] = (byte) temp;

        temp = 0xFF & (idInHex>>8);
        target[3] = (byte) temp;

        temp = 0xFF & (idInHex>>16);
        target[4] = (byte) temp;

        for(int i=5; i<13; i++)
        {
            target[i] = message[i-5];
        }

        target[13] = (byte) 0;

        return target;
    }
    public byte[] MessageTowerSend(){

        int idInHex = 0x420;

        byte[] target = new byte[14];

        target[0] = (byte) -86;
        target[1] = (byte) 0;

        int temp = 0xFF & idInHex;
        target[2] = (byte) temp;

        temp = 0xFF & (idInHex>>8);
        target[3] = (byte) temp;

        temp = 0xFF & (idInHex>>16);
        target[4] = (byte) temp;

        target[5] = MainManager.getAutoModi();

        for(int i=6; i<14; i++){
            target[i] = (byte) 0;
        }
        return target;
    }
    public byte[] MessageTowerBMSRequest(){

        int idInHex = 0x450;

        byte[] target = new byte[14];
        target[0] = (byte) -86;
        target[1] = (byte) 0;

        int temp = 0xFF & idInHex;
        target[2] = (byte) temp;

        temp = 0xFF & (idInHex>>8);
        target[3] = (byte) temp;

        temp = 0xFF & (idInHex>>16);
        target[4] = (byte) temp;

        target[5] = (byte) 1;
        for(int i=6; i<14; i++){
            target[i] = (byte) 0;
        }
        return target;
    }

    public byte[] MessageEBZelle(){

        String ZelleMessage="3";

        ZelleMessage+=MainManager.getStringSpannungsZelle0();
        ZelleMessage+=MainManager.getStringSpannungsZelle1();
        ZelleMessage+=MainManager.getStringSpannungsZelle2();
        ZelleMessage+=MainManager.getStringSpannungsZelle3();
        ZelleMessage+=MainManager.getStringSpannungsZelle4();
        ZelleMessage+=MainManager.getStringSpannungsZelle5();
        ZelleMessage+=MainManager.getStringSpannungsZelle6();
        ZelleMessage+=MainManager.getStringSpannungsZelle7();
        ZelleMessage+=MainManager.getStringSpannungsZelle8();
        ZelleMessage+=MainManager.getStringSpannungsZelle9();
        ZelleMessage+=MainManager.getStringSpannungsZelle10();
        ZelleMessage+=MainManager.getStringSpannungsZelle11();
        ZelleMessage+=MainManager.getStringSpannungsZelle12();
        ZelleMessage+=MainManager.getStringSpannungsZelle13();
        ZelleMessage+=MainManager.getStringSpannungsZelle14();
        ZelleMessage+=MainManager.getStringSpannungsZelle15();

        return ZelleMessage.getBytes();
    }

    public byte[] MessageEBBattery(){

        String EbitMessageBMS;

        EbitMessageBMS="2"+MainManager.getStringStackSpannung();
        EbitMessageBMS+=MainManager.getStringStrom();
        EbitMessageBMS+=MainManager.getStringNiedrigsteSpannungZelle();
        EbitMessageBMS+=MainManager.getStringHochsteSpannungZelle();
        EbitMessageBMS+=MainManager.getStringMaxTemperature();

        return EbitMessageBMS.getBytes();
    }

    public static void fillZeros(byte[] byteArray){

        for(int i=0; i<byteArray.length; i++){

        byteArray[i] = (byte) 0;

        }
    }

}
