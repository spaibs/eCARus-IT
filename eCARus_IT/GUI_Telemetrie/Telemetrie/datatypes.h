#ifndef DATATYPES_H
#define DATATYPES_H

#include <QString>
#include <QDateTime>

typedef struct {
    int ID;
    unsigned char Daten[8];
    long Time;
    long cycle;
} Ethernet_Msg;

struct RawData
{
    /* used to store raw data */
    QDateTime timestamp;
    unsigned char RawData[14];
    long cycle;
    int numberofreceiveddata;
};

struct LogCommand
{
    /*
    used to store Data to be logged.
    */
    //TDateTime timestamp;
    //String log_text;
};
/**
 * @brief The InterpretedData struct is used to store all interpreted data read from the network
 *
 */
struct InterpretedData {


    QDateTime timestamp;
    int EngineSwitch;
    int MasterSwitch;
    int RunlevelK60rear;
    int RunlevelK60front;
    int rawacceleratorangle;
    int rawacceleratorangleGray;
    int gear;
    int TorqueLeft;
    int BrakingTorqueLeft;
    int TorqueRight;
    int BrakingTorqueRight;
    int erroridK60rear;
    int erroridK60front;
    bool EthernetOkFront;
    bool EthernetOkRear;

    // left controller
    float ConverterLeftBatteryCurrent;		// in A
    float ConverterLeftCapacitorVoltage;    // in V
    float ConverterLeftControllerTemp;      // in �C
    float ConverterLeftCurrentRMS;          // in A
    float ConverterLeftCurrentRequest;      // in A
    int ConverterLeftMotorRPM;              // in RPM
    float ConverterLeftMotorTemp;           // in �C
    float ConverterLeftVCLThrottle;         // in %

    // right controller
    float ConverterRightBatteryCurrent;		// in A
    float ConverterRightCapacitorVoltage;   // in V
    float ConverterRightControllerTemp;     // in �C
    float ConverterRightCurrentRMS;         // in A
    float ConverterRightCurrentRequest;     // in A
    int ConverterRightMotorRPM;             // in RPM
    float ConverterRightMotorTemp;          // in �C
    float ConverterRightVCLThrottle;        // in %

    // security board
    float DCDCcurrent;						// in A
    float HVV;								// in V
    // light
    bool DirectionLeft;
    bool DirectionRight;
    bool Horn;
    bool BackwardLight;
    bool BrakeLight;
    bool ForwardLight;
    bool WarningLight;
    bool FullBeamLight;

    double speed; // in km/h
    double acceleratorpercentage;	// in %
    double brakepercentage;			// in %

    //BMS Battery Management System
    int SOC;
    int range_for_CAN;
    int SPI_error;
    int SPI_log;
    int BMS_Errors;
    int SHUNT_error;
    int RELAY_enable;
    int current_for_CAN;            //curent in 1/10 A per Bit
    int voltage_Stack_for_CAN;
    int voltage_cell_highest_for_CAN;
    int voltage_cell_lowest_for_CAN;
    int max_Temp;
    int battery_cell[16];
};

typedef struct {
    // used for settings

    QString networkADR;
    QString networkPORT;

    double tirecircmm;

    // accelerator fallback options
    bool acceleratorfallback;
    int acceleratoroffset;
    int acceleratorrange;

    double converterbatterycurrentfactor;
    double converterbatterycurrentoffset;
    double convertercapacitorvoltagefactor;
    double convertercapacitorvoltageoffset;
    double convertercontrollertempfactor;
    double convertercontrollertempoffset;
    double convertercurrentrmsfactor;
    double convertercurrentrmsoffset;
    double convertercurrentrequestfactor;
    double convertercurrentrequestoffset;
    int convertermotorrpmfactor;
    int convertermotorrpmoffset;
    double convertermotortempfactor;
    double convertermotortempoffset;
    double convertervclthrottlefactor;
    double convertervclthrottleoffset;

    double DCDCcurrentfactor;
    double DCDCcurrentoffset;
    double HVVfactor;
    double HVVoffset;
} configdata;

typedef struct {        ///<will store data that will be send periodicaly

    int ID;             ///< ID for the data (not to be confused with Can ID)
    unsigned char data [14]; ///< data that will be sent
    double interval;    ///< in ms
    double lastsent;    ///< in ms, set to 0 when it was never sent
    bool active;        ///< data will sent if active is true
    char creation;      ///< will be set to 1 if it was created with the manual function and to 2 if it was created with the guided function

} PeriodicSendObject;

typedef struct {
    QDateTime timestamp;
    double	  ticks;
    unsigned char  data[14];
} RecvBuf;

#endif // DATATYPES_H
