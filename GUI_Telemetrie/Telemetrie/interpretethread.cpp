#include "configuration.h"
#include "datatypes.h"
#include "interpretethread.h"
#include "udpthread.h"
#include "utility.h"
#include "mainwindow.h"
#include <iostream>

std::vector <struct RawData> RawHistory;
std::vector <struct InterpretedData> InterpretedHistory;
QMutex* HistoryLock;

InterpreteThread::InterpreteThread(QObject *parent) :
    QThread(parent)
{
    // create the TCriticalSections
    RecQueueLock = new QMutex; // used by TCPThread and InterpreteThread
    HistoryLock = new QMutex; // used by this file and interpreteThread

    //initiate LatestDataInterpreted with Zeros
    LatestDataInterpreted.EngineSwitch = 0;
    LatestDataInterpreted.MasterSwitch = 0;
    LatestDataInterpreted.RunlevelK60front = 0;
    LatestDataInterpreted.RunlevelK60rear = 0;
    LatestDataInterpreted.rawacceleratorangle = 0;
    LatestDataInterpreted.rawacceleratorangleGray = 0;
    LatestDataInterpreted.gear = 0;
    LatestDataInterpreted.TorqueLeft = 0;
    LatestDataInterpreted.BrakingTorqueLeft = 0;
    LatestDataInterpreted.TorqueRight = 0;
    LatestDataInterpreted.BrakingTorqueRight = 0;
    LatestDataInterpreted.erroridK60front = 0;
    LatestDataInterpreted.erroridK60rear = 0;
    LatestDataInterpreted.EthernetOkFront = false;
    LatestDataInterpreted.EthernetOkRear = false;
    LatestDataInterpreted.ConverterLeftBatteryCurrent = 0;
    LatestDataInterpreted.ConverterLeftCapacitorVoltage = 0;
    LatestDataInterpreted.ConverterLeftControllerTemp = 0;
    LatestDataInterpreted.ConverterLeftCurrentRMS = 0;
    LatestDataInterpreted.ConverterLeftCurrentRequest = 0;
    LatestDataInterpreted.ConverterLeftMotorRPM = 0;
    LatestDataInterpreted.ConverterLeftMotorTemp = 0;
    LatestDataInterpreted.ConverterLeftVCLThrottle = 0;
    LatestDataInterpreted.ConverterRightBatteryCurrent = 0;
    LatestDataInterpreted.ConverterRightCapacitorVoltage = 0;
    LatestDataInterpreted.ConverterRightControllerTemp = 0;
    LatestDataInterpreted.ConverterRightCurrentRMS = 0;
    LatestDataInterpreted.ConverterRightCurrentRequest = 0;
    LatestDataInterpreted.ConverterRightMotorRPM = 0;
    LatestDataInterpreted.ConverterRightMotorTemp = 0;
    LatestDataInterpreted.ConverterRightVCLThrottle = 0;
    LatestDataInterpreted.DCDCcurrent = 0;
    LatestDataInterpreted.HVV = 0;
    LatestDataInterpreted.DirectionLeft = false;
    LatestDataInterpreted.DirectionRight = false;
    LatestDataInterpreted.Horn = false;
    LatestDataInterpreted.BackwardLight = false;
    LatestDataInterpreted.BrakeLight = false;
    LatestDataInterpreted.ForwardLight = false;
    LatestDataInterpreted.speed = 0;
    LatestDataInterpreted.acceleratorpercentage = 0;
    LatestDataInterpreted.brakepercentage = 0;
    this->rearTimeOut = new QTimer(this);
    connect(rearTimeOut, SIGNAL(timeout()), this, SLOT(rearTimedOut()));
    rearTimeOut->start(1000);
    this->frontTimeOut = new QTimer(this);
    connect(frontTimeOut, SIGNAL(timeout()), this, SLOT(frontTimedOut()));
    frontTimeOut->start(1000);
    rearok=false;
    frontok=false;
}

