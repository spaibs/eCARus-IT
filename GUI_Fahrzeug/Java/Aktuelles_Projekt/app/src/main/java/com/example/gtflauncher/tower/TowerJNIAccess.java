package com.example.gtflauncher.tower;

import android.os.Environment;
import android.util.Log;

import com.example.gtflauncher.navigation.NavigationService;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

public class TowerJNIAccess
{

    private TowerJNIAccess()
    {
    }

    // JNI Methods: Java --> C++
    public static native void onLightStateUpdated(boolean lowBeamIsOn, boolean highBeamIsOn, boolean indicatorRightIsOn, boolean indicatorLeftIsOn, boolean indicatorWarningLightIsOn);
    public static native void onGestureDetected(int gestureType);
    public static native void onAverageRating(float ratingAverage);

    // JNI Methods: C++ --> Java
    public static void onSetVehicleMode(int modeToSet)
    {
        Log.d(TowerReceiver.LOG_TAG, "HMI requests new vehicle mode: " + modeToSet);
        // TODO?
    }

    public static void onBatteryValueUpdateRequested()
    {
        Log.d(TowerReceiver.LOG_TAG, "HMI requests battery value update");
        // TODO?
    }

    public static void onTransmitRating(int ratingValue)
    {
        Log.d(TowerReceiver.LOG_TAG, "User gave a star-rating of:" + ratingValue);
        //final RatingManager service = RatingManager.getInstance();
        //service.storeRating(ratingValue);

        File path = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOCUMENTS);
        File file = new File(path, "RatingList.txt");

        if(isExternalStorageWritable()) {
            if (!path.exists()) path.mkdirs();
            try {
                FileWriter writer = new FileWriter(file, true);
                writer.append(String.valueOf(ratingValue) + "\n");
                writer.close();
            } catch(IOException e) {
                e.printStackTrace();
                Log.d(TowerJNIAccess.class.getName(), "Problem with FileWriter.");
            }

            //Log.d(TowerJNIAccess.class.getName(), "The file is located in " + file);

            //Log.d(TowerJNIAccess.class.getName(), "Data was perfectly written.");

            //Log.d(TowerJNIAccess.class.getName(), "External storage was writable.");
        }
        else {
            Log.d(TowerJNIAccess.class.getName(), "No access to SD card.");
        }
        Log.d(TowerJNIAccess.class.getName(), "Average rating of " + loadRating());

    }

    public static boolean isExternalStorageWritable() {
        String state = Environment.getExternalStorageState();
        return Environment.MEDIA_MOUNTED.equals(state);
    }

    public static float loadRating() {
        File path = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOCUMENTS);
        File file = new File(path, "RatingList.txt");

        if(isExternalStorageWritable()) {
            if(path.exists()) {
                try {
                    int sum = 0, number = 0;
                    String line;

                    BufferedReader buf = new BufferedReader(new FileReader(file));

                    //Log.d(TowerJNIAccess.class.getName(),"Current line: " + buf.readLine());
                    while((line = buf.readLine()) !=  null && line.length() > 0) {
                        number++;
                        sum += Integer.parseInt(line);
                        //Log.d(TowerJNIAccess.class.getName(), "Data was read.");
                    }
                    buf.close();

                    if(number == 0) {
                        Log.d(TowerJNIAccess.class.getName(), "No ratings available.");
                        return -1;
                    } else {
                        //Log.d(TowerJNIAccess.class.getName(),"Anzahl" + number);
                        //Log.d(TowerJNIAccess.class.getName(),"Summe" + sum);
                        return (float) sum/number;
                    }

                } catch (IOException e) {
                    e.printStackTrace();
                    return -1;
                }
            }
            return -1;
        }
        else {
            Log.d(TowerJNIAccess.class.getName(), "No access to SD card.");
            return -1;
        }
    }

    public void clearRating() {
        File path = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOCUMENTS);
        File file = new File(path, "RatingList.txt");

        if(isExternalStorageWritable()) {
            if(!path.exists()) path.mkdirs();
            try {
                FileWriter writer = new FileWriter(file);
                writer.append("");
                writer.close();
            } catch(IOException e) {
                e.printStackTrace();
            }
        }
    }


    public static void onSetLedMode(int ledModeSet)
    {
        // TODO Logik
        TowerSender towersender = null;

        String message = "";
        //Hier koennte die zu sendende Nachricht unterschieden werden.
        /*
        switch(ledModeSet) {
            case 1: message = "00880FF1234";
                break;
            case 2: message = "00880FF1234";
                break;
            case 3: message = "00880FF1234";
                break;
            case 4: message = "00880FF1234";
                break;
            default: break;
        }*/


        // send update message
        towersender.sendToTower(message);
    }


}