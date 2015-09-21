package com.example.gtflauncher.tower;

public class GestureMessage extends TowerMessage
{
    public static final short VALID_MESSAGE_ID = 0x401;

    public static final int INVALID_GESTURE = 0;
    private final byte gestureType;

    public GestureMessage(final byte[] messageBuffer)
    {
        super(messageBuffer); // This will call TowerMessage constructor reading Message ID

        if(messageId != VALID_MESSAGE_ID)
        {
            throw new IllegalArgumentException();
        }

        // Read the data
        final byte[] messageData = getDataSection(messageBuffer);
        final byte transmittedType = messageData[1];
        switch(transmittedType)
        {
            case 1: // W-O
            case 2: // O-W
            case 3: // S-N
            case 4: // N-S
                gestureType = transmittedType;
                break;
            default:
                gestureType = INVALID_GESTURE;
                break;
        }
    }

    public final boolean gestureIsValid()
    {
        return gestureType != INVALID_GESTURE;
    }

    public final int getGestureType()
    {
        return (int) gestureType;
    }
}
