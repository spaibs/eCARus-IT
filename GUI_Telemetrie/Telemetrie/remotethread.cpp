#include "remotethread.h"
#include "utility.h"
#include <XInput.h>
#include <windows.h>
#include <QMessageBox>

QMutex* RemoteLock;

RemoteThread::RemoteThread(QObject *parent) :
    QThread(parent)
{
    MaxSpeed = 0;
}

void RemoteThread::run()
{
    /* load the xinput libary . If this fails the programm will not execute. */

#ifdef _WIN64
    /*
             For Windows 64:
             Load the xinput dynamically.
             If this fails the application will disable the remote controll function
         */

    HINSTANCE hDLL;               // Handle to DLL
    DWORD (*XInputGetState)(DWORD, _XINPUT_STATE*);    // Function pointer
    hDLL = LoadLibrary(L"xinput9_1_0.dll");
    if (hDLL == NULL)
    {
        RemoteStatusEXT(-1); // DLL load error
        //QMessageBox::warning(mainwindow, "Error", "Could not load xinput9_1_0.dll");
        this->Terminate();
        return;
    }
    // load the GetState Function from the DLL
    XInputGetState = (DWORD(*)(DWORD,XINPUT_STATE*))GetProcAddress(hDLL, "XInputGetState");
    if ( XInputGetState == NULL )
    {
        // check if the function is loaded correctly
        RemoteStatusEXT(-1); // DLL load error
        //QMessageBox::warning(mainwindow, "Error", "xinput9_1_0.dll was loaded successfully, however the DLL does not contain the required function(s)");
        FreeLibrary(hDLL);
        this->Terminate();
        return;
    }
#endif

    DWORD dwResult; // return value for xinputgetstate
    _XINPUT_STATE controllerstate; // status of all controller buttons
    char tmp[8] = {0}; // the message that will be sent
    char MessageCount = 0; // counts all messages that have been sent to be included
    // in the messages
    long LongCount=0;		   // longer counter for internal usage
    char emsdata[8] = {0xC0}; // contains data that will be send in case of emergency shutdown
    long ShoulderLeftCount=0;
    long ShoulderRightCount=0;
    long StartBtnCount=0;
    long UpBtnCount=0;
    long DownBtnCount=0;
    long BackBtnCount=0;
    long XBtnCount=0;
    long YBtnCount=0;
    long AcclrtBtnCount=0;
    long StrngBtnCount=0;
    long HrnBtnCount=0;
    long BrkBtnCount=0;
    int previousacceleratorvalue=0;
    while (true)
    {
        // read user input from controller
        /*FIXME: Doesn't work when compiled with MINGW compiler (if _WIN32 defined)
          *       because XInput library is not loaded due to the fact that MINGW compiler
          *       ignores above #pragma comment(lib, "XInput"). To solve the problem either compile the
          *       project using the Visual Studio compiler or find another way to
          *       load the XInput library.
          */
        dwResult = XInputGetState(0, &controllerstate); // Update the data from the controller

        // check for problems
        if ( dwResult == ERROR_DEVICE_NOT_CONNECTED )
        {
            if ( (RemoteStatusEXT() == 0) || (RemoteStatusEXT() == -2) || (RemoteStatusEXT() == -3) )
            {
                // if there is no active controller do nothing if the remote function
                // is idle or the error was already present
                RemoteStatusEXT(-2); // no controller
            }
            else
            {
                //  start emergency shutdown
                emsdata[7] = MessageCount;
                emsdata[0] = 0xC0; // emergency
                utility::SendCanMessage (0x190, emsdata);  // send emergency stop
                MessageCount++;

                emsdata[0] = 0xF0;	// emergency + disconnect
                emsdata[7] = MessageCount;
                utility::SendCanMessage (0x190, emsdata); // stop remote mode

                RemoteStatusEXT(-2); // no controller
            }
        }
        if ( (dwResult != ERROR_SUCCESS) && (dwResult != ERROR_DEVICE_NOT_CONNECTED) )
        {
            if ( (RemoteStatusEXT() == 0) || (RemoteStatusEXT() == -2) || (RemoteStatusEXT() == -3) )
            {
                // if there is no active controller do nothing if the remote function
                // is idle or the error was already present
                RemoteStatusEXT(-3); // other error
                // maybe we should stop the thread?
            }
            else
            {
                //  start emergency shutdown
                emsdata[7] = MessageCount;
                emsdata[0] = 0xC0; // emergency
                utility::SendCanMessage (0x190, emsdata);  // send emergency stop
                MessageCount++;

                emsdata[0] = 0xF0; // emergency + shutdown
                emsdata[7] = MessageCount;
                utility::SendCanMessage (0x190, emsdata); // stop remote mode

                RemoteStatusEXT(-3); // other error
            }
        }

        if ( RemoteEmStopBool() )
        {
            // switch gui status and reset Emergency button
            RemoteStatusGUI(0);
            RemoteEmStopBool(false);

            // start emergency shutdown only if there is a connection
            if ( RemoteStatusEXT() > 0 )
            {
                //  start emergency shutdown
                emsdata[0] = 0xC0; // emergency
                emsdata[7] = MessageCount;
                utility::SendCanMessage (0x190, emsdata);  // send emergency stop
                MessageCount++;

                emsdata[0] = 0xF0; // emergency + shutdown
                emsdata[7] = MessageCount;
                utility::SendCanMessage (0x190, emsdata); // stop remote mode

                RemoteStatusEXT(0); // standby
            }
        }
        if ( (dwResult == ERROR_SUCCESS) && (RemoteStatusEXT() < 0) )
        {
            /*
                 if the system is ready after it recovered from an error set status
                 to standby
             */
            RemoteStatusEXT(0); // standby
        }

        // read input from form 1 and send messages
        // init message
        for (int i = 0; i < 8; i++)
        {
            tmp[i] = 0;
        }

        // check if the user wants to toolge something
        if ( controllerstate.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) // direction indicator left
        {
            if ( (ShoulderLeftCount +1) != LongCount )
            {
                // only toogle when the button was released
                emit leftTrigger();
            }
            ShoulderLeftCount = LongCount; // update
        }
        if(ShoulderLeftCount +2 == LongCount && LongCount!=2)
            emit leftTrigger_Released();
        if ( controllerstate.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) // direction indicator right
        {
            if ( (ShoulderRightCount +1) != LongCount )
            {
                // only toogle when the button was released
                emit rightTrigger();
            }
            ShoulderRightCount = LongCount; // update
        }
        if(ShoulderRightCount +2 == LongCount && LongCount!=2)
            emit rightTrigger_Released();
        if ( controllerstate.Gamepad.wButtons & XINPUT_GAMEPAD_START ) // warn light
        {
            if ( (StartBtnCount +1) != LongCount )
            {
                // only toogle when the button was released
                emit startButton();
            }
            StartBtnCount = LongCount; // update
        }
        if((StartBtnCount+2)==LongCount)
            emit startButton_Released();

        if ( controllerstate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP ) // gear up
        {
            if ( (UpBtnCount +1) != LongCount )
            {
                // only set when the button was released
                emit gear_change(-1);
                RemoteGearChange(-1);
            }
            UpBtnCount = LongCount; // update
        }
        if(UpBtnCount +2 == LongCount && LongCount!=2)
             emit gear_change_Released(-1);

        if ( controllerstate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN ) // gear down
        {
            if ( (DownBtnCount +1) != LongCount )
            {
                // only set when the button was released
                emit gear_change(1);
                RemoteGearChange(1);
            }
            DownBtnCount = LongCount; // update
        }
        if(DownBtnCount +2 == LongCount && LongCount!=2)
             emit gear_change_Released(1);

        if ( controllerstate.Gamepad.wButtons & XINPUT_GAMEPAD_BACK ) // Emergency stop
        {
            if ( (BackBtnCount +1) != LongCount )
            {
                // only set when the button was released
                emit backButton();
                RemoteEmStopBool(1);
            }
            BackBtnCount = LongCount; // update
        }


        if ( controllerstate.Gamepad.wButtons & XINPUT_GAMEPAD_X ) // Head Lights
        {
            if ( (XBtnCount +1) != LongCount )
            {
                // only toggle when the button was released
                // switch off Full Beam if Head Lights are off
                emit xButton();
                if(!headLightIsOn())
                    turnOffFullBeam();
            }
            XBtnCount = LongCount; // update
        }
        if(XBtnCount +2 == LongCount && LongCount!=2)
             emit xButton_Released();
        if ( controllerstate.Gamepad.wButtons & XINPUT_GAMEPAD_Y ) // Full Beam
        {
            if ( ((YBtnCount +1) != LongCount ) && headLightIsOn() )
                // only activate when Head Lights are enabled
            {
                // only toggle when the button was released
                emit yButton();
            }
            YBtnCount = LongCount; // update
        }
        if(YBtnCount +2 == LongCount && LongCount!=2)
             emit yButton_Released();

        if(controllerstate.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)            //horn
        {
            if ( (HrnBtnCount +1) != LongCount )
            {
                // only toggle when the button was released
                emit horn();
            }
            HrnBtnCount = LongCount; // update
        }
        if(HrnBtnCount +2 == LongCount && LongCount!=2)
             emit horn_Released();


        // set Head Light Flash Value (for "LED")
        RemoteHeadLightFlashBool((controllerstate.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB));
        LongCount++; // update the counter
        if ( (RemoteStatusGUI() == 1) && (RemoteStatusEXT() == 0) ) // start
        {
            MessageCount = 0; // reset Message Counter
            previousacceleratorvalue=0;
            tmp[0] = 0x03; // start remote mode
            tmp[7] = MessageCount;

            RemoteStatusEXT(1);
            // send (enque) message
            utility::SendCanMessage(0x190, tmp);
            MessageCount++;

            QThread::msleep(20);
            continue;
        }
        if (RemoteStatusEXT() < 0)
        {
            // errors get handled above
            QThread::msleep(40);
            continue;
        }
        if ( RemoteStatusGUI() == -1 ) // disconnected
        {
            // there is basically nothing we can do since there is no connection.
            // we will not send anymore messages.
            RemoteStatusEXT(0); // standby
            QThread::msleep(40);
            continue;
        }
        if ( (RemoteStatusGUI() == 1) && (RemoteStatusEXT() == 1) )           //running
        {
            // apply the toogleable values
            tmp[1] += headLightIsOn() ? 0x1 : 0; // Head Light
            //            tmp[1] += (fullBeamIsOn() || RemoteHeadLightFlashBool()) ? 0x2: 0; // Full Beam
            tmp[1] += (fullBeamIsOn()) ? 0x2: 0; // Full Beam
            tmp[1] += indicatorLeftIsOn() ? 0x4: 0;	// direction indicator left
            tmp[1] += indicatorRightIsOn() ? 0x8: 0; // direction indicator right
            tmp[1] += warningLightIsOn() ? 0x10 : 0; // warning light
            tmp[1] += (*RemoteGear_ == 1) ? 0x20 : 0; // gear (1)
            tmp[5] += (*RemoteGear_ == 1) ? 0x01 : 0;
            tmp[1] += (*RemoteGear_ == 2) ? 0x40 : 0; // gear (-1)
            tmp[5] += (*RemoteGear_ == 2) ? 0x02 : 0;
            tmp[2] += (*RemoteSetting_1_ == 1) ? 0x1: 0; // es mode (S)
            tmp[2] += (*RemoteSetting_2_ == 1) ? 0x2: 0; // Recuperation Mode (2)
            tmp[2] += (*RemoteSetting_3_ == 1) ? 0x4: 0; // Boost Mode

            // direct transfer digital values
            tmp[0] += 0x0C; // remote enabled (Emergency is evalueted above)
            tmp[1] += (controllerstate.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) ? 0x80 : 0; // horn

            // analog values
            /**
             * @brief called when the value of the bright trigger changes
             * @param value position of the slider
             *
             * langert text.( it linearizes the  characteristic curve  of the accelerator )
             */

            if (controllerstate.Gamepad.bRightTrigger * MaxSpeed > previousacceleratorvalue+10 )  {
                tmp[3]= previousacceleratorvalue+10;
                previousacceleratorvalue=previousacceleratorvalue+10;
            }
            else {tmp[3] = controllerstate.Gamepad.bRightTrigger * MaxSpeed;
                previousacceleratorvalue=controllerstate.Gamepad.bRightTrigger * MaxSpeed; }

            emit accelerate(tmp[3]);

            tmp[4] = controllerstate.Gamepad.bLeftTrigger; // brake
            emit brake(controllerstate.Gamepad.bLeftTrigger);

            if ( (controllerstate.Gamepad.sThumbLX <= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) && (controllerstate.Gamepad.sThumbLX >= -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) )
            {
                tmp[6] = 128; // steering (in deadzone)
                emit steering(128);
            }
            else if (controllerstate.Gamepad.sThumbLX > 32767)
            {
                tmp[6] = 0xFF; // steering (out of allowed area)
                emit steering(255);
            }
            else if (controllerstate.Gamepad.sThumbLX < -32767)
            {
                tmp[6] = 0x00; // steering (out of allowed area)
                emit steering(0);
            }
            else
            {
                tmp[6] = controllerstate.Gamepad.sThumbLX * 127 / 32767 + 128; // steering
                emit steering(controllerstate.Gamepad.sThumbLX * 127 / 32767 + 128);
            }

            tmp[7] = MessageCount;

            // send
            utility::SendCanMessage(0x190,tmp);
            MessageCount++;
            QThread::msleep(20);
            continue;
        }
        if ( (RemoteStatusGUI() == 0) && (RemoteStatusEXT() == 1) ) // shutdown
        {
            tmp[0] = 0x30; // stop remote mode
            tmp[7] = MessageCount;
            utility::SendCanMessage(0x190, tmp);

            RemoteStatusEXT(0); // stopped
            QThread::msleep(20);
            continue;

        }
        if ( (RemoteStatusGUI() == 0) && (RemoteStatusEXT() == 0) ) // stopped
        {
            // do nothing
            QThread::msleep(40);
            continue;
        }
    }
}

