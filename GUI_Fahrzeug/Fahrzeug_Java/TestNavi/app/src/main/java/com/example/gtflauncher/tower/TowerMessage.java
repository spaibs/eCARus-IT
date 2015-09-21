package com.example.gtflauncher.tower;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Arrays;

public class TowerMessage
{
    public static final short INVALID_MESSAGE_ID = -1;

    protected short messageId = INVALID_MESSAGE_ID;

    public TowerMessage(final byte[] completeMessageBuffer)
    {
        if(completeMessageBuffer.length < 5)
        {
            throw new IllegalArgumentException();
        }

        messageId = parseId(completeMessageBuffer);
    }

    public static short parseId(final byte[] completeMessageBuffer)
    {
        // Read message ID from byte 2+3
        final byte[] idData = Arrays.copyOfRange(completeMessageBuffer, 2, 4);
        final ByteBuffer idBuffer = ByteBuffer.wrap(idData);

        idBuffer.order(ByteOrder.LITTLE_ENDIAN); // Be sure the buffer uses correct byte order!
        return idBuffer.getShort(0);
    }

    protected byte[] getDataSection(final byte[] completeMessageBuffer)
    {
        // Read and copy data section starting at byte 5
        return Arrays.copyOfRange(completeMessageBuffer, 5, completeMessageBuffer.length);
    }
}
