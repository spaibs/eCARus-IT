#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QVector>
#include <QTableWidgetItem>
#include <QLabel>
#include "datatypes.h"
#include "interpretethread.h"
#include "periodicsendworker.h"
#include "remotethread.h"
#include "udpthread.h"
#include "ui_mainwindow.h"
#include <special_label.h>
#include <QGraphicsPixmapItem>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QFile>

class QUdpSocket;
class InterpreteThread;
class RemoteThread;
class PeriodicSendThread;
class UDPThread;

extern QMutex* GUIDataLock;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    int DMX_Send(int Kanal, unsigned char Wert);
    void closeEvent(QCloseEvent *);

    struct InterpretedData LatestDataInterpreted;
    struct RawData	LatestRawData;
    std::vector <Ethernet_Msg> receivedMsgVector;  	///< GUI Version of all important datatypes
    QString RemoteHost; 		///< stores address and Port of the remote Port
    QString RemotePort;
    long Time_ms [2048];	///< stores times for all can ids
    int AvailableData;		///< can ids can be in the range 0 - 0x7FF
    int RemoteStatusGUI;
    int RemoteStatusEXT;
    bool RemoteHeadLightBool;
    bool RemoteFullBeamBool;
    bool RemoteDirIndLeftBool;
    int UnsignedIntToSignedInt(int unsignedint, int signbitnumber);
    bool RemoteDirIndRightBool;
    bool RemoteWarnLightBool;
    bool RemoteEmStopBool;
    int RemoteGearChange;
    bool RemoteHeadLightFlashBool;
    bool RemoteTerminated;
    int RemoteGear;
    int RemoteSetting_1;
    int RemoteSetting_2;
    int RemoteSetting_3;
    //JsonInit   
    QJsonObject JsonList;
    /**
     * @brief GetJSONInterpretation
     * interpretes incoming messages by the help of a Json config file and creates a QMap out of the data     *
     */
    void GetJSONInterpretation(QString ID, unsigned char data[]);
    void needleRotate(int); ///< rotates the speedometer's needle depending on the passed speed paramater
    void tiresRotate(int);  ///< rotates eCARus' tires depending on the passed steering value
    void arrive_Animation();///< animate ecarus' arriving to the GUI and hides key assignment widget
    void depart_Animation();
public slots:
    /**
     * @brief UpdateGui updates elements of the GUI every 100ms     *
     */
    void UpdateGui();
//RemoteController-Slots
    void ChangeGearList(int);
    void ChangeGearList_Released(int);
    void on_Connect_Button_clicked(void);
    void on_startRemoteButton_clicked(void); ///< activates the remote control and setups connections to the remote thread
    void on_stopRemoteButton_clicked(void);
    void on_warningButton_clicked(void);
    void on_warningButton_Released(void);
    void on_indicatorRightButton_clicked(void);
    void on_indicatorRightButton_Released(void);
    void on_indicatorLeftButton_clicked(void);
    void on_indicatorLeftButton_Released(void);
    void on_fullBeamButton_clicked(void);
    void on_fullBeamButton_Released(void);
    void on_headLightButton_clicked(void);
    void on_headLightButton_Released(void);
    void on_brakeButton_clicked(int);
    void on_accelerateButton_clicked(int);
    void on_hornButton_clicked(void);
    void on_hornButton_Released(void);
    void on_steering(int);
    void arrive_Animation_finished();
    void depart_Animation_finished();
//Special_Label_Slots
    void on_helpWindowMouseEntered();
    void on_helpWindowMouseLeaved();
    void on_helpWindowMouseClicked();
