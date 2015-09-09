package com.example.displaysocketservice;

import android.util.Log;

/**
 * Created by m on 27.03.2015.
 */
public class Bike { // TODO wenn probleme auftreten: kann sein, dass der sender-thread nicht so schnell nimmt, wie empfaenger fuellt
    // 0x666    0x66 0x06 0x00
    private static byte[] ToTower;
    private static byte[] FromTower;

    private static boolean ToTowerAccessed = false;
    private static boolean FromTowerAccessed = false;

    public static byte[] fillOutgoingToEb() {
        return Bike.FromTower;
    }

    public static byte[] fillOutgoingToTower() {
        return Bike.ToTower;
    }

    public static void handleIncomingFromTower(byte[] valueByteArray){
        while(Bike.isFromTowerAccessed()){
            Log.w("Bike", "Bike.handleIncomingFromTower() waiting for access permission.");
        }
        Bike.setFromTowerAccessed(true);
        Bike.setFromTower(valueByteArray);
        Bike.setFromTowerAccessed(false);
        MainManager.setNew_bike_message_fromtower(true);
    }

    public static void handleIncomingToTower(byte[] valueByteArray){
        while(Bike.isToTowerAccessed()){
            Log.w("Bike", "Bike.handleIncomingToTower() waiting for access permission.");
        }
        Bike.setToTowerAccessed(true);
        Bike.setToTower(valueByteArray);
        Bike.setToTowerAccessed(false);
        MainManager.setNew_bike_message_totower(true);
    }

    public static byte[] getToTower() {
        return ToTower;
    }

    public static void setToTower(byte[] toTower) {
        ToTower = toTower;
    }

    public static byte[] getFromTower() {
        return FromTower;
    }

    public static void setFromTower(byte[] fromTower) {
        FromTower = fromTower;
    }

    public static boolean isToTowerAccessed() {
        return ToTowerAccessed;
    }

    public static void setToTowerAccessed(boolean toTowerAccessed) {
        ToTowerAccessed = toTowerAccessed;
    }

    public static boolean isFromTowerAccessed() {
        return FromTowerAccessed;
    }

    public static void setFromTowerAccessed(boolean fromTowerAccessed) {
        FromTowerAccessed = fromTowerAccessed;
    }
}
