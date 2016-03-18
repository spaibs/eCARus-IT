package com.example.gtflauncher.tower;

public class LightState
{
    public boolean lowBeamIsOn;
    public boolean highBeamIsOn;
    public boolean indicatorRightIsOn;
    public boolean indicatorLeftIsOn;
    public boolean indicatorWarningLightIsOn;

    public LightState()
    {
        lowBeamIsOn = false;
        highBeamIsOn = false;
        indicatorRightIsOn = false;
        indicatorLeftIsOn = false;
        indicatorWarningLightIsOn = false;
    }

    public LightState(final LightState lightStateToCopy)
    {
        lowBeamIsOn = lightStateToCopy.lowBeamIsOn;
        highBeamIsOn = lightStateToCopy.highBeamIsOn;
        indicatorRightIsOn = lightStateToCopy.indicatorRightIsOn;
        indicatorLeftIsOn = lightStateToCopy.indicatorLeftIsOn;
        indicatorWarningLightIsOn = lightStateToCopy.indicatorWarningLightIsOn;
    }

    public boolean equals(final LightState other)
    {
        if(other == null) return false;
        if(other == this) return true;
        if(!(other instanceof LightState)) return false;
        LightState otherState = (LightState) other;
        return  (this.highBeamIsOn == other.highBeamIsOn) &&
                (this.lowBeamIsOn == other.lowBeamIsOn) &&
                (this.indicatorRightIsOn == other.indicatorRightIsOn) &&
                (this.indicatorLeftIsOn == other.indicatorLeftIsOn) &&
                (this.indicatorWarningLightIsOn == other.indicatorWarningLightIsOn);
    }
}