//Other Slots
    void on_ethernetAARadioButton_clicked();
    void on_ethernetCustomRadioButton_clicked();
    void on_checksumComputeRadioButton_clicked();
    void on_checksumCustomRadioButton_clicked();
    /**
     * @brief on_DebugSendNowButton_clicked
     *  This function allows the user to either enter a ethernet message that will be sent
     *  or to enter a can message and set all settings to automaticaly generate a ethernet
     *  message that will be sent.
     */
    void on_DebugSendNowButton_clicked();
    void on_DebugAddButton_clicked();
    void on_DebugToggleActiveButton_clicked();
    void on_DebugRemoveButton_clicked();
    void on_DebugEnableCheckBox_clicked();
    void on_DebugManualRadioButton_clicked();
    void on_DebugGuidedRadioButton_clicked();
    void on_DebugNoneRadioButton_clicked();
    void on_modeComboBox_currentIndexChanged(int index);
    void on_recupComboBox_currentIndexChanged(int index);
    void on_boostComboBox_currentIndexChanged(int index);
    void on_CAL_StartButton_clicked();
    void on_CAL_ContinueButton_clicked();
    void on_CreateLogButton_clicked();
    void on_maxSpeedSlider_actionTriggered(int action);
    void on_maxSpeedSlider_sliderMoved(int position);
    void on_maxSpeedSlider_valueChanged(int value);
    void on_actionQuit_triggered();
    void on_actionGet_Help_triggered();
    void on_actionAbout_triggered();
    void on_actionConnect_Disconnect_triggered();
   // void on_pushButton_StartMoitoring_clicked();
    void on_Development_Button_clicked();
    //void on_pushButton_clicked();
    void on_horizontalSlider_valueChanged(int value);    
    void on_keyAssignment_Button_clicked();

private slots:
    void on_Export_Button_clicked();
    void on_SaveLogButton_clicked();
    void on_RemoteStartButton_clicked();

private:
    QMovie *connection_Animation;
    bool connecting;
    bool movieIsPaused;
    int GUIUpdateCount;///< increments each update of the GUI
    std::vector<int> liveDataDelayVector;
    std::vector<int> liveRawDelayVector;
    void startConfiguration();
    /**
     * @brief DebugDecodeInput
     * @param RawData* msg
     * @return 0=success,1=error
     *
     *  Decodes the Data from the form (debug tab) and stores it in RawData* msg.
     *  Return Values:
     *  0		SUCESS
     *  1		ERROR (a error message will be displayed by this function)
     */
    int DebugDecodeInput (RawData* msg);
    int	CALstep; // stores in what step of the calibration the user is currently
    int blink_delaycounter;
    bool indicator_left;
    bool indicator_right;
    bool warning_light;
    bool arrive_Animation_started;
    Ui::MainWindow *ui;
    QTimer *timer;
    InterpreteThread *interprete_thread;
    RemoteThread *remote_thread;
    QThread *peri_Thread;
    QTimer* peri_Timer;
    PeriodicSendWorker* peri_worker;
    QMap<QString,QString> raw_messages;
    QMap<QString,QString> Json_Map;
    QVector<QTableWidgetItem*> tableWidgetItem_vector;
        QVector<QTableWidgetItem*> JsontableWidgetItem_vector;
    std::vector<float> cell_voltage;

    QImage greenLabel;
    QImage redLabel;
    QImage Battery_full;
    QImage Battery_half;
    QImage Battery_low;
    QImage Battery_none;
    QImage Headlight_on;
    QImage Headlight_off;
    QImage Fullbeam_on;
    QImage Fullbeam_off;
    QImage RightIndicator_on;
    QImage RightIndicator_off;
    QImage LeftIndicator_on;
    QImage LeftIndicator_off;
    QImage Warning_on;
    QImage Warning_off;

    QLabel *connected_label;
    QLabel *connected_false;
    QLabel *rearOkLabel;
    QLabel *frontOkLabel;

    QGraphicsPixmapItem* speedometer_GraphicsItem;
    QGraphicsPixmapItem* speedometer_Needle_GraphicsItem ;
    QGraphicsPixmapItem* tireLeft_GraphicsItem;
    QGraphicsPixmapItem* tireRight_GraphicsItem;
};

#endif // MAINWINDOW_H
