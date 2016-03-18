package com.example.gtflauncher;

import android.test.AndroidTestCase;
import android.test.suitebuilder.annotation.MediumTest;

import com.example.gtflauncher.tower.GestureMessage;
import com.example.gtflauncher.tower.TowerMessage;
import com.example.gtflauncher.tower.TowerMessageFactory;

public class GestureMessageTest extends AndroidTestCase
{
    @MediumTest
    public void testMessageIDReturnsCorrectInstance()
    {
        final TowerMessage parsedTestMessage = createTestMessage((byte) 1);
        assertTrue("No instance of GestureMessage", parsedTestMessage instanceof GestureMessage);
    }

    @MediumTest
    public void testMessageIDReturnsCorrectValidityTrue()
    {
        final TowerMessage parsedTestMessage = createTestMessage((byte) 2);
        final GestureMessage testMessage = (GestureMessage) parsedTestMessage;

        assertEquals(true, testMessage.gestureIsValid());
    }

    @MediumTest
    public void testMessageIDReturnsCorrectValidityFalse()
    {
        final TowerMessage parsedTestMessage = createTestMessage((byte)GestureMessage.INVALID_MESSAGE_ID);
        final GestureMessage testMessage = (GestureMessage) parsedTestMessage;

        assertEquals(false, testMessage.gestureIsValid());
    }

    @MediumTest
    public void testMessageIDReturnsCorrectValidityFalseHighNumber()
    {
        final TowerMessage parsedTestMessage = createTestMessage(Byte.MAX_VALUE);
        final GestureMessage testMessage = (GestureMessage) parsedTestMessage;

        assertEquals(false, testMessage.gestureIsValid());
    }

    @MediumTest
    public void testAllValidMessages()
    {
        for(int id=1; id<=4; ++id)
        {
            TowerMessage parsedTestMessage = createTestMessage((byte) id);
            GestureMessage testMessage = (GestureMessage) parsedTestMessage;

            assertEquals(true, testMessage.gestureIsValid());
            assertEquals(id, testMessage.getGestureType());
        }
    }

    private TowerMessage createTestMessage(final byte gestureId)
    {
        byte[] testMessage = new byte[15];

        testMessage[2] = 1;
        testMessage[3] = 4;
        testMessage[5] = gestureId;

        return TowerMessageFactory.parse(testMessage);
    }
}

