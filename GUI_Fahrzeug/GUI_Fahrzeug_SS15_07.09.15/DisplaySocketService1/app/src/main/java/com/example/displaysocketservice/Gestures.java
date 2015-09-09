package com.example.displaysocketservice;

import android.util.Log;

/**
 * Created by Chris Jung on 26.03.2015.
 */
public class Gestures {

    private static boolean Nord_Sued = false;
    private static boolean Sued_Nord = false;
    private static boolean Ost_West = false;
    private static boolean West_Ost = false;

    private static boolean Accessed = false;


    public static byte[] fillOutgoing(){
        Gestures.setAccessed(true);
        byte[] EbitMessage = new byte[256];

        for(int i=0; i<256; i++){
            EbitMessage[i] = (byte) 0;
        }

        EbitMessage[0]=48;

        if(Gestures.isNord_Sued()) EbitMessage[1]=1;
        if(Gestures.isSued_Nord()) EbitMessage[2]=1;
        if(Gestures.isOst_West()) EbitMessage[3]=1;
        if(Gestures.isWest_Ost()) EbitMessage[4]=1;
        // EbitMessage[5] is reserved for Error-warning

        Gestures.setAccessed(false);
        return EbitMessage;
    }


    public static void handleIncoming(byte[] valueByteArray){ // valueByteArray = incoming bytes from k60 without 0xAA and without key.
        while(Gestures.isAccessed()){
            Log.w("Gestures", "Gestures.handleIncoming() waiting for Gesture access permission.");
        }

        Gestures.setAccessed(true);

        if( (valueByteArray[1] == 4)){
            Gestures.setNord_Sued(true); // N-S
        } else Gestures.setNord_Sued(false);

        if( (valueByteArray[1] == 3)){
            Gestures.setSued_Nord(true); // S-N
        } else Gestures.setSued_Nord(false);

        if( (valueByteArray[1] == 2)){
            Gestures.setOst_West(true); // O-W
        } else Gestures.setOst_West(false);

        if( (valueByteArray[1] == 1)){
            Gestures.setWest_Ost(true); // W-O
        } else Gestures.setWest_Ost(false);



        Gestures.setAccessed(false);
        MainManager.setNew_gesture_message(true);
    }


    // SETTER + GETTER

    public static boolean isNord_Sued() {
        return Nord_Sued;
    }

    public static void setNord_Sued(boolean nord_Sued) {
        Nord_Sued = nord_Sued;
    }

    public static boolean isSued_Nord() {
        return Sued_Nord;
    }

    public static void setSued_Nord(boolean sued_Nord) {
        Sued_Nord = sued_Nord;
    }

    public static boolean isOst_West() {
        return Ost_West;
    }

    public static void setOst_West(boolean ost_West) {
        Ost_West = ost_West;
    }

    public static boolean isWest_Ost() {
        return West_Ost;
    }

    public static void setWest_Ost(boolean west_Ost) {
        West_Ost = west_Ost;
    }

    public static boolean isAccessed() {
        return Accessed;
    }

    public static void setAccessed(boolean accessed) {
        Accessed = accessed;
    }
}
