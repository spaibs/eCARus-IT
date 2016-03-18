package com.example.gtflauncher.tower;

public class LightStateMessage extends TowerMessage
{
    public static final short VALID_MESSAGE_ID = 0x300;
    private final LightState lightState;

    public LightStateMessage(final byte[] messageBuffer)
    {
        super(messageBuffer); // This will call TowerMessage constructor reading Message ID and storing data

        if(messageId != VALID_MESSAGE_ID)
        {
            throw new IllegalArgumentException();
        }

        lightState = new LightState();

        // Read the message data
        final byte[] messageData = getDataSection(messageBuffer);
        readBeamBits(messageData);
        readIndicatorBits(messageData);
    }

    private void readBeamBits(final byte[] messageData)
    {
        // get light status
        /*boolean lightOnBit =          ((messageData[2] >> 5) & 0x1) == 1;
        final boolean highBeamBit =         ((messageData[1] >> 5) & 0x1) == 1;
        final boolean lowBeamBit =          ((messageData[1] >> 4) & 0x1) == 1;

        lightOnBit = true;
*/
        boolean lightOnBit =          ((messageData[2] >> 5) & 0x1) == 1 && (messageData[1]>>5 & 1) == 1;
        final boolean highBeamBit =         ((messageData[2] >> 5) & 1) == 1 && (messageData[1]>>5 & 1) == 1;
        final boolean lowBeamBit =          ((messageData[2] >> 5) & 1) == 1 && (messageData[1]>>4 & 1) == 1;

        lightOnBit = true;

        lightState.highBeamIsOn =           (lightOnBit && highBeamBit);
        lightState.lowBeamIsOn =            (lightOnBit && lowBeamBit);
    }

    private void readIndicatorBits(final byte[] messageData)
    {
        final boolean rightIndicatorBit1 =  ((messageData[0] >> 1) & 0x1) == 1;
        final boolean rightIndicatorBit2 =  ((messageData[2] >> 1) & 0x1) == 1;
        final boolean leftIndicatorBit1 =   ((messageData[0]     ) & 0x1) == 1;
        final boolean leftIndicatorBit2 =   ((messageData[2]     ) & 0x1) == 1;

        lightState.indicatorRightIsOn =     (rightIndicatorBit1 && rightIndicatorBit2);
        lightState.indicatorLeftIsOn =      (leftIndicatorBit1 && leftIndicatorBit2);



        //lightState.indicatorLeftIsOn = true;
        //lightState.indicatorRightIsOn = true;



        // TODO extrahiere Warning light bit
        boolean links = (messageData[2] & 0x1) == 1;
        boolean rechts = ((messageData[2] >> 1) & 0x1) == 1;

        lightState.indicatorLeftIsOn = links;
        lightState.indicatorRightIsOn = rechts;

        lightState.indicatorWarningLightIsOn = false;
    }

    public final LightState getLightState()
    {
        return lightState;
    }
}