/*-----------------------------------------------------------------------------------------------------------------------*/

// mutators
void RemoteThread::RemoteStatusGUI(int i) { *RemoteStatusGUI_ = i;}
void RemoteThread::RemoteStatusEXT(int i) { *RemoteStatusEXT_ = i;}
void RemoteThread::RemoteEmStopBool(bool b) { *RemoteEmStopBool_ = b;}
void RemoteThread::RemoteGearChange(int i) { *RemoteGearChange_ = i;}
void RemoteThread::RemoteHeadLightFlashBool(bool b) {*RemoteHeadLightFlashBool_ = b;}
void RemoteThread::Terminate() { *Terminated_ = true;}
void RemoteThread::toggleHeadLight() {*headLightIsOn_ = !*headLightIsOn_;}
void RemoteThread::toggleFullBeam() { *fullBeamIsOn_ = !*fullBeamIsOn_;}
void RemoteThread::toggleIndicatorLeft() { *indicatorLeftIsOn_ = !*indicatorLeftIsOn_;}
void RemoteThread::toggleIndicatorRight() { *indicatorRightIsOn_ = !*indicatorRightIsOn_;}
void RemoteThread::toggleWarningLight() { *warningLightIsOn_ = !*warningLightIsOn_;}
void RemoteThread::turnOffHeadLight() { *headLightIsOn_ = false;}
void RemoteThread::turnOffFullBeam() { *fullBeamIsOn_ = false;}
void RemoteThread::turnOffIndicatorLeft() { *indicatorLeftIsOn_ = false;}
void RemoteThread::turnOffIndicatorRight() { *indicatorRightIsOn_ = false;}
void RemoteThread::turnOffWarningLight() { *warningLightIsOn_ = false;}