void InterpreteThread::run()
{

    static int AvaibleData = 0;

    while(true)
    {
        RecvBuf tmp;
        RecQueueLock->lock();

        // check if queue is empty
        if ( RecQueue.empty() )
        {
            RecQueueLock->unlock();
            QThread::msleep(2);
            continue;
        }

        // copy queue to tmp
        tmp.timestamp = RecQueue.front().timestamp;
        tmp.ticks = RecQueue.front().ticks;
        //qDebug()<< tmp.ticks;
        for (int i=0; i < 14; i++)
        {
            tmp.data[i] = RecQueue.front().data[i];
        }

        // remove element
        RecQueue.pop();
        RecQueueLock->unlock();

        //prepare Raw Data to be written to the log
        for(int j = 0; j < 14; j++)
        {
            LatestRawData.RawData[j] = tmp.data[j];
        }
        receivedMsg.ID = (tmp.data[4]<<16) + (tmp.data[3]<<8) + tmp.data[2];

        //	LatestRawData.numberofreceiveddata = numberofreceiveddata;   // removed
        LatestRawData.timestamp = tmp.timestamp;

        if ( (receivedMsg.ID <= 0x7FF) && (receivedMsg.ID >= 0) )
        {
            LatestRawData.cycle = tmp.ticks - Time_ms[receivedMsg.ID];
            Time_ms[receivedMsg.ID] = tmp.ticks;
        }
        else
        {
            // set cycle count to -1 to indicate that it is invalid
            LatestRawData.cycle = -1;
        }

        receivedMsg.cycle = LatestRawData.cycle;

        //in Puffer Speichern

        for(int j = 0; j < 8; j++)
        {
            receivedMsg.Daten[0+j] = tmp.data[5+j];
        }

        receivedMsg.Time = tmp.ticks;

        HistoryLock->lock();
        if ( tmp.data[0] == 0xAA )
        {
            Daten_konvertieren(receivedMsg );
            InterpretedHistory.push_back(LatestDataInterpreted);
        }
        RawHistory.push_back(LatestRawData);
        HistoryLock->unlock();

        // add Latest receivedMsg to the vector if it is a AA packet
        if (LatestRawData.RawData[0] == 0xAA)
        {
            bool sucess = false;
            if ( !receivedMsgVector.empty() )
            {
                for (std::vector<Ethernet_Msg>::iterator i=receivedMsgVector.begin(); i != receivedMsgVector.end(); ++i)
                {
                    if ( i->ID == receivedMsg.ID )
                    {
                        // if we find our id, change the data
                        for (int j = 0; j < 8; j++)
                        {
                            i->Daten [j] = receivedMsg.Daten [j];
                        }
                        // and the cycle information
                        i->cycle = receivedMsg.cycle;
                        sucess = true;
                        break;
                    }
                }
            }
            if ( !sucess )
            {
                // if not found add it
                receivedMsgVector.push_back(receivedMsg);
            }
        }
        // increase avaible data
        AvaibleData++;
        // copy
        if (GUIDataLock->tryLock()) // we do not update the GUI if the gui is currently busy, we also wont wait
        {
            //*ptr_JsonList = JsonList;
            *ptr_LatestDataInterpreted = LatestDataInterpreted;
            *ptr_receivedMsgVector = receivedMsgVector;
            *ptr_LatestRawData = LatestRawData;
            *ptr_AvaibleData = AvaibleData;
            GUIDataLock->unlock();
        }
    }
}
//-------------------------------------------------------------------------------------------------------------------

