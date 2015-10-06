package de.tum.ei.ecarus.telemetrie;

import android.util.Log;

/**
 * Created by ga24coq on 22.09.2015.
 */
public class InterpretedData {
    String[][] dataString = new String[60][2];


    // DEFAULT CONFIGURATION

    // car informations
    double tirecircmm = 1753;  // the tire circumference of ecarus

    // accelerator fallback data
    boolean acceleratorfallback=false; // no fallback by default
    int acceleratorrange=0x8C;
    int acceleratoroffset=0x0C;
    // this values are the (former) fallback values from the car code

    // converter data factors
    double converterbatterycurrentfactor = 0.1;	// ??? i am not sure about this data
    double convertercapacitorvoltagefactor = 0.015625;
    double convertercontrollertempfactor = 0.1;
    double convertercurrentrequestfactor = 0.1;
    double convertercurrentrmsfactor = 0.1;
    double convertermotorrpmfactor = 1;
    double convertermotortempfactor = 0.1;
    double convertervclthrottlefactor = 0.00305185094759971922971282082583;
    double DCDCcurrentfactor = 0.01;
    double HVVfactor = 0.01;

    // converter data offsets
    double converterbatterycurrentoffset = 0;
    double convertercapacitorvoltageoffset = 0;
    double convertercontrollertempoffset = 0;
    double convertercurrentrmsoffset = 0;
    double convertercurrentrequestoffset = 0;
    double convertermotorrpmoffset = 0;
    double convertermotortempoffset = 0;
    double convertervclthrottleoffset = 0;
    double DCDCcurrentoffset = 0;
    double HVVoffset = 0;



    // ECARUS' DATA WHICH IS SUPPOSED TO BE SENT OVER WI-FI
    //    QDateTime timestamp;
    public int EngineSwitch;
    public int MasterSwitch;
    public int RunlevelK60rear;
    public int RunlevelK60front;
    public int rawacceleratorangle;
    public int rawacceleratorangleGray;
    public int gear;
    public int TorqueLeft;
    public int BrakingTorqueLeft;
    public int TorqueRight;
    public int BrakingTorqueRight;
    public int erroridK60rear;
    public int erroridK60front;
    public boolean EthernetOkFront;
    public boolean EthernetOkRear;

    // left controller
    public float ConverterLeftBatteryCurrent;		// in A
    public float ConverterLeftCapacitorVoltage;    // in V
    public float ConverterLeftControllerTemp;      // in �C
    public float ConverterLeftCurrentRMS;          // in A
    public float ConverterLeftCurrentRequest;      // in A
    public int ConverterLeftMotorRPM;              // in RPM
    public float ConverterLeftMotorTemp;           // in �C
    public float ConverterLeftVCLThrottle;         // in %

    // right controller
    public float ConverterRightBatteryCurrent;		// in A
    public float ConverterRightCapacitorVoltage;   // in V
    public float ConverterRightControllerTemp;     // in �C
    public float ConverterRightCurrentRMS;         // in A
    public float ConverterRightCurrentRequest;     // in A
    public int ConverterRightMotorRPM;             // in RPM
    public float ConverterRightMotorTemp;          // in �C
    public float ConverterRightVCLThrottle;        // in %

    // security board
    public float DCDCcurrent;						// in A
    public float HVV;								// in V
    // light
    public boolean DirectionLeft;
    public boolean DirectionRight;
    public boolean Horn;
    public boolean BackwardLight;
    public boolean BrakeLight;
    public boolean ForwardLight;
    public boolean WarningLight;
    public boolean FullBeamLight;

    public double speed; // in km/h
    public double acceleratorpercentage;	// in %
    public double brakepercentage;			// in %

    //BMS Battery Management System
    public int SOC;
    public int range_for_CAN;
    public int SPI_error;
    public int SPI_log;
    public int BMS_Errors;
    public int SHUNT_error;
    public int RELAY_enable;
    public int current_for_CAN;            //curent in 1/10 A per Bit
    public int voltage_Stack_for_CAN;
    public int voltage_cell_highest_for_CAN;
    public int voltage_cell_lowest_for_CAN;
    public int max_Temp;
    public int[] battery_cell= new int[16];

    public InterpretedData()
    {
        initDataString();
    }

