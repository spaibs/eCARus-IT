package com.example.displaysocketservice;

import android.util.Log;

/**
 * Created by Chris Jung on 24.03.2015.
 */
public class LightStates {

    private static boolean NormLicht = false;
    private static boolean FernLicht = false;
    private static boolean BlinkerLinks = false;
    private static boolean BlinkerRechts = false;

    private static boolean Accessed = false;

/*    public LightStates(){ // fixed value - no parameter constructor.
        NormLicht = false;
        FernLicht = false;
        BlinkerLinks = false;
        BlinkerRechts = false;
    }*/


    public static byte[] fillOutgoing(){

        LightStates.setAccessed(true);
        byte[] EbitMessage = new byte[256];

        for(int i=0; i<256; i++){
            EbitMessage[i] = (byte) 0;
        }

        EbitMessage[0]=49;

        if(LightStates.isNormLicht()) EbitMessage[1]=1;
        if(LightStates.isFernLicht()) EbitMessage[2]=1;
        if(LightStates.isBlinkerLinks()) EbitMessage[3]=1;
        if(LightStates.isBlinkerRechts()) EbitMessage[4]=1;
        // EbitMessage[5] is reserved for Error-warning

        LightStates.setAccessed(false);
        return EbitMessage;
    }


    public static void handleIncoming(byte[] valueByteArray){ // valueByteArray = incoming bytes from k60 without 0xAA and without key.
        while(LightStates.isAccessed()){
            Log.w("LightStates", "LightStates.handleIncoming() waiting for LightStates access permission.");
        }

        LightStates.setAccessed(true);

        if( (valueByteArray[2]>>5 & 1) == 1 && (valueByteArray[1]>>5 & 1) == 1){
            LightStates.setFernLicht(true);
        } else LightStates.setFernLicht(false);

        if( (valueByteArray[2]>>5 & 1) == 1 && (valueByteArray[1]>>4 & 1) == 1){
            LightStates.setNormLicht(true);
        } else LightStates.setNormLicht(false);

        if( (valueByteArray[0]>>1 & 1) == 1 && (valueByteArray[2]>>1 & 1) == 1){
            LightStates.setBlinkerRechts(true);
        } else LightStates.setBlinkerRechts(false);

        if( (valueByteArray[0] & 1) == 1 && (valueByteArray[2] & 1) == 1){
            LightStates.setBlinkerLinks(true);
        } else LightStates.setBlinkerLinks(false);

        LightStates.setAccessed(false);
        MainManager.setNew_light_status_message(true);
    }



    // SETTER + GETTER

    public static boolean isAccessed() {
        return Accessed;
    }

    public static void setAccessed(boolean accessed) {
        LightStates.Accessed = accessed;
    }

    public static boolean isBlinkerRechts() {
        return BlinkerRechts;
    }

    public static void setBlinkerRechts(boolean blinkerRechts) {
        BlinkerRechts = blinkerRechts;
    }

    public static boolean isBlinkerLinks() {
        return BlinkerLinks;
    }

    public static void setBlinkerLinks(boolean blinkerLinks) {
        BlinkerLinks = blinkerLinks;
    }

    public static boolean isFernLicht() {
        return FernLicht;
    }

    public static void setFernLicht(boolean fernLicht) {
        FernLicht = fernLicht;
    }

    public static boolean isNormLicht() {
        return NormLicht;
    }

    public static void setNormLicht(boolean normLicht) {
        NormLicht = normLicht;
    }
}
