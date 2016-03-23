package de.tum.ei.ecarus.ecarus;

// The interpretation of data messages with the ID 0x101 is stored in an object of this class.
// For more information visit the eCARus Wiki.

public class InterpretedData {
    String[][] dataString = new String[10][1];

    public int leftBlinkerValue;
    public int rightBlinkerValue;
    public int warningLightValue;
    public int headlightsValue;
    public int fullBeamValue;
    public double speed;        // in km/h

    // TODO: add these values to the messages of ID 0x101. This has to be implemented in the tower software first.
    public int backlightsValue;
    public int brakelightsValue;
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
        brakelightsValue = 0;
        batteryLevel = 10;
    }

    public String[][] createStringArrayFromData(){
        dataString[0][0] = Integer.toString(batteryLevel);
        dataString[1][0] = Integer.toString(leftBlinkerValue);
        dataString[2][0] = Integer.toString(rightBlinkerValue);
        dataString[3][0] = Integer.toString(warningLightValue);
        dataString[4][0] = Integer.toString(headlightsValue);
        dataString[5][0] = Integer.toString(fullBeamValue);
        dataString[6][0] = Double.toString(speed) +" km/h";
        dataString[7][0] = Integer.toString(backlightsValue);
        dataString[8][0] = Integer.toString(brakelightsValue);
        return dataString;
    }
}
