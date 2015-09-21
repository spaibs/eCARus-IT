package com.example.gtflauncher;

import android.test.AndroidTestCase;
import android.test.suitebuilder.annotation.MediumTest;

import com.example.gtflauncher.tower.LightState;
import com.example.gtflauncher.tower.LightStateMessage;
import com.example.gtflauncher.tower.TowerMessage;
import com.example.gtflauncher.tower.TowerMessageFactory;

public class LightStateMessageTest extends AndroidTestCase
{
    @MediumTest
    public void testMessageIDReturnsCorrectInstance()
    {
        byte[] testMessage = initializeTestMessage();
        final TowerMessage parsedMessage = TowerMessageFactory.parse(testMessage);
        assertTrue("No instance of LightStateMessage", parsedMessage instanceof LightStateMessage);
    }

    @MediumTest
    public void testMessageIDReturnsCorrectStatesAllActive()
    {
        byte[] testMessage = initializeTestMessage();
        testMessage[4] = (1 << 1) | 1;
        testMessage[5] = (1 << 5) | (1 << 4);
        testMessage[6] = (1 << 5) | (1 << 1) | 1;

        final LightState testState = extractLightState(testMessage);

        assertEquals(true, testState.highBeamIsOn);
        assertEquals(true, testState.lowBeamIsOn);
        assertEquals(true, testState.indicatorLeftIsOn);
        assertEquals(true, testState.indicatorRightIsOn);
    }

    @MediumTest
    public void testMessageIDReturnsCorrectStatesAllInactive()
    {
        byte[] testMessage = initializeTestMessage();
        testMessage[4] = 0;
        testMessage[5] = 0;
        testMessage[6] = 0;

        final LightState testState = extractLightState(testMessage);

        assertEquals(false, testState.highBeamIsOn);
        assertEquals(false, testState.lowBeamIsOn);
        assertEquals(false, testState.indicatorLeftIsOn);
        assertEquals(false, testState.indicatorRightIsOn);
    }

    @MediumTest
    public void testMessageIDReturnsCorrectStatesIndicatorLeftActive()
    {
        byte[] testMessage = initializeTestMessage();
        testMessage[4] = 1;
        testMessage[5] = 0;
        testMessage[6] = 1;

        final LightState testState = extractLightState(testMessage);

        assertEquals(false, testState.highBeamIsOn);
        assertEquals(false, testState.lowBeamIsOn);
        assertEquals(true, testState.indicatorLeftIsOn);
        assertEquals(false, testState.indicatorRightIsOn);
    }

    @MediumTest
    public void testMessageIDReturnsCorrectStatesHighBeamOnlyActive()
    {
        byte[] testMessage = initializeTestMessage();
        testMessage[4] = 0;
        testMessage[5] = (1 << 5);
        testMessage[6] = (1 << 5);

        final LightState testState = extractLightState(testMessage);

        assertEquals(true, testState.highBeamIsOn);
        assertEquals(false, testState.lowBeamIsOn);
        assertEquals(false, testState.indicatorLeftIsOn);
        assertEquals(false, testState.indicatorRightIsOn);
    }

    private byte[] initializeTestMessage()
    {
        byte[] testMessage = new byte[15];
        testMessage[2] = 0;
        testMessage[3] = 3;

        return testMessage;
    }

    private LightState extractLightState(byte[] testMessage)
    {
        final TowerMessage parsedMessage = TowerMessageFactory.parse(testMessage);
        final LightStateMessage testMessageLightState = (LightStateMessage) parsedMessage;
        return testMessageLightState.getLightState();
    }
}
