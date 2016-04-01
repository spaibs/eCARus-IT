package de.tum.ei.ecarus.ecarus;

// The interpretation of data messages with the ID 0x101 is stored in an object of this class.
// For more information visit the eCARus Wiki.

public class InterpretedData {

    Boolean[] lightData = new Boolean[6];
    int[] infoData= new int[5];

    public int leftBlinkerValue = 0;
    public int rightBlinkerValue = 0;
    public int warningLightValue = 0;
    public int headlightsValue = 0;
    public int fullBeamValue = 0;
    public int speed = 0;        // in km/h

    // TODO: add these values to the messages of ID 0x101. This has to be implemented in the tower software first.
    public int backlightsValue;
    public int batteryLevel;


    public void interpretData(byte[] data) {
        leftBlinkerValue = (data[0] >> 0)& 1;
        rightBlinkerValue = (data[0] >> 1)& 1;
        warningLightValue = (data[0] >> 2)& 1;
        headlightsValue = (data[0] >> 3)& 1;
        fullBeamValue = (data[0] >> 4)& 1;
        speed = data[4]& 0xFF;

        // TODO: get real values
        backlightsValue = 0;
        batteryLevel = 80;
    }


    public Boolean[] createBooleanArrayFromData() {
        lightData[0] = intToBoolean(leftBlinkerValue);
        lightData[1] = intToBoolean(rightBlinkerValue);
        lightData[2] = intToBoolean(warningLightValue);
        lightData[3] = intToBoolean(headlightsValue);
        lightData[4] = intToBoolean(fullBeamValue);
        lightData[5] = intToBoolean(backlightsValue);
        return lightData;
    }

    // This function should be used to turn an int variable with the value "1" or "0" into a boolean variable.
    public Boolean intToBoolean(int value) {
        if(value == 1){
            return true;
        }
        else {
            return false;
        }
    }

    public int[] createIntArrayFromData(){
        infoData[0] = speed;
        infoData[1] = batteryLevel;
        return  infoData;
    }
}