    public void interpretData(int ID, byte[] data)
    {
        int Value = 0;
        int Value2 = 0;
        float ValueF = 0;
        switch(ID)
        {

            case 0x150:	//Status K60 front
                EngineSwitch = data[0]&0x0F;
                MasterSwitch =(data[0]>>4)&0x0F;
                RunlevelK60rear = data[1] & 0x7F;
                EthernetOkRear = 0 != (data[1] & 0x80);
                //frontok=true;
                // frontTimeOut->start(1000); //QObject killtimer timers cannot be stopped from another thread,timers cannot be started from
                break;

            case 0x151: //Status K60 rear
                EngineSwitch = data[0]&0x0F;
                MasterSwitch =(data[0]>>4)&0x0F;
                RunlevelK60front = data[1] & 0x7F;
                EthernetOkFront = 0 != (data[1] & 0x80);
                //rearok=true;
                //rearTimeOut->start(1000);
                break;


            case 0x167:	//Gaspedal
                Value = ((int)data[1]);
                Value += ((int)data[0])<<8;

                rawacceleratorangle=Value;

                Value = ((int)data[4]);
                Value += ((int)data[5])<<8;

                rawacceleratorangleGray=Value;

                if(data[2] == 0xF0)
                {
                    gear=1;
                }
                else if(data[2] == 0x0F)
                {
                    gear=-1;
                }
                else if(data[2] == 0x00)
                {
                    gear=0;
                }
                else
                {
                    gear=-9; //undefined
                }

                break;
            case 0x222:	//Drehmoment linker Umrichter
                Value2 = (data[0]);
                Value2 += (data[1])<<8;
                Value2 = unsignedIntToSignedInt(Value2, 15);

                TorqueLeft=Value2;

                Value2 = (data[2]);
                Value2 += (data[3])<<8;
                Value2 = unsignedIntToSignedInt(Value2, 15);

                BrakingTorqueLeft=Value2;

                break;
            case 0x224:	//Drehmoment rechter Umrichter
                Value2 = (data[0]);
                Value2 += (data[1])<<8;
                Value2 = unsignedIntToSignedInt(Value2, 15);

                TorqueRight=Value2;

                Value2 = (data[2]);
                Value2 += (data[3])<<8;
                Value2 = unsignedIntToSignedInt(Value2, 15);

                BrakingTorqueRight=Value2;

                break;
            case 0x280: //Error K60 hinten
                erroridK60rear=(data[0]<<24) + (data[1]<<16) + (data[2]<<8) + (data[3]);
                break;
            case 0x281: //Error K60 vorne
                erroridK60front=(data[0]<<24) + (data[1]<<16) + (data[2]<<8) + (data[3]);
                break;

            case 0x5A2: //Werte linker Umrichter 1
                Value = ((int)data[0]);
                Value += ((int)data[1])<<8;  // i am not sure how to interpret this data
                Value = unsignedIntToSignedInt(Value,15);
                ValueF = (float)Value;           //?
                ValueF *= converterbatterycurrentfactor;
                ValueF += converterbatterycurrentoffset;
                ConverterLeftBatteryCurrent=ValueF;

                Value = ((int)data[2]);
                Value += ((int)data[3])<<8;
                Value = unsignedIntToSignedInt(Value, 15);
                ValueF = (float)Value;
                ValueF *= convertercapacitorvoltagefactor;
                ValueF += convertercapacitorvoltageoffset;
                ConverterLeftCapacitorVoltage=ValueF;

                Value = ((int)data[4]);
                Value += ((int)data[5])<<8;
                Value = unsignedIntToSignedInt(Value, 15);
                ValueF = (float)Value;
                ValueF *= convertercontrollertempfactor;
                ValueF += convertercontrollertempoffset;
                ConverterLeftControllerTemp=ValueF;

                Value = ((int)data[6]);
                Value += ((int)data[7])<<8;
                Value = unsignedIntToSignedInt(Value, 15);
                ValueF = (float)Value;
                ValueF *= convertercurrentrmsfactor;
                ValueF += convertercurrentrmsoffset;
                ConverterLeftCurrentRMS=ValueF;

                break;

            case 0x5A3: //Werte linker Umrichter 2
                Value = ((int)data[0]);
                Value += ((int)data[1])<<8;
                Value = unsignedIntToSignedInt(Value, 15);
                ValueF = (float)Value;
                ValueF *= convertercurrentrequestfactor;
                ValueF += convertercurrentrequestoffset;
                ConverterLeftCurrentRequest=ValueF;


                Value = ((int)data[2]);
                Value += ((int)data[3])<<8;
                Value = unsignedIntToSignedInt(Value, 15);
                Value *= convertermotorrpmfactor;
                Value += convertermotorrpmoffset;
                ConverterLeftMotorRPM=Value;

                Value = ((int)data[4]);
                Value += ((int)data[5])<<8;
                Value = unsignedIntToSignedInt(Value, 15);
                ValueF = (float)Value;
                ValueF *= convertermotortempfactor;
                ValueF += convertermotortempoffset;
                ConverterLeftMotorTemp=ValueF;


                // VCL Throttle
                Value2 = ((short)data[6]);
                Value2 += ((short)data[7])<<8;
                Value2 = unsignedIntToSignedInt(Value2, 15);
                ValueF = (float)Value2;
                ValueF *= convertervclthrottlefactor;
                ValueF += convertervclthrottleoffset;
                ConverterLeftVCLThrottle=ValueF;

                break;

            case 0x5A4: //Werte rechter Umrichter 1
                Value = ((int)data[0]);
                Value += ((int)data[1])<<8;  // i am not sure how to interpret this data
                Value = unsignedIntToSignedInt(Value, 15);
                ValueF = (float)Value;           //?
                ValueF *= converterbatterycurrentfactor;
                ValueF += converterbatterycurrentoffset;
                ConverterLeftBatteryCurrent=ValueF;

                Value = ((int)data[2]);
                Value += ((int)data[3])<<8;
                Value = unsignedIntToSignedInt(Value, 15);
                ValueF = (float)Value;
                ValueF *= convertercapacitorvoltagefactor;
                ValueF += convertercapacitorvoltageoffset;
                ConverterRightCapacitorVoltage=ValueF;

                Value = ((int)data[4]);
                Value += ((int)data[5])<<8;
                Value = unsignedIntToSignedInt(Value, 15);
                ValueF = (float)Value;
                ValueF *= convertercontrollertempfactor;
                ValueF += convertercontrollertempoffset;
                ConverterRightControllerTemp=ValueF;

                Value = ((int)data[6]);
                Value += ((int)data[7])<<8;
                Value = unsignedIntToSignedInt(Value, 15);
                ValueF = (float)Value;
                ValueF *= convertercurrentrmsfactor;
                ValueF += convertercurrentrmsoffset;
                ConverterRightCurrentRMS=ValueF;

                break;

            case 0x5A5: //Werte rechter Umrichter 2
                Value = ((int)data[0]);
                Value += ((int)data[1])<<8;
                Value = unsignedIntToSignedInt(Value, 15);
                ValueF = (float)Value;
                ValueF *= convertercurrentrequestfactor;
                ValueF += convertercurrentrequestoffset;
                ConverterRightCurrentRequest=ValueF;
                // no infotamtion found in documentation


                Value = ((int)data[2]);
                Value += ((int)data[3])<<8;
                Value = unsignedIntToSignedInt(Value, 15);
                Value *= convertermotorrpmfactor;
                Value += convertermotorrpmoffset;
                ConverterRightMotorRPM=Value;

                Value = ((int)data[4]);
                Value += ((int)data[5])<<8;
                Value = unsignedIntToSignedInt(Value, 15);
                ValueF = (float)Value;
                ValueF *= convertermotortempfactor;
                ValueF += convertermotortempoffset;
                ConverterRightMotorTemp=ValueF;


                // VCL Throttle
                Value2 = ((short)data[6]);
                Value2 += ((short)data[7])<<8;
                Value2 = unsignedIntToSignedInt(Value2, 15);
                ValueF = (float)Value2;
                ValueF *= convertervclthrottlefactor;
                ValueF += convertervclthrottleoffset;
                ConverterRightVCLThrottle = ValueF;

                break;

            case 0x300: // lights
                DirectionLeft=0 != (data[2] & 0x01);
                DirectionRight=0 != (data[2] & 0x02);
                Horn= 0 != (data[2] & 0x04);
                BackwardLight=0 != (data[2] & 0x08);
                BrakeLight=0 != (data[2] & 0x10);
                ForwardLight= 0 != (data[2] & 0x20);
                FullBeamLight=0 != (data[1] & 0x10);
                WarningLight=0 != (data[1] & 0x40);
                break;
            case 0x170: // pedal information
                Value = data[0];	// accelerator
                Value += data[1] << 8;
                ValueF = (float)Value;
                ValueF /= 10;
                acceleratorpercentage = ValueF;

                Value = data[6];   // brake
                Value += data[7] << 8;
                ValueF = (float)Value;
                ValueF /= 10;
                brakepercentage = ValueF;
                break;

            case 0x180: // safety board Rear
                Value = data[2]; // current DC/DC converter
                Value += data[3] << 8;
                Value = unsignedIntToSignedInt(Value, 15);
                ValueF = (float)Value;
                ValueF *= DCDCcurrentfactor;
                ValueF += DCDCcurrentoffset;
                DCDCcurrent = ValueF;

                Value = data[4]; // hv voltage
                Value += data[5] << 8;
                Value = unsignedIntToSignedInt(Value, 15);
                ValueF = (float)Value;
                ValueF *= HVVfactor;
                ValueF += HVVoffset;
                HVV = ValueF;
                break;

            case 0x250:
                SOC = data[0];     // SOC [0-100]
                range_for_CAN =  data[1];
                range_for_CAN =  range_for_CAN << 8;
                range_for_CAN +=  data[2];
                SPI_error = data[3];
                SPI_log = data[4];
                BMS_Errors = data[5];
                SHUNT_error = data[6];
                RELAY_enable = data[7];
                break;

            case 0x251:
                current_for_CAN = data[0];
                current_for_CAN = current_for_CAN << 8;
                current_for_CAN +=  data[1];
                voltage_Stack_for_CAN = data[2];
                voltage_Stack_for_CAN = voltage_Stack_for_CAN << 8;
                voltage_Stack_for_CAN +=  data[3];
                voltage_cell_highest_for_CAN = data[4];
                voltage_cell_highest_for_CAN = voltage_cell_highest_for_CAN << 8;
                voltage_cell_highest_for_CAN +=  data[5];
                voltage_cell_lowest_for_CAN= data[6];
                voltage_cell_lowest_for_CAN = voltage_cell_lowest_for_CAN << 8;
                voltage_cell_lowest_for_CAN +=  data[7];
                break;

            case 0x252:
                max_Temp = data[0];
                break;

        /*can message from 453-456 read in mV*/
            case 0x453:
                battery_cell[0] = data[0];
                battery_cell[0] = battery_cell[0] << 8;
                battery_cell[0] += data[1];
                battery_cell[1] = data[2];
                battery_cell[1] = battery_cell[1] << 8;
                battery_cell[1] += data[3];
                battery_cell[2] = data[4];
                battery_cell[2] = battery_cell[2] << 8;
                battery_cell[2] += data[5];
                battery_cell[3] = data[6];
                battery_cell[3] = battery_cell[3] << 8;
                battery_cell[3] += data[7];
                break;

            case 0x454:
                battery_cell[4] = data[0];
                battery_cell[4] = battery_cell[0] << 8;
                battery_cell[4] += data[1];
                battery_cell[5] = data[2];
                battery_cell[5] = battery_cell[1] << 8;
                battery_cell[5] += data[3];
                battery_cell[6] = data[4];
                battery_cell[6] = battery_cell[2] << 8;
                battery_cell[6] += data[5];
                battery_cell[7] = data[6];
                battery_cell[7] = battery_cell[3] << 8;
                battery_cell[7] += data[7];
                break;

            case 0x455:
                battery_cell[8] = data[0];
                battery_cell[8] = battery_cell[0] << 8;
                battery_cell[8] += data[1];
                battery_cell[9] = data[2];
                battery_cell[9] = battery_cell[1] << 8;
                battery_cell[9] += data[3];
                battery_cell[10] = data[4];
                battery_cell[10] = battery_cell[2] << 8;
                battery_cell[10] += data[5];
                battery_cell[11] = data[6];
                battery_cell[11] = battery_cell[3] << 8;
                battery_cell[11] += data[7];
                break;

            case 0x456:
                battery_cell[12] = data[0];
                battery_cell[12] = battery_cell[0] << 8;
                battery_cell[12] += data[1];
                battery_cell[13] = data[2];
                battery_cell[13] = battery_cell[1] << 8;
                battery_cell[13] += data[3];
                battery_cell[14] = data[4];
                battery_cell[14] = battery_cell[2] << 8;
                battery_cell[14] += data[5];
                battery_cell[15] = data[6];
                battery_cell[15] = battery_cell[3] << 8;
                battery_cell[15] += data[7];
                break;
        }

        // calculate speed
        speed = (double)(ConverterLeftMotorRPM+ConverterRightMotorRPM)   / 2 * tirecircmm * 6 / 7 / 100000;

        // calculate accelerator percentage
        // if fallback is active (will overwrite values send from car)

        if (acceleratorfallback)
        {
            if (acceleratorrange > 0)
            {
                // only calculate the accelerator position if the value of range is greater than 0
                // to avoid a division through zero
                acceleratorpercentage = (rawacceleratorangle-acceleratoroffset) / acceleratorrange;
            }
            else
            {
                acceleratorpercentage = 0;
            }
        }
        // check if the data is in the correct range and correct them if they are not
        if ( acceleratorpercentage < 0)
        {
            acceleratorpercentage = 0;
        }
        if (acceleratorpercentage > 100)
        {
            acceleratorpercentage = 100;
        }
    }
    public int unsignedIntToSignedInt(int unsigned, int MSBindex) // This function gets an unsigned Integer and interprets the MSB as a sign bit and returns a signed int
    {
        /*
            unsigned=00..001010101110 != 687 , MSBindex=9 -> should be intepreted as a negative number
            signed = 100..0010101110 !=-175
         */
        if((unsigned & (0x01 << MSBindex)) != 0) //unsigned is negative?
        {
            return -(unsigned & ((0x01 << MSBindex) -1));
        }
        else
            return unsigned; //unsigned is positive
    }
    public String[][] createStringArrayFromData(){

        dataString[1][1] = "" + EngineSwitch;
        dataString[2][1] = ""+ MasterSwitch;
        dataString[3][1] = ""+RunlevelK60rear;
        dataString[4][1] =""+ RunlevelK60front;
        dataString[5][1] = ""+rawacceleratorangle;
        dataString[6][1] = ""+rawacceleratorangleGray;
        dataString[7][1] = ""+gear;
        dataString[8][1] = ""+TorqueLeft;
        dataString[9][1] = ""+BrakingTorqueLeft;
        dataString[10][1] = ""+TorqueRight;
        dataString[11][1] = ""+BrakingTorqueRight;
        dataString[12][1] = ""+erroridK60rear;
        dataString[13][1] = ""+erroridK60front;
        dataString[14][1] = ""+EthernetOkFront;
        dataString[15][1] = ""+EthernetOkRear;
        dataString[16][1] = ConverterLeftBatteryCurrent +" A";
        dataString[17][1] = ConverterLeftCapacitorVoltage +" V";
        dataString[18][1] = ConverterLeftControllerTemp +" °C";
        dataString[19][1] = ConverterLeftCurrentRMS +" A";
        dataString[20][1] = ConverterLeftCurrentRequest +" A";
        dataString[21][1] = ConverterLeftMotorRPM +" RPM";
        dataString[22][1] = ConverterLeftMotorTemp +" °C";
        dataString[23][1] = ConverterLeftVCLThrottle +" %";
        dataString[24][1] = ConverterRightBatteryCurrent +" A";
        dataString[25][1] = ConverterRightCapacitorVoltage +" V";
        dataString[26][1] = ConverterRightControllerTemp +" °C";
        dataString[27][1] = ConverterRightCurrentRMS +" A";
        dataString[28][1] = ConverterRightCurrentRequest +" A";
        dataString[29][1] = ConverterRightMotorRPM +" RPM";
        dataString[30][1] = ConverterRightMotorTemp+ " °C";
        dataString[31][1] = ConverterRightVCLThrottle +" %";
        dataString[32][1] = DCDCcurrent +" A";
        dataString[33][1] = HVV +" V";
        dataString[34][1] = ""+DirectionLeft;
        dataString[35][1] = ""+DirectionRight;
        dataString[36][1] = ""+Horn;
        dataString[37][1] = ""+BackwardLight;
        dataString[38][1] = ""+BrakeLight;
        dataString[39][1] = ""+ForwardLight;
        dataString[40][1] = ""+WarningLight;
        dataString[41][1] = ""+FullBeamLight;
        dataString[42][1] = speed +" km/h";
        dataString[43][1] = acceleratorpercentage +" %";
        dataString[44][1] = brakepercentage +" %";
        dataString[45][1] = ""+SOC;
        dataString[46][1] = ""+range_for_CAN;
        dataString[47][1] = ""+SPI_error;
        dataString[48][1] = ""+SPI_log;
        dataString[49][1] = ""+BMS_Errors;
        dataString[50][1] = ""+SHUNT_error;
        dataString[51][1] = ""+RELAY_enable;
        dataString[52][1] = current_for_CAN +" A";
        dataString[53][1] = voltage_Stack_for_CAN +" V";
        dataString[54][1] = voltage_cell_highest_for_CAN +" V";
        dataString[55][1] = voltage_cell_lowest_for_CAN + " V";
        dataString[56][1] = max_Temp + " °C";
        return dataString;
    }
    public void initDataString()
    {
        dataString[1][0] = "EngineSwitch";
        dataString[2][0] = "MasterSwitch";
        dataString[3][0] = "RunlevelK60rear";
        dataString[4][0] = "RunlevelK60front";
        dataString[5][0] = "rawacceleratorangle";
        dataString[6][0] = "rawacceleratorangleGray";
        dataString[7][0] = "gear";
        dataString[8][0] = "TorqueLeft";
        dataString[9][0] = "BrakingTorqueLeft";
        dataString[10][0] = "TorqueRight";
        dataString[11][0] = "BrakingTorqueRight";
        dataString[12][0] = "erroridK60rear";
        dataString[13][0] = "erroridK60front";
        dataString[14][0] = "EthernetOkFront";
        dataString[15][0] = "EthernetOkRear";
        dataString[16][0] = "ConverterLeftBatteryCurrent";
        dataString[17][0] = "ConverterLeftCapacitorVoltage";
        dataString[18][0] = "ConverterLeftControllerTemp";
        dataString[19][0] = "ConverterLeftCurrentRMS";
        dataString[20][0] = "ConverterLeftCurrentRequest";
        dataString[21][0] = "ConverterLeftMotorRPM";
        dataString[22][0] = "ConverterLeftMotorTemp";
        dataString[23][0] = "ConverterLeftVCLThrottle";
        dataString[24][0] = "ConverterRightBatteryCurrent";
        dataString[25][0] = "ConverterRightCapacitorVoltage";
        dataString[26][0] = "ConverterRightControllerTemp";
        dataString[27][0] = "ConverterRightCurrentRMS";
        dataString[28][0] = "ConverterRightCurrentRequest";
        dataString[29][0] = "ConverterRightMotorRPM";
        dataString[30][0] = "ConverterRightMotorTemp";
        dataString[31][0] = "ConverterRightVCLThrottle";
        dataString[32][0] = "DCDCcurrent";
        dataString[33][0] = "HVV in V";
        dataString[34][0] = "DirectionLeft";
        dataString[35][0] = "DirectionRight";
        dataString[36][0] = "Horn";
        dataString[37][0] = "BackwardLight";
        dataString[38][0] = "BrakeLight";
        dataString[39][0] = "ForwardLight";
        dataString[40][0] = "WarningLight";
        dataString[41][0] = "FullBeamLight";
        dataString[42][0] = "speed";
        dataString[43][0] = "acceleratorpercentage";
        dataString[44][0] = "brakepercentage";
        dataString[45][0] = "SOC";
        dataString[46][0] = "range_for_CAN";
        dataString[47][0] = "SPI_error";
        dataString[48][0] = "SPI_log";
        dataString[49][0] = "BMS_Errors";
        dataString[50][0] = "SHUNT_error";
        dataString[51][0] = "RELAY_enable";
        dataString[52][0] = "current_for_CAN";
        dataString[53][0] = "voltage_Stack_for_CAN";
        dataString[54][0] = "voltage_cell_highest_for_CAN";
        dataString[55][0] = "voltage_cell_lowest_for_CAN";
        dataString[56][0] = "max_Temp";
    }
}
