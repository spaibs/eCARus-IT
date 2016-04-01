#ifndef REMOTETHREAD_H
#define REMOTETHREAD_H

#include <QThread>
#include <mainwindow.h>
#include <QObject>
//#include "ui_mainwindow.h"

extern QMutex* RemoteLock;
/**
 * @brief The RemoteThread class
 * 	1: connection setup between Xbox_Controller and mainwindow : RemoteStatusEXT=1;
 *   2: if a connection to eCARus exists, a controller command will be sent to the car;

 */
class RemoteThread : public QThread
{
    Q_OBJECT
public:
    explicit RemoteThread(QObject *parent = 0);

    void run();

    // mutators
    //void setMainWindow(Ui::MainWindow *w);
    void RemoteStatusGUI(int);
    void RemoteStatusEXT(int);
    void RemoteEmStopBool(bool);
    void RemoteGearChange(int);
    void RemoteHeadLightFlashBool(bool);
    void Terminate();
    void toggleHeadLight();
    void toggleFullBeam();
    void toggleIndicatorLeft();
    void toggleIndicatorRight();
    void toggleWarningLight();
    void turnOffHeadLight();
    void turnOffFullBeam();
    void turnOffIndicatorLeft();
    void turnOffIndicatorRight();
    void turnOffWarningLight();

    //accessors
    int RemoteStatusGUI();
    int RemoteStatusEXT();
    bool RemoteEmStopBool();
    int RemoteGearChange();
    bool RemoteHeadLightFlashBool();
    bool Terminated();
    bool headLightIsOn();
    bool fullBeamIsOn();
    bool indicatorLeftIsOn();
    bool indicatorRightIsOn();
    bool warningLightIsOn();

    //pointer initiation
    void init_RemoteStatusGUI(int*);
    void init_RemoteStatusEXT(int*);
    void init_headLightIsOn(bool*);
    void init_fullBeamIsOn(bool*);
    void init_indicatorLeftIsOn(bool*);
    void init_indicatorRightIsOn(bool*);
    void init_warningLightIsOn(bool*);
    void init_RemoteEmStopBool(bool*);
    void init_RemoteGearChange(int*);
    void init_RemoteHeadLightFlashBool(bool*);
    void init_Terminated(bool*);
    void init_RemoteGear(int*);
    void init_RemoteSetting_1(int*);
    void init_RemoteSetting_2(int*);
    void init_RemoteSetting_3(int*);

    float MaxSpeed;

private:
    int* RemoteStatusGUI_;
    int* RemoteStatusEXT_;
    bool* headLightIsOn_;
    bool* fullBeamIsOn_;
    bool* indicatorLeftIsOn_;
    bool* indicatorRightIsOn_;
    bool* warningLightIsOn_;
    bool* RemoteEmStopBool_;
    int* RemoteGearChange_;
    int* RemoteGear_;
    int* RemoteSetting_1_;
    int* RemoteSetting_2_;
    int* RemoteSetting_3_;
    bool* RemoteHeadLightFlashBool_;
    bool* Terminated_;

signals:
    void gear_change(int);
    void gear_change_Released(int);
    void xButton(void);
    void xButton_Released(void);
    void yButton(void);
    void yButton_Released(void);
    void rightTrigger(void);
    void rightTrigger_Released(void);
    void leftTrigger(void);
    void leftTrigger_Released(void);
    void startButton(void);
    void startButton_Released(void);
    void backButton(void);
    void steering(int);
    void accelerate(int);
    void brake(int);
    void horn(void);
    void horn_Released(void);
};


#endif // REMOTETHREAD_H