void InterpreteThread::Daten_konvertieren(Ethernet_Msg Msg) // deprecated << replace this with GetJSONInterpretation
{
    //used as tempomary storage
    int Value;
    short Value2;
    float ValueF;

    // save the current time to the dataset
    LatestDataInterpreted.timestamp=QDateTime::currentDateTime();

    switch(Msg.ID)
    {
    case 0x150:	//Status K60 front
        LatestDataInterpreted.EngineSwitch = Msg.Daten[0]&0x0F;
        LatestDataInterpreted.MasterSwitch =(Msg.Daten[0]>>4)&0x0F;
        LatestDataInterpreted.RunlevelK60rear = Msg.Daten[1] & 0x7F;
        LatestDataInterpreted.EthernetOkRear = Msg.Daten[1] & 0x80;
        frontok=true;
       // frontTimeOut->start(1000); //QObject killtimer timers cannot be stopped from another thread,timers cannot be started from
        break;

    case 0x151: //Status K60 rear
        LatestDataInterpreted.EngineSwitch = Msg.Daten[0]&0x0F;
        LatestDataInterpreted.MasterSwitch =(Msg.Daten[0]>>4)&0x0F;
        LatestDataInterpreted.RunlevelK60front = Msg.Daten[1] & 0x7F;
        LatestDataInterpreted.EthernetOkFront = Msg.Daten[1] & 0x80;
        rearok=true;
        //rearTimeOut->start(1000);
        break;


    case 0x167:	//Gaspedal
        Value = ((int)Msg.Daten[1]);
        Value += ((int)Msg.Daten[0])<<8;

        LatestDataInterpreted.rawacceleratorangle=Value;

        Value = ((int)Msg.Daten[4]);
        Value += ((int)Msg.Daten[5])<<8;

        LatestDataInterpreted.rawacceleratorangleGray=Value;

        if(Msg.Daten[2] == 0xF0)
        {
            LatestDataInterpreted.gear=1;
        }
        else if(Msg.Daten[2] == 0x0F)
        {
            LatestDataInterpreted.gear=-1;
        }
        else if(Msg.Daten[2] == 0x00)
        {
            LatestDataInterpreted.gear=0;
        }
        else
        {
            LatestDataInterpreted.gear=-9; //undefined
        }

        break;
    case 0x222:	//Drehmoment linker Umrichter
        Value2 = (Msg.Daten[0]);
        Value2 += (Msg.Daten[1])<<8;
        Value2 = utility::UnsignedIntToSignedInt(Value2, 15);

        LatestDataInterpreted.TorqueLeft=Value2;

        Value2 = (Msg.Daten[2]);
        Value2 += (Msg.Daten[3])<<8;
        Value2 = utility::UnsignedIntToSignedInt(Value2, 15);

        LatestDataInterpreted.BrakingTorqueLeft=Value2;

        break;
    case 0x224:	//Drehmoment rechter Umrichter
        Value2 = (Msg.Daten[0]);
        Value2 += (Msg.Daten[1])<<8;
        Value2 = utility::UnsignedIntToSignedInt(Value2, 15);

        LatestDataInterpreted.TorqueRight=Value2;

        Value2 = (Msg.Daten[2]);
        Value2 += (Msg.Daten[3])<<8;
        Value2 = utility::UnsignedIntToSignedInt(Value2, 15);

        LatestDataInterpreted.BrakingTorqueRight=Value2;

        break;
    case 0x280: //Error K60 hinten
        LatestDataInterpreted.erroridK60rear=(Msg.Daten[0]<<24) + (Msg.Daten[1]<<16) + (Msg.Daten[2]<<8) + (Msg.Daten[3]);
        break;
    case 0x281: //Error K60 vorne
        LatestDataInterpreted.erroridK60front=(Msg.Daten[0]<<24) + (Msg.Daten[1]<<16) + (Msg.Daten[2]<<8) + (Msg.Daten[3]);
        break;

    case 0x5A2: //Werte linker Umrichter 1
        Value = ((int)Msg.Daten[0]);
        Value += ((int)Msg.Daten[1])<<8;  // i am not sure how to interpret this data
        Value = utility::UnsignedIntToSignedInt(Value,15);
        ValueF = (float)Value;           //?
        ValueF *= config_handler::currentconfig.converterbatterycurrentfactor;
        ValueF += config_handler::currentconfig.converterbatterycurrentoffset;
        LatestDataInterpreted.ConverterLeftBatteryCurrent=ValueF;

        Value = ((int)Msg.Daten[2]);
        Value += ((int)Msg.Daten[3])<<8;
        Value = utility::UnsignedIntToSignedInt(Value, 15);
        ValueF = (float)Value;
        ValueF *= config_handler::currentconfig.convertercapacitorvoltagefactor;
        ValueF += config_handler::currentconfig.convertercapacitorvoltageoffset;
        LatestDataInterpreted.ConverterLeftCapacitorVoltage=ValueF;

        Value = ((int)Msg.Daten[4]);
        Value += ((int)Msg.Daten[5])<<8;
        Value = utility::UnsignedIntToSignedInt(Value, 15);
        ValueF = (float)Value;
        ValueF *= config_handler::currentconfig.convertercontrollertempfactor;
        ValueF += config_handler::currentconfig.convertercontrollertempoffset;
        LatestDataInterpreted.ConverterLeftControllerTemp=ValueF;

        Value = ((int)Msg.Daten[6]);
        Value += ((int)Msg.Daten[7])<<8;
        Value = utility::UnsignedIntToSignedInt(Value, 15);
        ValueF = (float)Value;
        ValueF *= config_handler::currentconfig.convertercurrentrmsfactor;
        ValueF += config_handler::currentconfig.convertercurrentrmsoffset;
        LatestDataInterpreted.ConverterLeftCurrentRMS=ValueF;

        break;

    case 0x5A3: //Werte linker Umrichter 2
        Value = ((int)Msg.Daten[0]);
        Value += ((int)Msg.Daten[1])<<8;
        Value = utility::UnsignedIntToSignedInt(Value, 15);
        ValueF = (float)Value;
        ValueF *= config_handler::currentconfig.convertercurrentrequestfactor;
        ValueF += config_handler::currentconfig.convertercurrentrequestoffset;
        LatestDataInterpreted.ConverterLeftCurrentRequest=ValueF;


        Value = ((int)Msg.Daten[2]);
        Value += ((int)Msg.Daten[3])<<8;
        Value = utility::UnsignedIntToSignedInt(Value, 15);
        Value *= config_handler::currentconfig.convertermotorrpmfactor;
        Value += config_handler::currentconfig.convertermotorrpmoffset;
        LatestDataInterpreted.ConverterLeftMotorRPM=Value;

        Value = ((int)Msg.Daten[4]);
        Value += ((int)Msg.Daten[5])<<8;
        Value = utility::UnsignedIntToSignedInt(Value, 15);
        ValueF = (float)Value;
        ValueF *= config_handler::currentconfig.convertermotortempfactor;
        ValueF += config_handler::currentconfig.convertermotortempoffset;
        LatestDataInterpreted.ConverterLeftMotorTemp=ValueF;


        // VCL Throttle
        Value2 = ((short)Msg.Daten[6]);
        Value2 += ((short)Msg.Daten[7])<<8;
        Value2 = utility::UnsignedIntToSignedInt(Value2, 15);
        ValueF = (float)Value2;
        ValueF *= config_handler::currentconfig.convertervclthrottlefactor;
        ValueF += config_handler::currentconfig.convertervclthrottleoffset;
        LatestDataInterpreted.ConverterLeftVCLThrottle=ValueF;

        break;

    case 0x5A4: //Werte rechter Umrichter 1
        Value = ((int)Msg.Daten[0]);
        Value += ((int)Msg.Daten[1])<<8;  // i am not sure how to interpret this data
        Value = utility::UnsignedIntToSignedInt(Value, 15);
        ValueF = (float)Value;           //?
        ValueF *= config_handler::currentconfig.converterbatterycurrentfactor;
        ValueF += config_handler::currentconfig.converterbatterycurrentoffset;
        LatestDataInterpreted.ConverterLeftBatteryCurrent=ValueF;

        Value = ((int)Msg.Daten[2]);
        Value += ((int)Msg.Daten[3])<<8;
        Value = utility::UnsignedIntToSignedInt(Value, 15);
        ValueF = (float)Value;
        ValueF *= config_handler::currentconfig.convertercapacitorvoltagefactor;
        ValueF += config_handler::currentconfig.convertercapacitorvoltageoffset;
        LatestDataInterpreted.ConverterRightCapacitorVoltage=ValueF;

        Value = ((int)Msg.Daten[4]);
        Value += ((int)Msg.Daten[5])<<8;
        Value = utility::UnsignedIntToSignedInt(Value, 15);
        ValueF = (float)Value;
        ValueF *= config_handler::currentconfig.convertercontrollertempfactor;
        ValueF += config_handler::currentconfig.convertercontrollertempoffset;
        LatestDataInterpreted.ConverterRightControllerTemp=ValueF;

        Value = ((int)Msg.Daten[6]);
        Value += ((int)Msg.Daten[7])<<8;
        Value = utility::UnsignedIntToSignedInt(Value, 15);
        ValueF = (float)Value;
        ValueF *= config_handler::currentconfig.convertercurrentrmsfactor;
        ValueF += config_handler::currentconfig.convertercurrentrmsoffset;
        LatestDataInterpreted.ConverterRightCurrentRMS=ValueF;

        break;

    case 0x5A5: //Werte rechter Umrichter 2
        Value = ((int)Msg.Daten[0]);
        Value += ((int)Msg.Daten[1])<<8;
        Value = utility::UnsignedIntToSignedInt(Value, 15);
        ValueF = (float)Value;
        ValueF *= config_handler::currentconfig.convertercurrentrequestfactor;
        ValueF += config_handler::currentconfig.convertercurrentrequestoffset;
        LatestDataInterpreted.ConverterRightCurrentRequest=ValueF;
        // no infotamtion found in documentation


        Value = ((int)Msg.Daten[2]);
        Value += ((int)Msg.Daten[3])<<8;
        Value = utility::UnsignedIntToSignedInt(Value, 15);
        Value *= config_handler::currentconfig.convertermotorrpmfactor;
        Value += config_handler::currentconfig.convertermotorrpmoffset;
        LatestDataInterpreted.ConverterRightMotorRPM=Value;

        Value = ((int)Msg.Daten[4]);
        Value += ((int)Msg.Daten[5])<<8;
        Value = utility::UnsignedIntToSignedInt(Value, 15);
        ValueF = (float)Value;
        ValueF *= config_handler::currentconfig.convertermotortempfactor;
        ValueF += config_handler::currentconfig.convertermotortempoffset;
        LatestDataInterpreted.ConverterRightMotorTemp=ValueF;


        // VCL Throttle
        Value2 = ((short)Msg.Daten[6]);
        Value2 += ((short)Msg.Daten[7])<<8;
        Value2 = utility::UnsignedIntToSignedInt(Value2, 15);
        ValueF = (float)Value2;
        ValueF *= config_handler::currentconfig.convertervclthrottlefactor;
        ValueF += config_handler::currentconfig.convertervclthrottleoffset;
        LatestDataInterpreted.ConverterRightVCLThrottle = ValueF;

        break;

    case 0x300: // lights
        LatestDataInterpreted.DirectionLeft=Msg.Daten[2] & 0x01;
        LatestDataInterpreted.DirectionRight=Msg.Daten[2] & 0x02;
        LatestDataInterpreted.Horn=Msg.Daten[2] & 0x04;
        LatestDataInterpreted.BackwardLight=Msg.Daten[2] & 0x08;
        LatestDataInterpreted.BrakeLight=Msg.Daten[2] & 0x10;
        LatestDataInterpreted.ForwardLight=Msg.Daten[2] & 0x20;
        LatestDataInterpreted.FullBeamLight=Msg.Daten[1] & 0x10;
        LatestDataInterpreted.WarningLight=Msg.Daten[1] & 0x40;

        break;
    case 0x170: // pedal information
        Value = Msg.Daten[0];	// accelerator
        Value += Msg.Daten[1] << 8;
        ValueF = (float)Value;
        ValueF /= 10;
        LatestDataInterpreted.acceleratorpercentage = ValueF;

        Value = Msg.Daten[6];   // brake
        Value += Msg.Daten[7] << 8;
        ValueF = (float)Value;
        ValueF /= 10;
        LatestDataInterpreted.brakepercentage = ValueF;
        break;

    case 0x180: // safety board Rear
        Value = Msg.Daten[2]; // current DC/DC converter
        Value += Msg.Daten[3] << 8;
        Value = utility::UnsignedIntToSignedInt(Value, 15);
        ValueF = (float)Value;
        ValueF *= config_handler::currentconfig.DCDCcurrentfactor;
        ValueF += config_handler::currentconfig.DCDCcurrentoffset;
        LatestDataInterpreted.DCDCcurrent = ValueF;

        Value = Msg.Daten[4]; // hv voltage
        Value += Msg.Daten[5] << 8;
        Value = utility::UnsignedIntToSignedInt(Value, 15);
        ValueF = (float)Value;
        ValueF *= config_handler::currentconfig.HVVfactor;
        ValueF += config_handler::currentconfig.HVVoffset;
        LatestDataInterpreted.HVV = ValueF;
        break;

    case 0x250:
        LatestDataInterpreted.SOC = Msg.Daten[0];     // SOC [0-100]
        LatestDataInterpreted.range_for_CAN =  Msg.Daten[1];
        LatestDataInterpreted.range_for_CAN =  LatestDataInterpreted.range_for_CAN << 8;
        LatestDataInterpreted.range_for_CAN +=  Msg.Daten[2];
        LatestDataInterpreted.SPI_error = Msg.Daten[3];
        LatestDataInterpreted.SPI_log = Msg.Daten[4];
        LatestDataInterpreted.BMS_Errors = Msg.Daten[5];
        LatestDataInterpreted.SHUNT_error = Msg.Daten[6];
        LatestDataInterpreted.RELAY_enable = Msg.Daten[7];
        break;

    case 0x251:
        LatestDataInterpreted.current_for_CAN = Msg.Daten[0];
        LatestDataInterpreted.current_for_CAN = LatestDataInterpreted.current_for_CAN << 8;
        LatestDataInterpreted.current_for_CAN +=  Msg.Daten[1];
        LatestDataInterpreted.voltage_Stack_for_CAN = Msg.Daten[2];
        LatestDataInterpreted.voltage_Stack_for_CAN = LatestDataInterpreted.voltage_Stack_for_CAN << 8;
        LatestDataInterpreted.voltage_Stack_for_CAN +=  Msg.Daten[3];
        LatestDataInterpreted.voltage_cell_highest_for_CAN = Msg.Daten[4];
        LatestDataInterpreted.voltage_cell_highest_for_CAN = LatestDataInterpreted.voltage_cell_highest_for_CAN << 8;
        LatestDataInterpreted.voltage_cell_highest_for_CAN +=  Msg.Daten[5];
        LatestDataInterpreted.voltage_cell_lowest_for_CAN= Msg.Daten[6];
        LatestDataInterpreted.voltage_cell_lowest_for_CAN = LatestDataInterpreted.voltage_cell_lowest_for_CAN << 8;
        LatestDataInterpreted.voltage_cell_lowest_for_CAN +=  Msg.Daten[7];
        break;

    case 0x252:
        LatestDataInterpreted.max_Temp = Msg.Daten[0];
        break;

        /*can message from 453-456 read in mV*/
    case 0x453:
        LatestDataInterpreted.battery_cell[0] = Msg.Daten[0];
        LatestDataInterpreted.battery_cell[0] = LatestDataInterpreted.battery_cell[0] << 8;
        LatestDataInterpreted.battery_cell[0] += Msg.Daten[1];
        LatestDataInterpreted.battery_cell[1] = Msg.Daten[2];
        LatestDataInterpreted.battery_cell[1] = LatestDataInterpreted.battery_cell[1] << 8;
        LatestDataInterpreted.battery_cell[1] += Msg.Daten[3];
        LatestDataInterpreted.battery_cell[2] = Msg.Daten[4];
        LatestDataInterpreted.battery_cell[2] = LatestDataInterpreted.battery_cell[2] << 8;
        LatestDataInterpreted.battery_cell[2] += Msg.Daten[5];
        LatestDataInterpreted.battery_cell[3] = Msg.Daten[6];
        LatestDataInterpreted.battery_cell[3] = LatestDataInterpreted.battery_cell[3] << 8;
        LatestDataInterpreted.battery_cell[3] += Msg.Daten[7];
        break;

    case 0x454:
        LatestDataInterpreted.battery_cell[4] = Msg.Daten[0];
        LatestDataInterpreted.battery_cell[4] = LatestDataInterpreted.battery_cell[0] << 8;
        LatestDataInterpreted.battery_cell[4] += Msg.Daten[1];
        LatestDataInterpreted.battery_cell[5] = Msg.Daten[2];
        LatestDataInterpreted.battery_cell[5] = LatestDataInterpreted.battery_cell[1] << 8;
        LatestDataInterpreted.battery_cell[5] += Msg.Daten[3];
        LatestDataInterpreted.battery_cell[6] = Msg.Daten[4];
        LatestDataInterpreted.battery_cell[6] = LatestDataInterpreted.battery_cell[2] << 8;
        LatestDataInterpreted.battery_cell[6] += Msg.Daten[5];
        LatestDataInterpreted.battery_cell[7] = Msg.Daten[6];
        LatestDataInterpreted.battery_cell[7] = LatestDataInterpreted.battery_cell[3] << 8;
        LatestDataInterpreted.battery_cell[7] += Msg.Daten[7];
        break;

    case 0x455:
        LatestDataInterpreted.battery_cell[8] = Msg.Daten[0];
        LatestDataInterpreted.battery_cell[8] = LatestDataInterpreted.battery_cell[0] << 8;
        LatestDataInterpreted.battery_cell[8] += Msg.Daten[1];
        LatestDataInterpreted.battery_cell[9] = Msg.Daten[2];
        LatestDataInterpreted.battery_cell[9] = LatestDataInterpreted.battery_cell[1] << 8;
        LatestDataInterpreted.battery_cell[9] += Msg.Daten[3];
        LatestDataInterpreted.battery_cell[10] = Msg.Daten[4];
        LatestDataInterpreted.battery_cell[10] = LatestDataInterpreted.battery_cell[2] << 8;
        LatestDataInterpreted.battery_cell[10] += Msg.Daten[5];
        LatestDataInterpreted.battery_cell[11] = Msg.Daten[6];
        LatestDataInterpreted.battery_cell[11] = LatestDataInterpreted.battery_cell[3] << 8;
        LatestDataInterpreted.battery_cell[11] += Msg.Daten[7];
        break;

    case 0x456:
        LatestDataInterpreted.battery_cell[12] = Msg.Daten[0];
        LatestDataInterpreted.battery_cell[12] = LatestDataInterpreted.battery_cell[0] << 8;
        LatestDataInterpreted.battery_cell[12] += Msg.Daten[1];
        LatestDataInterpreted.battery_cell[13] = Msg.Daten[2];
        LatestDataInterpreted.battery_cell[13] = LatestDataInterpreted.battery_cell[1] << 8;
        LatestDataInterpreted.battery_cell[13] += Msg.Daten[3];
        LatestDataInterpreted.battery_cell[14] = Msg.Daten[4];
        LatestDataInterpreted.battery_cell[14] = LatestDataInterpreted.battery_cell[2] << 8;
        LatestDataInterpreted.battery_cell[14] += Msg.Daten[5];
        LatestDataInterpreted.battery_cell[15] = Msg.Daten[6];
        LatestDataInterpreted.battery_cell[15] = LatestDataInterpreted.battery_cell[3] << 8;
        LatestDataInterpreted.battery_cell[15] += Msg.Daten[7];
        break;
    }

    // calculate speed
    LatestDataInterpreted.speed =
            (double)(LatestDataInterpreted.ConverterLeftMotorRPM+LatestDataInterpreted.ConverterRightMotorRPM)
            / 2 * config_handler::currentconfig.tirecircmm * 6 / 7 / 100000;

    // calculate accelerator percentage
    // if fallback is active (will overwrite values send from car)
    if (config_handler::currentconfig.acceleratorfallback)
    {
        if (config_handler::currentconfig.acceleratorrange > 0)
        {
            // only calculate the accelerator position if the value of range is greater than 0
            // to avoid a division through zero
            LatestDataInterpreted.acceleratorpercentage = (LatestDataInterpreted.rawacceleratorangle-
                                                           config_handler::currentconfig.acceleratoroffset) / config_handler::currentconfig.acceleratorrange;
        }
        else
        {
            LatestDataInterpreted.acceleratorpercentage = 0;
        }
    }

    // check if the data is in the correct range and correct them if they are not
    if ( LatestDataInterpreted.acceleratorpercentage < 0)
    {
        LatestDataInterpreted.acceleratorpercentage = 0;
    }
    if (LatestDataInterpreted.acceleratorpercentage > 100)
    {
        LatestDataInterpreted.acceleratorpercentage = 100;
    }
    return;
}

void InterpreteThread::frontTimedOut()
{

    frontok=false;

}
void InterpreteThread::rearTimedOut()
{
    rearok=false;
}

void InterpreteThread::init_LatestDataInterpreted(struct InterpretedData *i){ptr_LatestDataInterpreted = i;}
void InterpreteThread::init_receivedMsgVector(std::vector <Ethernet_Msg> *i){ptr_receivedMsgVector = i;}
void InterpreteThread::init_LatestRawData(struct RawData *i){ptr_LatestRawData = i;}
void InterpreteThread::init_AvailableData(int *i){ptr_AvaibleData = i;}
//void InterpreteThread::init_JsonList(QJsonObject *i){ptr_JsonList = i;}