// accessors
int RemoteThread::RemoteStatusGUI() { return *RemoteStatusGUI_;}
int RemoteThread::RemoteStatusEXT() { return *RemoteStatusEXT_;}
bool RemoteThread::RemoteEmStopBool() { return *RemoteEmStopBool_;}
bool RemoteThread::RemoteHeadLightFlashBool() { return *RemoteHeadLightFlashBool_;}
bool RemoteThread::Terminated() { return *Terminated_;}
bool RemoteThread::headLightIsOn() { return *headLightIsOn_;}
bool RemoteThread::fullBeamIsOn() { return *fullBeamIsOn_;}
bool RemoteThread::indicatorLeftIsOn() { return *indicatorLeftIsOn_;}
bool RemoteThread::indicatorRightIsOn() { return *indicatorRightIsOn_;}
bool RemoteThread::warningLightIsOn() { return *warningLightIsOn_;}

//initiation: mainwindow.cpp initiates the variables and passes their adresses to the RemoteThread
void RemoteThread::init_RemoteStatusGUI(int *i){RemoteStatusGUI_ = i;}
void RemoteThread::init_RemoteStatusEXT(int *i){RemoteStatusEXT_ = i;}
void RemoteThread::init_headLightIsOn(bool *b){headLightIsOn_ = b;}
void RemoteThread::init_fullBeamIsOn(bool *b){fullBeamIsOn_ = b;}
void RemoteThread::init_indicatorLeftIsOn(bool *b){indicatorLeftIsOn_ = b;}
void RemoteThread::init_indicatorRightIsOn(bool *b){indicatorRightIsOn_ = b;}
void RemoteThread::init_warningLightIsOn(bool *b){warningLightIsOn_ = b;}
void RemoteThread::init_RemoteEmStopBool(bool *b){RemoteEmStopBool_ = b;}
void RemoteThread::init_RemoteGearChange(int *i){RemoteGearChange_ = i;}
void RemoteThread::init_RemoteHeadLightFlashBool(bool *b){RemoteHeadLightFlashBool_ = b;}
void RemoteThread::init_Terminated(bool *b){Terminated_ = b;}
void RemoteThread::init_RemoteGear(int *i){RemoteGear_ = i;}
void RemoteThread::init_RemoteSetting_1(int *i){RemoteSetting_1_ = i;}
void RemoteThread::init_RemoteSetting_2(int *i){RemoteSetting_2_ = i;}
void RemoteThread::init_RemoteSetting_3(int *i){RemoteSetting_3_ = i;}
