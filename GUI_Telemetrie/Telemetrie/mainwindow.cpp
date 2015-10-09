#include "mainwindow.h"

#include "configuration.h"
#include "interpretethread.h"
#include "periodicsendworker.h"
#include "remotethread.h"
#include "udpthread.h"
#include "utility.h"
//#include "secdialog.h"
#include <iostream>
#include <windows.h>
#include <QtWidgets>
#include <QtNetwork>
#include <QPixmap>
#include <QLabel>
#include <QIcon>
#include <QPushButton>
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QString>
#include "helpwindow.h"
#include <QHoverEvent>
#include <iostream>
#include <mouse_label.h>
#include <QLabel>
#include <QGraphicsView>
#include <special_label.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

QMutex* GUIDataLock;

UDPThread *TCP_Thread = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    cell_voltage(16,-1)

{
    ui->setupUi(this);

    ui->Label_Gear_N->setToolTip("Gear: Neutral");
    ui->Label_Gear_1->setToolTip("Gear: Drive");
    ui->Label_Gear_R->setToolTip("Gear: Reverse");

    connecting = false;
    GUIUpdateCount = 0;     //increments each update of the GUI

    // init speedometer animation
    QGraphicsScene *speedometerScene = new QGraphicsScene(this);
    ui->speedometer_GraphicsView->setScene(speedometerScene);
    QPixmap pix1(":/new/prefix2/photos/Car/tachonadel.png");
    QPixmap pix2(":/new/prefix2/photos/Car/tachometer.png");
    speedometer_GraphicsItem = speedometerScene->addPixmap(pix2);
    speedometer_Needle_GraphicsItem = speedometerScene->addPixmap(pix1);
    speedometer_Needle_GraphicsItem->moveBy(100,150);

    //init Tire animation
    QGraphicsScene *tireLeftScene = new QGraphicsScene(this);
    ui->tireLeft->setScene(tireLeftScene);
    QGraphicsScene *tireRightScene = new QGraphicsScene(this);
    ui->tireRight->setScene(tireRightScene);
    QPixmap pix3(":/new/prefix2/photos/Car/Tire_65.png");
    QPixmap pix4(":/new/prefix2/photos/Car/Tire_65.png");
    tireLeftScene->setSceneRect(0,0,141,121);
    tireRightScene->setSceneRect(0,0,141,121);
    tireLeft_GraphicsItem = tireLeftScene->addPixmap(pix3);
    tireRight_GraphicsItem = tireRightScene->addPixmap(pix4);
    tireLeft_GraphicsItem->setPos(50,60);
    tireRight_GraphicsItem->setPos(50,21);

    //init light pictures
    ui->fullbeam_Left->setVisible(false);
    ui->fullbeam_Right->setVisible(false);
    warning_light = false;
    indicator_right = false;
    indicator_left = false;
    arrive_Animation_started = false;
    blink_delaycounter = 0;
    //ui->Raw_Edit->setColumnWidth(1,400);

    timer = new QTimer(this);
    timer->start(100);
    connect(timer, SIGNAL(timeout()),this,SLOT(UpdateGui()));

    //init connection animation
    QMovie *movie = new QMovie(":new/prefix2/photos/Car/Accesspoint_Animation_klein.gif");
    ui->AccesspointLabel->setMovie(movie);
    movie->start();

    RemoteStatusGUI = 0;
    RemoteStatusEXT = 0;
    RemoteHeadLightBool = false;
    RemoteFullBeamBool = false;
    RemoteDirIndLeftBool = false;
    RemoteDirIndRightBool = false;
    RemoteWarnLightBool = false;
    RemoteEmStopBool = false;
    RemoteGearChange = 0;
    RemoteHeadLightFlashBool = false;
    RemoteTerminated = false;

    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, Qt::white);
    ui->general_widget->setAutoFillBackground(true);
    ui->general_widget->setPalette(Pal);
    connected_label = new QLabel(this);
    this->rearOkLabel = new QLabel(this);
    this->frontOkLabel = new QLabel(this);
    rearOkLabel->setText("Not connected to K60 rear");
    frontOkLabel->setText("Not connected to K60 front");
    connected_label->setText("Not Connected");
    QStatusBar *status;
    status = statusBar();
    status->addPermanentWidget(connected_label, 200);
    status->addPermanentWidget(rearOkLabel, 200);
    status->addPermanentWidget(frontOkLabel, 200);

    ui->Debug_ManualInput->setVisible(false);
    ui->Debug_GuidedInput->setVisible(false);
    ui->progressBar_Log->setVisible(false);
    ui->DebugStartCustomEdit->setEnabled(false);
    ui->DebugChecksumCustomEdit->setEnabled(false);
    RemoteGear = ui->gearList->currentRow();
    RemoteSetting_1 = ui->modeComboBox->currentIndex();
    RemoteSetting_2 = ui->recupComboBox->currentIndex();
    RemoteSetting_3 = ui->boostComboBox->currentIndex();
    startConfiguration();


    //Debug Help Window init
    ui->Debug_Help_Label->setText("");
    connect(ui->Debug_Help_Label,SIGNAL(mouse_enter()),this,SLOT(on_helpWindowMouseEntered()));
    connect(ui->Debug_Help_Label,SIGNAL(mouse_leave()),this,SLOT(on_helpWindowMouseLeaved()));
    connect(ui->Debug_Help_Label,SIGNAL(mouse_release()),this,SLOT(on_helpWindowMouseClicked()));

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
    for(int i = 0; i<16 ; i++)
        LatestDataInterpreted.battery_cell[i] = -1;

    ui->maxSpeedLabel->setText(QString::number(ui->maxSpeedSlider->value() / 10.0));
    remote_thread->MaxSpeed = ui->maxSpeedSlider->value() / 10.0;

    movieIsPaused = false;
    peri_Thread = 0;
}

MainWindow::~MainWindow()
{
    try{
        if(peri_Thread!=0)
        {
            peri_Thread->quit();
        }
        if(interprete_thread!=0)
        {
            interprete_thread->quit();
        }
        if(remote_thread!=0)
        {
            remote_thread->quit();
        }
        if(TCP_Thread!=0)
        {        
            TCP_Thread->FinishThread();
        }
        delete timer;
        //delete NetLock;
        //delete PeriodicSendObjectVectorLock;
        //delete RecQueueLock;
        //delete GUIDataLock;
        //delete HistoryLock;
        //delete RemoteLock;
        //delete ui;
    qApp->exit(0);
    }catch(std::exception e){}
    return;
}

void MainWindow::startConfiguration()
{
    ui->tab_widget->setCurrentIndex(0);
    //init Json
    QFile ConfigFileById(":/new/prefix2/ConfigbyID.json");
    if (!ConfigFileById.open(QIODevice::ReadOnly)) {
          qWarning("Couldn't open JSON file!!!");
          return;
      }
    else
        qWarning("Could open JSON file successfully!!!");
    QByteArray saveData = ConfigFileById.readAll();
    QJsonDocument loadDoc = QJsonDocument::fromJson(saveData);
    JsonList = loadDoc.object();
    ConfigFileById.close();
    //init GUI
    ui->Controller_Groupbox->setVisible(false);
    connection_Animation = new QMovie(":new/prefix2/photos/Car/Accesspoint_Animation_klein.gif");
    QMessageBox::StandardButton msganswer;		/*! the answer the user gives to the message box*/
    greenLabel.load(":/new/prefix2/photos/Green_Light.png");
    redLabel.load(":/new/prefix2/photos/Red_Light.png");
    Battery_full.load(":/new/prefix2/photos/Battery_state_full.png");
    Battery_half.load(":/new/prefix2/photos/Battery_state_middle.png");
    Battery_low.load(":/new/prefix2/photos/Battery_state_low.png");
    Battery_none.load(":/new/prefix2/photos/Battery_state_none.png");
    Headlight_on.load(":/new/prefix2/photos/4_converted.png");
    Headlight_off.load(":/new/prefix2/photos/3_converted.png");
    Fullbeam_on.load(":/new/prefix2/photos/1_converted.png");
    Fullbeam_off.load(":/new/prefix2/photos/2_converted.png");
    RightIndicator_on.load(":/new/prefix2/photos/14_converted.png");
    RightIndicator_off.load(":/new/prefix2/photos/11_converted.png");
    LeftIndicator_on.load(":/new/prefix2/photos/134_converted.png");
    LeftIndicator_off.load(":/new/prefix2/photos/133_converted.png");
    Warning_on.load(":/new/prefix2/photos/10_converted.png");
    Warning_off.load(":/new/prefix2/photos/9_converted.png");

    ui->BatteryCell_1->setPixmap(QPixmap::fromImage(Battery_none));
    ui->BatteryCell_2->setPixmap(QPixmap::fromImage(Battery_none));
    ui->BatteryCell_3->setPixmap(QPixmap::fromImage(Battery_none));
    ui->BatteryCell_4->setPixmap(QPixmap::fromImage(Battery_none));
    ui->BatteryCell_5->setPixmap(QPixmap::fromImage(Battery_none));
    ui->BatteryCell_6->setPixmap(QPixmap::fromImage(Battery_none));
    ui->BatteryCell_7->setPixmap(QPixmap::fromImage(Battery_none));
    ui->BatteryCell_8->setPixmap(QPixmap::fromImage(Battery_none));
    ui->BatteryCell_9->setPixmap(QPixmap::fromImage(Battery_none));
    ui->BatteryCell_10->setPixmap(QPixmap::fromImage(Battery_none));
    ui->BatteryCell_11->setPixmap(QPixmap::fromImage(Battery_none));
    ui->BatteryCell_12->setPixmap(QPixmap::fromImage(Battery_none));
    ui->BatteryCell_13->setPixmap(QPixmap::fromImage(Battery_none));
    ui->BatteryCell_14->setPixmap(QPixmap::fromImage(Battery_none));
    ui->BatteryCell_15->setPixmap(QPixmap::fromImage(Battery_none));
    ui->BatteryCell_16->setPixmap(QPixmap::fromImage(Battery_none));

    // Initializing the config
    config_handler::init_config();

    // load the configuration file
    int errorcode = config_handler::load_config(&config_handler::currentconfig, CONFIG_FILE_DEFAULT);
    if ( errorcode == 0)
    {
        ui->Config_TextEdit->append(QDateTime::currentDateTime().toString("hh:mm:ss") + " : " +
                                    "Configuration loaded.");
    }
    if ( errorcode == 1 )
    {
        msganswer = QMessageBox::question(this, "Error", "Could not open config file. It is possible that no config file exists. Do you want to create a new with the default configuration? WARNING! This could overwrite your config file.", QMessageBox::Yes | QMessageBox::No);
        if ( msganswer == QMessageBox::Yes )
        {
            int errorcode2;
            // because load_config failed (with code 1) the default config is
            // stored in currentconfig
            errorcode2 = config_handler::save_config(&config_handler::currentconfig, CONFIG_FILE_STD_LOC);
            if ( errorcode2 == 1 )
            {
                QMessageBox::warning(this, "Error", "Saving the default configuration has failed. It is possible that you don't have acess to the location of the config file");
                ui->Log_TextEdit->insertPlainText("Saving the default configuration has failed.\n");
            }
            if ( errorcode2 == 0 )
            {
                ui->Log_TextEdit->insertPlainText("Configuration file has been restored to default.\n");
            }
        }
    }
    if ( errorcode == 2 )
    {
        QMessageBox::warning(this, "Warning", "There are errors in the config file. Please see the log for details");
    }
    if ( errorcode == 3)
    {
        QMessageBox::warning(this, "Warning", "Could not find %appdata%.");
    }
    if ( (errorcode > 3) || (errorcode < 0) )
    {
        QMessageBox::warning(this, "Warning", "Unknown error while reading the config file.");
    }

    // Set default values for UI items
    ui->Host_Edit->setText(config_handler::currentconfig.networkADR);
    ui->Port_Edit->setText(config_handler::currentconfig.networkPORT);
    ui->CAL_ContinueButton->hide();
    ui->gearList->setCurrentRow(0);

    NetLock = new QMutex;	//used by this file, utility, PeriodicSendThread and TcpThread for only allowing one acess to SendData at a time.
    PeriodicSendObjectVectorLock = new QMutex;	// used by this file and PeriodicSendThread
    RecQueueLock = new QMutex; // used by TCPThread and InterpreteThread
    GUIDataLock = new QMutex; // see explenation at top of this file / used for displaying interpreted/raw data (on gui)
    HistoryLock = new QMutex; // used by this file and interpreteThread
    RemoteLock = new QMutex;

    // start the threads
    //TODO InterpreteThread* iT = new InterpreteThread(false); // interpretes new data
    interprete_thread = new InterpreteThread;
    //connect   "handle results"
    connect(interprete_thread, SIGNAL(finished()),interprete_thread,SLOT(deleteLater()));
    interprete_thread->init_LatestDataInterpreted(&LatestDataInterpreted);
    //interprete_thread->init_JsonList(&JsonList);
    interprete_thread->init_receivedMsgVector(&receivedMsgVector);
    interprete_thread->init_LatestRawData(&LatestRawData);
    interprete_thread->init_AvailableData(&AvailableData);
    interprete_thread->start();

    /*setup Remote Thread */
    remote_thread = new RemoteThread;
    remote_thread->init_RemoteStatusGUI(&RemoteStatusGUI);
    remote_thread->init_RemoteStatusEXT(&RemoteStatusEXT);
    remote_thread->init_fullBeamIsOn(&RemoteFullBeamBool);
    remote_thread->init_headLightIsOn(&RemoteHeadLightBool);
    remote_thread->init_indicatorLeftIsOn(&RemoteDirIndLeftBool);
    remote_thread->init_indicatorRightIsOn(&RemoteDirIndRightBool);
    remote_thread->init_warningLightIsOn(&RemoteWarnLightBool);
    remote_thread->init_RemoteEmStopBool(&RemoteEmStopBool);
    remote_thread->init_RemoteGearChange(&RemoteGearChange);
    remote_thread->init_RemoteHeadLightFlashBool(&RemoteHeadLightFlashBool);
    remote_thread->init_Terminated(&RemoteTerminated);
    remote_thread->init_RemoteGear(&RemoteGear);
    remote_thread->init_RemoteSetting_1(&RemoteSetting_1);
    remote_thread->init_RemoteSetting_2(&RemoteSetting_2);
    remote_thread->init_RemoteSetting_3(&RemoteSetting_3);
    remote_thread->start();
    // beginn to update gui
}

/**
 * @brief MainWindow::UpdateGui( it updates the graphical user interface  )
 *
 */
void MainWindow::UpdateGui()
{
    GUIUpdateCount++;
    GUIDataLock->lock();// the interprete thread and the remotethread use the same variables as this thread such as LatestDataInterpreted, RemoteStatusGUI...
                        // because of this, any access by the wrong thread must be locked!!!
    //*******************
    //UPDATE GENERAL VIEW
{
    if(movieIsPaused && ui->Connect_Label->text()=="Connected") //connection animation
    {
        ui->AccesspointLabel->setMovie(connection_Animation);
        ui->AccesspointLabel->setText("");
        ui->AccesspointLabel->movie()->start();
        movieIsPaused=false;
    }
   if(ui->Connect_Label->text()!="Connected")
    {
         ui->AccesspointLabel->setMovie(connection_Animation);
         ui->AccesspointLabel->movie()->start();
         ui->AccesspointLabel->movie()->stop();     //start,stop to show the movie's start frame
         movieIsPaused=true;
    }
    if(ui->Connect_Label->text()=="Disconnected" && !ui->Autoconnect_Checkbox->isChecked())
      connecting=false;

    if(ui->Autoconnect_Checkbox->isChecked()&& ui->Connect_Label->text()=="Disconnected")
    {
        ui->Connect_Button->click();
    }
    if(ui->Autoconnect_Checkbox->isChecked() && ui->Connect_Label->text()!="Connected" && !connecting)
    {
        QNetworkConfiguration cfg;
        QNetworkConfigurationManager ncm;
        QList <QNetworkConfiguration> nc = ncm.allConfigurations();

        for(QList <QNetworkConfiguration>::iterator x = nc.begin(); x!=nc.end(); ++x)
        {
            if (x->bearerType() == QNetworkConfiguration::BearerWLAN)
            {
                if (x->name() == "eCarus2")
                 {
                    try{
                         cfg = *x;
                         QNetworkSession *session = new QNetworkSession(cfg, this);
                         session->open();
                         connecting=true;
                    }
                    catch(std::exception e){}
                }
            }
        }
    }
    if(ui->Connect_Label->text()=="Connection\npending..." && this->interprete_thread->rearok && this->interprete_thread->frontok)
    {
        ui->Connect_Label->setText("Connected");
    }
    /*
    if(ui->Connect_Label->text()=="Connected" && !(this->interprete_thread->rearok && this->interprete_thread->frontok))
    {
       ui->Connect_Label->setText("Connection\npending...");
    }*/
    ui->Engine_Label->setVisible(LatestDataInterpreted.EngineSwitch);
    ui->Label_Master->setVisible(LatestDataInterpreted.MasterSwitch);
   // ui->LCD_Speed->display(QString::number(LatestDataInterpreted.speed,'f',1));
    //needleRotate(QString::number(LatestDataInterpreted.speed,'f',1).toInt());
    if (LatestDataInterpreted.gear == -1)
    {
        ui->Label_Gear_1->setVisible(false);
        ui->Label_Gear_N->setVisible(false);
        ui->Label_Gear_R->setVisible(true);
    }
    else if (LatestDataInterpreted.gear == 0)
    {
        ui->Label_Gear_1->setVisible(false);
        ui->Label_Gear_N->setVisible(true);
        ui->Label_Gear_R->setVisible(false);
    }
    else if (LatestDataInterpreted.gear == 1)
    {
        ui->Label_Gear_1->setVisible(true);
        ui->Label_Gear_N->setVisible(false);
        ui->Label_Gear_R->setVisible(false);
    }
    else
    {
        ui->Label_Gear_1->setVisible(false);
        ui->Label_Gear_N->setVisible(false);
        ui->Label_Gear_R->setVisible(false);
    }
    ui->Progressbar_Accelerator_3->setValue((int)LatestDataInterpreted.acceleratorpercentage);
    ui->Progressbar_Brake_3->setValue((int)LatestDataInterpreted.brakepercentage);
    ui->Horn_Label->setVisible(LatestDataInterpreted.Horn);
    ui->indicatorlight_Left->setVisible(LatestDataInterpreted.DirectionLeft);
    ui->indicatorlight_Right->setVisible(LatestDataInterpreted.DirectionRight);
    ui->indicatorlight_Left_Back->setVisible(LatestDataInterpreted.DirectionLeft);
    ui->indicatorlight_Right_Back->setVisible(LatestDataInterpreted.DirectionRight);

    if(LatestDataInterpreted.ForwardLight && LatestDataInterpreted.FullBeamLight)
     {
        ui->headlight_Left->setVisible(true);
        ui->headlight_Right->setVisible(true);
        ui->fullbeam_Left->setVisible(false);
        ui->fullbeam_Right->setVisible(false);
    }
    else if(LatestDataInterpreted.ForwardLight)
    {
        ui->fullbeam_Left->setVisible(true);
        ui->fullbeam_Right->setVisible(true);
        ui->headlight_Left->setVisible(false);
        ui->headlight_Right->setVisible(false);
    }
    else if(!LatestDataInterpreted.ForwardLight && !LatestDataInterpreted.FullBeamLight)
    {
        ui->fullbeam_Left->setVisible(false);
        ui->fullbeam_Right->setVisible(false);
        ui->headlight_Left->setVisible(false);
        ui->headlight_Right->setVisible(false);

    }
    ui->backlight_Left->setVisible(LatestDataInterpreted.BackwardLight);
    ui->backlight_Right->setVisible(LatestDataInterpreted.BackwardLight);
    ui->brakelight_Left->setVisible(LatestDataInterpreted.BrakeLight);
    ui->brakelight_Right->setVisible(LatestDataInterpreted.BrakeLight);

        //LatestDataInterpreted.
    ui->temperatureMotor1_Label->setText("Temp.: "+QString::number(LatestDataInterpreted.ConverterRightMotorTemp,'f',1)+"°C");
    ui->temperatureMotor2_Label->setText("Temp.: "+QString::number(LatestDataInterpreted.ConverterLeftMotorTemp,'f',1)+"°C");

    //ui->LCD_MoterLeft->display(QString::number(LatestDataInterpreted.ConverterLeftMotorTemp,'f',1));
    //ui->LCD_MotorRight->display(QString::number(LatestDataInterpreted.ConverterRightMotorTemp,'f',1));
    if(ui->interpretedDataComboBox->currentText()=="Controller L")
        ui->interpretedDataLabel->setText(""+QString::number(LatestDataInterpreted.ConverterLeftControllerTemp,'f',1)+"°C");
    if(ui->interpretedDataComboBox->currentText()=="Controller R")
       ui->interpretedDataLabel->setText(""+QString::number(LatestDataInterpreted.ConverterRightControllerTemp,'f',1)+"°C");
    //ui->LCD_ControllerLeft->display(QString::number(LatestDataInterpreted.ConverterLeftControllerTemp,'f',1));
    //ui->LCD_ControllerRight->display(QString::number(LatestDataInterpreted.ConverterRightControllerTemp,'f',1));
}
    //*******************

    //****************
    //UPDATE LIVE DATA
{

    std::vector<QString> liveDataVector;
    uint j = 0;
    for (QTreeWidgetItem* i=ui->treeWidget->topLevelItem(0); i != 0; i=ui->treeWidget->itemBelow(i))
    {
        liveDataVector.push_back(i->text(1)); //copy the data of the last update to spot data changes and highlight them with red color
        if(j>=liveDataVector.size()-1)
            liveDataDelayVector.push_back(0); //init with zero
        if(i->textColor(1)!=QColor(255,0,0))
            liveDataDelayVector[j]=GUIUpdateCount; //The red color should remain 10 GUI updates after a change
        j++;
    }

    QTreeWidgetItem *edit;
    edit = ui->treeWidget->topLevelItem(0);
    edit->setText(1,QString::number(LatestDataInterpreted.EngineSwitch,16));

    edit = ui->treeWidget->itemBelow(edit);
    edit->setText(1,QString::number(LatestDataInterpreted.MasterSwitch,16));

    edit = ui->treeWidget->itemBelow(edit);
    edit->setText(1,QString::number(LatestDataInterpreted.RunlevelK60rear));

    edit = ui->treeWidget->itemBelow(edit);
    edit->setText(1,QString::number(LatestDataInterpreted.RunlevelK60front));

    edit = ui->treeWidget->itemBelow(edit);
    edit->setText(1,QString::number(LatestDataInterpreted.rawacceleratorangle,16));

    edit = ui->treeWidget->itemBelow(edit);
    edit->setText(1,QString::number(LatestDataInterpreted.rawacceleratorangleGray,16));

    edit = ui->treeWidget->itemBelow(edit);
    if (LatestDataInterpreted.gear == -9){
        edit->setText(1,"undefined"); }
    else{
        edit->setText(1,QString::number(LatestDataInterpreted.gear)); }

    edit = ui->treeWidget->itemBelow(edit);
    edit->setText(1,QString::number(utility::CutOffDataFromInt(LatestDataInterpreted.TorqueLeft,15),16)
                  + " / " + QString::number(LatestDataInterpreted.TorqueLeft));
    edit = ui->treeWidget->itemBelow(edit);
    edit->setText(1,QString::number(utility::CutOffDataFromInt(LatestDataInterpreted.BrakingTorqueLeft,15),16)
                  + " / " +QString::number(LatestDataInterpreted.BrakingTorqueLeft));
    edit = ui->treeWidget->itemBelow(edit);
    edit->setText(1,QString::number(utility::CutOffDataFromInt(LatestDataInterpreted.TorqueRight,15),16)
                  + " / " +QString::number(LatestDataInterpreted.TorqueRight));
    edit = ui->treeWidget->itemBelow(edit);
    edit->setText(1,QString::number(utility::CutOffDataFromInt(LatestDataInterpreted.BrakingTorqueRight,15),16)
                  + " / " +QString::number(LatestDataInterpreted.BrakingTorqueRight));
    edit = ui->treeWidget->itemBelow(edit);
    edit->setText(1,QString::number(LatestDataInterpreted.erroridK60rear,16)
                  + " / " +QString::number(LatestDataInterpreted.erroridK60rear));
    edit = ui->treeWidget->itemBelow(edit);
    edit->setText(1,QString::number(LatestDataInterpreted.erroridK60front,16)
                  + " / " +QString::number(LatestDataInterpreted.erroridK60front));
    edit = ui->treeWidget->itemBelow(edit);                 //Battery current life converter
    edit->setText(1,QString::number(LatestDataInterpreted.ConverterLeftBatteryCurrent,'g', 6) + " A");

    edit = ui->treeWidget->itemBelow(edit);
    edit->setText(1,QString::number(LatestDataInterpreted.ConverterLeftCapacitorVoltage,'g', 6) + " V");

    edit = ui->treeWidget->itemBelow(edit);
    edit->setText(1,QString::number(LatestDataInterpreted.ConverterLeftControllerTemp,'g', 6) + " °C");

    edit = ui->treeWidget->itemBelow(edit);
    edit->setText(1,QString::number(LatestDataInterpreted.ConverterLeftCurrentRequest,'g', 6) + " A");

    edit = ui->treeWidget->itemBelow(edit);
    edit->setText(1,QString::number(LatestDataInterpreted.ConverterLeftCurrentRMS,'g', 6) + " A");

    edit = ui->treeWidget->itemBelow(edit);
    edit->setText(1,QString::number(LatestDataInterpreted.ConverterLeftMotorRPM));

    edit = ui->treeWidget->itemBelow(edit);
    edit->setText(1,QString::number(LatestDataInterpreted.ConverterLeftMotorTemp,'g', 6) + " °C");

    edit = ui->treeWidget->itemBelow(edit);
    edit->setText(1,QString::number(LatestDataInterpreted.ConverterLeftVCLThrottle,'g', 6) + " %");

    edit = ui->treeWidget->itemBelow(edit);
    edit->setText(1,QString::number(LatestDataInterpreted.ConverterRightBatteryCurrent,'g', 6) + " A");

    edit = ui->treeWidget->itemBelow(edit);
    edit->setText(1,QString::number(LatestDataInterpreted.ConverterRightCapacitorVoltage,'g', 6) + " V");

    edit = ui->treeWidget->itemBelow(edit);
    edit->setText(1,QString::number(LatestDataInterpreted.ConverterRightControllerTemp,'g', 6) + " °C");

    edit = ui->treeWidget->itemBelow(edit);
    edit->setText(1,QString::number(LatestDataInterpreted.ConverterRightCurrentRequest,'g', 6) + " A");

    edit = ui->treeWidget->itemBelow(edit);
    edit->setText(1,QString::number(LatestDataInterpreted.ConverterRightCurrentRMS,'g', 6) + " A");

    edit = ui->treeWidget->itemBelow(edit);
    edit->setText(1,QString::number(LatestDataInterpreted.ConverterRightMotorRPM));

    edit = ui->treeWidget->itemBelow(edit);
    edit->setText(1,QString::number(LatestDataInterpreted.ConverterRightMotorTemp,'g', 6) + " °C");

    edit = ui->treeWidget->itemBelow(edit);
    edit->setText(1,QString::number(LatestDataInterpreted.ConverterRightVCLThrottle,'g', 6) + " %");

    edit = ui->treeWidget->itemBelow(edit);
    if (LatestDataInterpreted.DirectionLeft){
        edit->setText(1,"ON");  }
    else{
        edit->setText(1,"OFF");  }

    edit = ui->treeWidget->itemBelow(edit);
    if (LatestDataInterpreted.DirectionRight){
        edit->setText(1,"ON");  }
    else{
        edit->setText(1,"OFF");  }

    edit = ui->treeWidget->itemBelow(edit);
    if (LatestDataInterpreted.Horn){
        edit->setText(1,"Beeeeeeeep");  }
    else{
        edit->setText(1,"OFF");  }

    edit = ui->treeWidget->itemBelow(edit);
    if (LatestDataInterpreted.BackwardLight){
        edit->setText(1,"ON");  }
    else{
        edit->setText(1,"OFF");  }

    edit = ui->treeWidget->itemBelow(edit);
    if (LatestDataInterpreted.BrakeLight){
        edit->setText(1,"ON");  }
    else{
        edit->setText(1,"OFF");  }

    edit = ui->treeWidget->itemBelow(edit);
    if (LatestDataInterpreted.ForwardLight){
        edit->setText(1,"ON");  }
    else{
        edit->setText(1,"OFF");  }

    edit = ui->treeWidget->itemBelow(edit);
    edit->setText(1,QString::number(LatestDataInterpreted.speed,'g', 4) + " km/h");

    edit = ui->treeWidget->itemBelow(edit);
    edit->setText(1,QString::number(LatestDataInterpreted.acceleratorpercentage,'g', 6) + " %");

    edit = ui->treeWidget->itemBelow(edit);
    edit->setText(1,QString::number(LatestDataInterpreted.brakepercentage,'g', 6) + " %");

    edit = ui->treeWidget->itemBelow(edit);
    if (LatestDataInterpreted.EthernetOkFront){
        edit->setText(1,"True");  }
    else{
        edit->setText(1,"False");  }

    edit = ui->treeWidget->itemBelow(edit);
    if (LatestDataInterpreted.EthernetOkRear){
        edit->setText(1,"True");  }
    else{
        edit->setText(1,"False");  }
    edit = ui->treeWidget->itemBelow(edit);
    edit->setText(1,QString::number(LatestDataInterpreted.DCDCcurrent,'g', 6) + " A");

    edit = ui->treeWidget->itemBelow(edit);
    edit->setText(1,QString::number(LatestDataInterpreted.HVV,'g', 6) + " V");


    int k=0;
    for (QTreeWidgetItem* i=ui->treeWidget->topLevelItem(0); i != 0; i=ui->treeWidget->itemBelow(i))
    {
        if(liveDataVector[k]!=i->text(1))  // a data change has occurred
        {
            i->setTextColor(1,QColor(255,0,0));         //set the data's color red
            liveDataDelayVector[k]=GUIUpdateCount;
        }
        else if(GUIUpdateCount-10>=liveDataDelayVector[k])
           i->setTextColor(1,QColor(0,0,0)); //After 10 GUI updates without a change: black color
        k++;
    }
}
    //****************

    //***************
    //UPDATE RAW DATA
{
    std::vector<QString> liveRawVector;
    uint j = 0;
     while(abs(ui->Raw_Edit->rowCount())*2>abs(liveRawDelayVector.size()))
         liveRawDelayVector.push_back(0); //init with zero

    for (int i=0; i<ui->Raw_Edit->rowCount(); i++)
    {
        QTableWidgetItem* item0 = ui->Raw_Edit->item(i,0);
        QTableWidgetItem* item1 = ui->Raw_Edit->item(i,1);

        liveRawVector.push_back(item0->text()); //copy the data of the last update to spot data changes and highlight them with red color
        liveRawVector.push_back(item1->text());
        if(item0->textColor()!=QColor(255,0,0))
            liveRawDelayVector[j]=GUIUpdateCount; //The red color should remain 10 GUI updates after a change
        if(item1->textColor()!=QColor(255,0,0))
            liveRawDelayVector[j+1]=GUIUpdateCount;
        j+=2;
    }

    ui->labelAvailableData->setText("Available Data: " + QString::number(AvailableData) );
    if ( !receivedMsgVector.empty() )
    {

        for (std::vector<Ethernet_Msg>::iterator j=receivedMsgVector.begin(); j != receivedMsgVector.end(); ++j)
        {
            QString raw_message;
            QByteArray bytearray;
            bytearray.resize(8);
            for(int i=0; i<8 ; i++)
            {
                bytearray[i] = j->Daten[i];
            }
            QString hexdata = bytearray.toHex();
            hexdata = hexdata.toUpper();

            //format the Data
            for(int k = 0; k < 8; k++)
            {
                raw_message += " " + QString(hexdata.at(k*2)) + QString(hexdata.at(k*2+1));
            }
            if (j->cycle == -1)
            {
                raw_message += " (invalid ID)";
            }
            else
            {
                raw_message += " (Cycle: " + QString::number((int)j->cycle) + ")";
            }
    //RAW-DATA-TABLE
            raw_messages.insert("0x"+QString::number(j->ID,16),raw_message);
            ui->Raw_Edit->setRowCount(raw_messages.size());

            if(tableWidgetItem_vector.size() < raw_messages.size()*2)                  //reserve memory for tablewidget
            {

                while(tableWidgetItem_vector.size()!= raw_messages.size()*2)
                {
                    QTableWidgetItem *tableWidgetItem = new QTableWidgetItem(tableWidgetItem_vector.size());
                    ui->Raw_Edit->setItem(tableWidgetItem_vector.size()/2,
                                          tableWidgetItem_vector.size()%2,
                                          tableWidgetItem);
                    tableWidgetItem_vector.push_back(tableWidgetItem);
                }
            }

            int table_index=0;
            for(QMap<QString,QString>::const_iterator i = raw_messages.constBegin(); i != raw_messages.constEnd() ; i++)
            {
                tableWidgetItem_vector.at(table_index*2)->setText(i.key());
                tableWidgetItem_vector.at(table_index*2+1)->setText(i.value());
                table_index++;
            }


    //JSON TABLE UPDATE<-TO DO: REPLACE THE CURRENT DATA INTERPRETATION METHOD BY THE JSON INTERPRETATION METHOD
            this->GetJSONInterpretation("0x"+QString::number(j->ID,16),j->Daten);  //saves DataMap with key and Value in Json_Map
            ui->JsonTableWidget->setRowCount(Json_Map.size());

            while(JsontableWidgetItem_vector.size() < Json_Map.size()*3)
            {
                QTableWidgetItem *JsontableWidgetItem = new QTableWidgetItem(JsontableWidgetItem_vector.size());
                ui->JsonTableWidget->setItem(JsontableWidgetItem_vector.size()/3,
                                      JsontableWidgetItem_vector.size()%3,
                                      JsontableWidgetItem);
                JsontableWidgetItem_vector.push_back(JsontableWidgetItem);
            }

            table_index=0;
            for(QMap<QString,QString>::const_iterator mapItemIterator = Json_Map.constBegin(); mapItemIterator != Json_Map.constEnd() ; mapItemIterator++)
            {               
                JsontableWidgetItem_vector.at(table_index*3)->setText(QString::number(j->ID,16));
                JsontableWidgetItem_vector.at(table_index*3+1)->setText(mapItemIterator.key());
                JsontableWidgetItem_vector.at(table_index*3+2)->setText(mapItemIterator.value());
                table_index++;
            }
            ui->JsonTableWidget->resizeColumnToContents(0);
        }
    }

    while(ui->Raw_Edit->rowCount()*2>abs(liveRawDelayVector.size()))
        liveRawDelayVector.push_back(0); //init with zero
    while(ui->Raw_Edit->rowCount()*2>abs(liveRawVector.size()))
        liveRawVector.push_back(0); //init with zero

    int k=0;
    for (int i=0; i<ui->Raw_Edit->rowCount(); i++)
    {
        QTableWidgetItem* item0 = ui->Raw_Edit->item(i,0);
        QTableWidgetItem* item1 = ui->Raw_Edit->item(i,1);
        if(liveRawVector[k]!=item0->text())  // a data change has occurred
        {
            item0->setTextColor(QColor(255,0,0));         //set the data's color red
            liveRawDelayVector[k]=GUIUpdateCount;
        }
        else if(GUIUpdateCount-10>=liveRawDelayVector[k])
            item0->setTextColor(QColor(0,0,0)); //After 10 GUI updates without a change: black color
        if(liveRawVector[k+1]!=item1->text())  // a data change has occurred
        {
            item1->setTextColor(QColor(255,0,0));         //set the data's color red
            liveRawDelayVector[k+1]=GUIUpdateCount;
        }
        else if(GUIUpdateCount-10>=liveRawDelayVector[k+1])
           item1->setTextColor(QColor(0,0,0)); //After 10 GUI updates without a change: black color

        k+=2;
    }

    GUIDataLock->unlock();
}
    //***************

    //**********************
    //UPDATE CALIBRATION VIEW
{

    if ( ui->Connect_Label->text() != "Connected" )
    {
        CALstep = 0;
        ui->CAL_Label->setText("You have to be connected to start the calibration");
        ui->CAL_ContinueButton->setVisible(false);
        ui->CAL_StartButton->setEnabled(false);
    }

    if ( (ui->Connect_Label->text() == "Connected") && (CALstep == 0) )
    {
        // reset if there is a connection
        ui->CAL_Label->setText(" ");
        ui->CAL_StartButton->setEnabled(true);
        ui->CAL_StartButton->setText("Start");
    }
}
    //***********************

    //******************
    //UPDATE REMOTE VIEW
{
    if ( RemoteStatusEXT == 1)
    {
        ui->Remote_StatusLabel->setText("Active. You can use the generatl tab to see relevant car data.");
        ui->startRemoteButton->setEnabled(true);
        ui->startRemoteButton->setText("Stop Remote Control Mode");
    }

    if ( (RemoteStatusEXT == 0) && (ui->Connect_Label->text() == "Connected") )
    {
        ui->Remote_StatusLabel->setText("Ready. Press Start to Start");
        ui->startRemoteButton->setEnabled(true);
        ui->startRemoteButton->setText("Start Remote Control ");
    }

    if ( ui->Connect_Label->text() == "Connected")
    {
        if ( RemoteStatusEXT == -1 )
        {
            ui->Remote_StatusLabel->setText("Remote Control not available. Could not Load libary. See manual/readme for more information.");
            ui->startRemoteButton->setEnabled(false);
            RemoteStatusGUI = 0;
            ui->startRemoteButton->setText("Start Remote Control");
        }
        if ( RemoteStatusEXT == -2 )
        {
            ui->Remote_StatusLabel->setText("Please connect a Microsoft XBox 360 Controller for Windows.");
            ui->startRemoteButton->setEnabled(false);
            RemoteStatusGUI = 0;
            ui->startRemoteButton->setText("Start Remote Control");
        }
        if ( RemoteStatusEXT == -3 )
        {
            ui->Remote_StatusLabel->setText("Unknown error.");
            ui->startRemoteButton->setEnabled(false);
            RemoteStatusGUI = 0;
            ui->startRemoteButton->setText("Start Remote Control");
        }
    }
    if ( ui->Connect_Label->text() != "Connected" )
    {
        ui->Remote_StatusLabel->setText("Please connect as IP 192.168.1.5 on \"eCARus2\" network to enable remote control");
        ui->startRemoteButton->setEnabled(false);
        RemoteStatusGUI = -1; // no connection
        ui->startRemoteButton->setText("Start Remote Control");
    }
}
    //*******************

    //*******************
    /*BATTERY CELL UPDATE
     * receive messages of ID 250 - 252 and get Data from it.
     * By getting the Voltage of each cell, the Gui shall display it by Value and Picture
     * to do implement it in a for-loop,
     * by setting a pointer on all BatteryCell-Labels
     * and stor them in a Vector
     */
{
    if(LatestDataInterpreted.battery_cell[0] > 3500)
        ui->BatteryCell_1->setPixmap(QPixmap::fromImage(Battery_full));
    else if(LatestDataInterpreted.battery_cell[0]==-1)
        ui->BatteryCell_1->setPixmap(QPixmap::fromImage(Battery_none));
    else if(LatestDataInterpreted.battery_cell[0]< 3000)
        ui->BatteryCell_1->setPixmap(QPixmap::fromImage(Battery_low));
    else
        ui->BatteryCell_1->setPixmap(QPixmap::fromImage(Battery_half));

    if(LatestDataInterpreted.battery_cell[1] > 3500)
        ui->BatteryCell_2->setPixmap(QPixmap::fromImage(Battery_full));
    else if(LatestDataInterpreted.battery_cell[1]==-1)
        ui->BatteryCell_2->setPixmap(QPixmap::fromImage(Battery_none));
    else if(LatestDataInterpreted.battery_cell[1]< 300)
        ui->BatteryCell_2->setPixmap(QPixmap::fromImage(Battery_low));
    else
        ui->BatteryCell_2->setPixmap(QPixmap::fromImage(Battery_half));

    if(LatestDataInterpreted.battery_cell[2] > 3500)
        ui->BatteryCell_3->setPixmap(QPixmap::fromImage(Battery_full));
    else if(LatestDataInterpreted.battery_cell[2]==-1)
        ui->BatteryCell_3->setPixmap(QPixmap::fromImage(Battery_none));
    else if(LatestDataInterpreted.battery_cell[2]< 3000)
        ui->BatteryCell_3->setPixmap(QPixmap::fromImage(Battery_low));
    else
        ui->BatteryCell_3->setPixmap(QPixmap::fromImage(Battery_half));

    if(LatestDataInterpreted.battery_cell[3] > 3500)
        ui->BatteryCell_4->setPixmap(QPixmap::fromImage(Battery_full));
    else if(LatestDataInterpreted.battery_cell[3]==-1)
        ui->BatteryCell_4->setPixmap(QPixmap::fromImage(Battery_none));
    else if(LatestDataInterpreted.battery_cell[3]< 3000)
        ui->BatteryCell_4->setPixmap(QPixmap::fromImage(Battery_low));
    else
        ui->BatteryCell_4->setPixmap(QPixmap::fromImage(Battery_half));

    if(LatestDataInterpreted.battery_cell[4] > 3500)
        ui->BatteryCell_5->setPixmap(QPixmap::fromImage(Battery_full));
    else if(LatestDataInterpreted.battery_cell[4]==-1)
        ui->BatteryCell_5->setPixmap(QPixmap::fromImage(Battery_none));
    else if(LatestDataInterpreted.battery_cell[4]< 3000)
        ui->BatteryCell_5->setPixmap(QPixmap::fromImage(Battery_low));
    else
        ui->BatteryCell_5->setPixmap(QPixmap::fromImage(Battery_half));

    if(LatestDataInterpreted.battery_cell[5] > 3500)
        ui->BatteryCell_6->setPixmap(QPixmap::fromImage(Battery_full));
    else if(LatestDataInterpreted.battery_cell[5]==-1)
        ui->BatteryCell_6->setPixmap(QPixmap::fromImage(Battery_none));
    else if(LatestDataInterpreted.battery_cell[5]< 3000)
        ui->BatteryCell_6->setPixmap(QPixmap::fromImage(Battery_low));
    else
        ui->BatteryCell_6->setPixmap(QPixmap::fromImage(Battery_half));

    if(LatestDataInterpreted.battery_cell[6] > 3500)
        ui->BatteryCell_7->setPixmap(QPixmap::fromImage(Battery_full));
    else if(LatestDataInterpreted.battery_cell[6]==-1)
        ui->BatteryCell_7->setPixmap(QPixmap::fromImage(Battery_none));
    else if(LatestDataInterpreted.battery_cell[6]< 3000)
        ui->BatteryCell_7->setPixmap(QPixmap::fromImage(Battery_low));
    else
        ui->BatteryCell_7->setPixmap(QPixmap::fromImage(Battery_half));

    if(LatestDataInterpreted.battery_cell[7] > 3500)
        ui->BatteryCell_8->setPixmap(QPixmap::fromImage(Battery_full));
    else if(LatestDataInterpreted.battery_cell[7]==-1)
        ui->BatteryCell_8->setPixmap(QPixmap::fromImage(Battery_none));
    else if(LatestDataInterpreted.battery_cell[7]< 3000)
        ui->BatteryCell_8->setPixmap(QPixmap::fromImage(Battery_low));
    else
        ui->BatteryCell_8->setPixmap(QPixmap::fromImage(Battery_half));

    if(LatestDataInterpreted.battery_cell[8] > 3500)
        ui->BatteryCell_9->setPixmap(QPixmap::fromImage(Battery_full));
    else if(LatestDataInterpreted.battery_cell[8]==-1)
        ui->BatteryCell_9->setPixmap(QPixmap::fromImage(Battery_none));
    else if(LatestDataInterpreted.battery_cell[8]< 3000)
        ui->BatteryCell_9->setPixmap(QPixmap::fromImage(Battery_low));
    else
        ui->BatteryCell_9->setPixmap(QPixmap::fromImage(Battery_half));

    if(LatestDataInterpreted.battery_cell[9] > 3500)
        ui->BatteryCell_10->setPixmap(QPixmap::fromImage(Battery_full));
    else if(LatestDataInterpreted.battery_cell[9]==-1)
        ui->BatteryCell_10->setPixmap(QPixmap::fromImage(Battery_none));
    else if(LatestDataInterpreted.battery_cell[9]< 3000)
        ui->BatteryCell_10->setPixmap(QPixmap::fromImage(Battery_low));
    else
        ui->BatteryCell_10->setPixmap(QPixmap::fromImage(Battery_half));

    if(LatestDataInterpreted.battery_cell[10] > 3500)
        ui->BatteryCell_11->setPixmap(QPixmap::fromImage(Battery_full));
    else if(LatestDataInterpreted.battery_cell[10]==-1)
        ui->BatteryCell_11->setPixmap(QPixmap::fromImage(Battery_none));
    else if(LatestDataInterpreted.battery_cell[10]< 3000)
        ui->BatteryCell_11->setPixmap(QPixmap::fromImage(Battery_low));
    else
        ui->BatteryCell_11->setPixmap(QPixmap::fromImage(Battery_half));

    if(LatestDataInterpreted.battery_cell[11] > 3500)
        ui->BatteryCell_12->setPixmap(QPixmap::fromImage(Battery_full));
    else if(LatestDataInterpreted.battery_cell[11]==-1)
        ui->BatteryCell_12->setPixmap(QPixmap::fromImage(Battery_none));
    else if(LatestDataInterpreted.battery_cell[11]< 3000)
        ui->BatteryCell_12->setPixmap(QPixmap::fromImage(Battery_low));
    else
        ui->BatteryCell_12->setPixmap(QPixmap::fromImage(Battery_half));

    if(LatestDataInterpreted.battery_cell[12] > 3500)
        ui->BatteryCell_13->setPixmap(QPixmap::fromImage(Battery_full));
    else if(LatestDataInterpreted.battery_cell[12]==-1)
        ui->BatteryCell_13->setPixmap(QPixmap::fromImage(Battery_none));
    else if(LatestDataInterpreted.battery_cell[12]< 3000)
        ui->BatteryCell_13->setPixmap(QPixmap::fromImage(Battery_low));
    else
        ui->BatteryCell_13->setPixmap(QPixmap::fromImage(Battery_half));

    if(LatestDataInterpreted.battery_cell[13] > 3500)
        ui->BatteryCell_14->setPixmap(QPixmap::fromImage(Battery_full));
    else if(LatestDataInterpreted.battery_cell[13]==-1)
        ui->BatteryCell_14->setPixmap(QPixmap::fromImage(Battery_none));
    else if(LatestDataInterpreted.battery_cell[13]< 3000)
        ui->BatteryCell_14->setPixmap(QPixmap::fromImage(Battery_low));
    else
        ui->BatteryCell_14->setPixmap(QPixmap::fromImage(Battery_half));

    if(LatestDataInterpreted.battery_cell[14] > 3500)
        ui->BatteryCell_15->setPixmap(QPixmap::fromImage(Battery_full));
    else if(LatestDataInterpreted.battery_cell[14]==-1)
        ui->BatteryCell_15->setPixmap(QPixmap::fromImage(Battery_none));
    else if(LatestDataInterpreted.battery_cell[14]< 3000)
        ui->BatteryCell_15->setPixmap(QPixmap::fromImage(Battery_low));
    else
        ui->BatteryCell_15->setPixmap(QPixmap::fromImage(Battery_half));

    if(LatestDataInterpreted.battery_cell[15] > 3500)
        ui->BatteryCell_16->setPixmap(QPixmap::fromImage(Battery_full));
    else if(LatestDataInterpreted.battery_cell[15]==-1)
        ui->BatteryCell_16->setPixmap(QPixmap::fromImage(Battery_none));
    else if(LatestDataInterpreted.battery_cell[15]< 3000)
        ui->BatteryCell_16->setPixmap(QPixmap::fromImage(Battery_low));
    else
        ui->BatteryCell_16->setPixmap(QPixmap::fromImage(Battery_half));
}
    //*******************

    if(this->interprete_thread->rearok == true)
    {
        rearOkLabel->setText("Connected to K60 rear");
    }
    else
    {
        rearOkLabel->setText("Not connected to K60 rear");
    }

    if(this->interprete_thread->frontok == true)
    {
        frontOkLabel->setText("Connected to K60 front");
    }
    else
    {
        frontOkLabel->setText("Not connected to K60 front");
    }

    //needleRotate((QTime::currentTime().hour()-8)*10+(QTime::currentTime().minute()-30)/6);
    needleRotate(LatestDataInterpreted.speed);// speedometer_update
}

void MainWindow::on_Connect_Button_clicked()
{
    if(ui->Connect_Label->text() == "Disconnected")
    {
        ui->Connect_Button->setStyleSheet("background:transparent;background-image:url(':/new/prefix2/photos/Buttons/Green_Button_on');background-repeat:no-repeat;");
        ui->Connect_Label->setText("Connection\npending...");

         connected_label->setText("Listening ..." );
         ui->Config_TextEdit->append(QDateTime::currentDateTime().toString("hh:mm:ss.zzz") + ": " +
                                    "Connected to "+ ui->Host_Edit->text());
        ui->Host_Edit->setReadOnly(true);
        ui->Port_Edit->setReadOnly(true);
        //disable settings on menu
        TCP_Thread = new UDPThread;
        connect(TCP_Thread, SIGNAL(finished()), TCP_Thread, SLOT(deleteLater()));
        TCP_Thread->SetupThread(ui->Host_Edit->text(), ui->Port_Edit->text());
        TCP_Thread->start();
    }
    else if(ui->Connect_Label->text() != "Disconnected")
    {
        ui->Connect_Button->setStyleSheet("background:transparent;background-image:url(':/new/prefix2/photos/Buttons/Green_Button_off');background-repeat:no-repeat;");
        ui->Connect_Label->setText("Disconnected");
        connected_label->setText("Not Connected");
        ui->Config_TextEdit->append(QDateTime::currentDateTime().toString("hh:mm:ss.zzz") + ": " +
                                    "Connection closed by user.");
        ui->Host_Edit->
                setReadOnly(false);
        ui->Port_Edit->setReadOnly(false);
        TCP_Thread->FinishThread();
        TCP_Thread->quit();
    }
}

/**
 * @brief MainWindow::on_actionConnect_Disconnect_triggered : called to verify if there is connection with the car or not.
 */
void MainWindow::on_actionConnect_Disconnect_triggered()
{
    if(ui->Connect_Label->text() == "Disconnected")
    {
        ui->Connect_Label->setText("Connection\npending...");
        connected_label->setText("Listening ..." );
        ui->Connect_Button->setStyleSheet("background:transparent;background-image:url(':/new/prefix2/photos/Buttons/Green_Button_on')");
        ui->Config_TextEdit->append(QDateTime::currentDateTime().toString("hh:mm:ss.zzz") + ": " +
                                    "Connected to "+ ui->Host_Edit->text());
        ui->Host_Edit->setReadOnly(true);
        ui->Port_Edit->setReadOnly(true);
        //disable settings on menu
        TCP_Thread = new UDPThread;
        connect(TCP_Thread, SIGNAL(finished()), TCP_Thread, SLOT(deleteLater()));
        TCP_Thread->SetupThread(ui->Host_Edit->text(), ui->Port_Edit->text());
        TCP_Thread->start();
    }
    else if(ui->Connect_Label->text() != "Disconnected")
    {
        ui->Connect_Button->setStyleSheet("background:transparent;background-image:url(':/new/prefix2/photos/Buttons/Green_Button_off')");
        ui->Connect_Label->setText("Disconnected");
        connected_label->setText("Not Connected");
        ui->Config_TextEdit->append(QDateTime::currentDateTime().toString("hh:mm:ss.zzz") + ": " +
                                    "Connection closed by user.");
        ui->Host_Edit->
                setReadOnly(false);
        ui->Port_Edit->setReadOnly(false);
        TCP_Thread->FinishThread();
        TCP_Thread->quit();
    }

    if(this->interprete_thread->rearok == true)
    {
        rearOkLabel->setText("Connected to K60 rear");
    }
    else
    {
        rearOkLabel->setText("Not connected to K60 rear");
    }

    if(this->interprete_thread->frontok == true)
    {
        frontOkLabel->setText("Connected to K60 front");
    }
    else
    {
        frontOkLabel->setText("Not connected to K60 front");
    }
    if(this->interprete_thread->rearok && this->interprete_thread->frontok)
    {
        ui->Connect_Label->setText("Connected");
    }

}

//************************************************
//      FUNCTIONS ACTIVATED BY THE XBOX CONTROLLER
//************************************************

void MainWindow::ChangeGearList(int i)
{
    int next_gear = i + ui->gearList->currentRow();
    if((next_gear >= 0) && (next_gear <= 2))
    {
        ui->gearList->setCurrentRow(next_gear);
        RemoteGear = next_gear;
    }
    ui->Label_gear->setText_Clicked("Gang: "+ui->gearList->currentItem()->text());
    ui->Label_gear->clicked();
}
void MainWindow::ChangeGearList_Released(int i)
{
    ui->Label_gear->released();
}

void MainWindow::on_startRemoteButton_clicked()
{
    if ( (RemoteStatusEXT == 0)  && (ui->Connect_Label->text() == "Connected")) // standby and connected
    {
        RemoteStatusGUI = 1; // start
        connect(remote_thread, SIGNAL(finished()),remote_thread,SLOT(deleteLater()));
        connect(remote_thread, SIGNAL(gear_change(int)),this,SLOT(ChangeGearList(int)));
        connect(remote_thread, SIGNAL(gear_change_Released(int)),this,SLOT(ChangeGearList_Released(int)));
        connect(remote_thread, SIGNAL(xButton(void)),this,SLOT(on_headLightButton_clicked(void)));
        connect(remote_thread, SIGNAL(xButton_Released(void)),this,SLOT(on_headLightButton_Released(void)));
        connect(remote_thread, SIGNAL(yButton(void)),this,SLOT(on_fullBeamButton_clicked(void)));
        connect(remote_thread, SIGNAL(yButton_Released()),this,SLOT(on_fullBeamButton_Released()));
        connect(remote_thread, SIGNAL(rightTrigger()), this, SLOT(on_indicatorRightButton_clicked()));
        connect(remote_thread, SIGNAL(rightTrigger_Released()), this, SLOT(on_indicatorRightButton_Released()));
        connect(remote_thread, SIGNAL(leftTrigger()), this, SLOT(on_indicatorLeftButton_clicked()));
        connect(remote_thread, SIGNAL(leftTrigger_Released()), this, SLOT(on_indicatorLeftButton_Released()));
        connect(remote_thread, SIGNAL(startButton()), this, SLOT(on_warningButton_clicked()));
        connect(remote_thread, SIGNAL(startButton_Released()), this, SLOT(on_warningButton_Released()));
        connect(remote_thread, SIGNAL(accelerate(int)),this,SLOT(on_accelerateButton_clicked(int)));
        connect(remote_thread, SIGNAL(brake(int)),this,SLOT(on_brakeButton_clicked(int)));
        connect(remote_thread, SIGNAL(horn()),this,SLOT(on_hornButton_clicked()));
        connect(remote_thread, SIGNAL(horn_Released(void)),this,SLOT(on_hornButton_Released(void)));
        connect(remote_thread, SIGNAL(steering(int)),this,SLOT(on_steering(int)));
    }
    if ( RemoteStatusEXT == 1 )  // RC Enabled
    {
        RemoteStatusGUI = 0; // shutdown
        disconnect(remote_thread, SIGNAL(finished()),remote_thread,SLOT(deleteLater()));
        disconnect(remote_thread, SIGNAL(gear_change(int)),this,SLOT(ChangeGearList(int)));
        disconnect(remote_thread, SIGNAL(gear_change_Released(int)),this,SLOT(ChangeGearList_Released(int)));
        disconnect(remote_thread, SIGNAL(xButton()),this,SLOT(on_headLightButton_clicked()));
        disconnect(remote_thread, SIGNAL(xButton_Released()),this,SLOT(on_headLightButton_Released()));
        disconnect(remote_thread, SIGNAL(yButton()),this,SLOT(on_fullBeamButton_clicked()));
        disconnect(remote_thread, SIGNAL(yButton_Released()),this,SLOT(on_fullBeamButton_Released()));
        disconnect(remote_thread, SIGNAL(rightTrigger()), this, SLOT(on_indicatorRightButton_clicked()));
        disconnect(remote_thread, SIGNAL(rightTrigger_Released()), this, SLOT(on_indicatorRightButton_Released));
        disconnect(remote_thread, SIGNAL(leftTrigger()), this, SLOT(on_indicatorLeftButton_clicked()));
        disconnect(remote_thread, SIGNAL(leftTrigger_Released()), this, SLOT(on_indicatorLeftButton_Released()));
        disconnect(remote_thread, SIGNAL(startButton()), this, SLOT(on_warningButton_clicked()));
        disconnect(remote_thread, SIGNAL(startButton_Released()), this, SLOT(on_warningButton_Released()));
        disconnect(remote_thread, SIGNAL(accelerate(int)),this,SLOT(on_accelerateButton_clicked(int)));
        disconnect(remote_thread, SIGNAL(brake(int)),this,SLOT(on_brakeButton_clicked(int)));
        disconnect(remote_thread, SIGNAL(horn()),this,SLOT(on_hornButton_clicked()));
        disconnect(remote_thread, SIGNAL(horn_Released()),this,SLOT(on_hornButton_Released()));
        disconnect(remote_thread, SIGNAL(steering(int)),this,SLOT(on_steering(int)));
    }
}

void MainWindow::on_stopRemoteButton_clicked()
{
    RemoteEmStopBool = true;
}

void MainWindow::on_headLightButton_clicked()
{
    RemoteHeadLightBool = !RemoteHeadLightBool; // toogle
    // switch of Full Beam when Headlights are off
    RemoteFullBeamBool = RemoteHeadLightBool ? RemoteFullBeamBool : false;
    //set image labels
    if(RemoteHeadLightBool)
    {
        ui->fullBeamButton->setEnabled(true);
        ui->headLightLabel->setPixmap(QPixmap::fromImage(greenLabel));
        ui->General_Headlight->setPixmap(QPixmap::fromImage(Headlight_on));
        ui->Label_light->setText_Clicked("Head Light ON");
    }
    else
    {
        ui->fullBeamButton->setEnabled(false);
        ui->headLightLabel->setPixmap(QPixmap::fromImage(redLabel));
        ui->General_Headlight->setPixmap(QPixmap::fromImage(Headlight_off));
        ui->fullBeamLabel->setPixmap(QPixmap::fromImage(redLabel));
        ui->General_Fullbeam->setPixmap(QPixmap::fromImage(Fullbeam_off));
        ui->Label_light->setText_Clicked("Head Light OFF");
    }
    ui->Label_light->clicked();

    //ui->headlight_Left->setVisible(RemoteHeadLightBool);
    //ui->headlight_Right->setVisible(RemoteHeadLightBool);
   // ui->fullbeam_Left->setVisible(false);
   // ui->fullbeam_Right->setVisible(false);
}

void MainWindow::on_headLightButton_Released()
{
  ui->Label_light->released();
}

void MainWindow::on_fullBeamButton_clicked()
{

    RemoteFullBeamBool = !RemoteFullBeamBool;
    if(RemoteFullBeamBool)
    {
        ui->Label_light->setText_Clicked("Full Beam ON");
        ui->fullBeamLabel->setPixmap(QPixmap::fromImage(greenLabel));
        ui->General_Fullbeam->setPixmap(QPixmap::fromImage(Fullbeam_on));
    }
    else
    {
        ui->Label_light->setText_Clicked("Full Beam OFF");
        ui->fullBeamLabel->setPixmap(QPixmap::fromImage(redLabel));
        ui->General_Fullbeam->setPixmap(QPixmap::fromImage(Fullbeam_off));
    }
    ui->Label_light->clicked();

   // ui->fullbeam_Left->setVisible(RemoteFullBeamBool);
   // ui->fullbeam_Right->setVisible(RemoteFullBeamBool);
  //  ui->headlight_Left->setVisible(false);
  //  ui->headlight_Right->setVisible(false);
}
void MainWindow::on_fullBeamButton_Released()
{
    ui->Label_light->released();
}

void MainWindow::on_indicatorLeftButton_clicked()
{

    RemoteDirIndLeftBool = !RemoteDirIndLeftBool; // toogle
    // switch off all other indicators
    RemoteDirIndRightBool = false;
    RemoteWarnLightBool = false;
    if(RemoteDirIndLeftBool)
    {
        ui->indicatorLeftLabel->setPixmap(QPixmap::fromImage(greenLabel));
        ui->Label_indicatorLeft->setText_Clicked("Left Indicator\nON");
    }
    else
    {
        ui->indicatorLeftLabel->setPixmap(QPixmap::fromImage(redLabel));
        ui->Label_indicatorLeft->setText_Clicked("Left Indicator\nOFF");
    }
    ui->indicatorRightLabel->setPixmap(QPixmap::fromImage(redLabel));
    ui->warningLabel->setPixmap(QPixmap::fromImage(redLabel));
    ui->General_Warninglight->setPixmap(QPixmap::fromImage(Warning_off));
    ui->Label_indicatorLeft->clicked();
    indicator_left = RemoteDirIndLeftBool;
    indicator_right = false;
  //  if(!indicator_left)
 //       ui->indicatorlight_Left->setVisible(RemoteDirIndLeftBool);
    warning_light=false;
}
void MainWindow::on_indicatorLeftButton_Released()
{
    ui->Label_indicatorLeft->released();
}

void MainWindow::on_indicatorRightButton_clicked()
{
    RemoteDirIndRightBool =  !RemoteDirIndRightBool; // toogle
    // switch off all other indicators
    RemoteDirIndLeftBool = false;
    RemoteWarnLightBool = false;
    if(RemoteDirIndRightBool)
    {
        ui->indicatorRightLabel->setPixmap(QPixmap::fromImage(greenLabel));
        ui->Label_indicatorRight->setText_Clicked("Right Indicator\nON");
    }
    else
    {
        ui->indicatorRightLabel->setPixmap(QPixmap::fromImage(redLabel));
        ui->Label_indicatorRight->setText_Clicked("Right Indicator\nOFF");
    }
    ui->indicatorLeftLabel->setPixmap(QPixmap::fromImage(redLabel));
    ui->warningLabel->setPixmap(QPixmap::fromImage(redLabel));
    ui->General_Warninglight->setPixmap(QPixmap::fromImage(Warning_off));
    ui->Label_indicatorRight->clicked();
    indicator_right=RemoteDirIndRightBool;
    indicator_left=false;
    warning_light=false;
   // if(!indicator_right)
     //   ui->indicatorlight_Right->setVisible(RemoteDirIndRightBool);
}
void MainWindow::on_indicatorRightButton_Released()
{
    ui->Label_indicatorRight->released();
}

void MainWindow::on_warningButton_clicked()
{
    RemoteWarnLightBool =! RemoteWarnLightBool; // toogle
    warning_light=RemoteWarnLightBool;
    // switch off all other indicators
    RemoteDirIndLeftBool = false;
    RemoteDirIndRightBool = false;
    if(RemoteWarnLightBool)
    {
        ui->warningLabel->setPixmap(QPixmap::fromImage(greenLabel));
        ui->General_Warninglight->setPixmap(QPixmap::fromImage(Warning_on));
        ui->Label_warning->setText_Clicked("Warning:\nON");
    }
    else
    {
        ui->warningLabel->setPixmap(QPixmap::fromImage(redLabel));
        ui->General_Warninglight->setPixmap(QPixmap::fromImage(Warning_off));
        ui->Label_warning->setText_Clicked("Warning:\nOFF");
    }
    ui->indicatorLeftLabel->setPixmap(QPixmap::fromImage(redLabel));
    ui->indicatorRightLabel->setPixmap(QPixmap::fromImage(redLabel));
    ui->Label_warning->clicked();

    indicator_left=false;
    indicator_right=false;
}
void MainWindow::on_warningButton_Released()
{
    ui->Label_warning->released();
}

void MainWindow::on_accelerateButton_clicked(int accelerationvalue)
{
    ui->Label_throttle->setText_Clicked(QString("Gas\n%1").arg(accelerationvalue));
    if(accelerationvalue!=0)
        ui->Label_throttle->clicked();
    else
        ui->Label_throttle->released();
}

void MainWindow::on_brakeButton_clicked(int brakevalue)
{
    ui->Label_brake->setText_Clicked(QString("%1").arg(brakevalue));
    if(brakevalue!=0)
     {
        ui->Label_brake->clicked();
       // ui->brakelight_Left->setVisible(true);
        //ui->brakelight_Right->setVisible(true);
     }
    else
    {
        ui->Label_brake->released();
        if(!warning_light)
        {
          //  ui->brakelight_Left->setVisible(false);
           // ui->brakelight_Right->setVisible(false);
        }
    }

}

void MainWindow::on_hornButton_clicked()
{
    ui->Label_horn->clicked();
}
void MainWindow::on_hornButton_Released(void)
{
    ui->Label_horn->released();
}

void MainWindow::on_steering(int angle)
{
    ui->Label_steering->setText_Clicked(QString("steering\n%1").arg(angle));
    tiresRotate((angle-127)*0.32);
    if(angle!=128)
        ui->Label_steering->clicked();
    else
        ui->Label_steering->released();
}
//-----------------------------------------------------------------------

//***************
//      DEBUG TAB
//***************
int MainWindow::DebugDecodeInput (RawData* msg)
{
    RawData message; // the message

    if ( ui->DebugManualRadioButton->isChecked() )
    {
        // prepare the raw message

        // check if the length is ok
        if (ui->DebugManualDataEdit->text().length() > 14*2)
        {
            QMessageBox::information(NULL, "Error!", "Your message is to long.It has not been sent.");
            return 1;
        }

        // convert the message
        std::string text;
        text = ui->DebugManualDataEdit->text().toStdString();
        while (text.length() < 28)
        {
            text="0"+text;
        }

        if (utility::HexStringToCharArray(text, message.RawData, text.length(), 14))
        {
           QMessageBox::information(NULL, "Error!", "Your message contains non valid characters. It has not been sent.");
           return 1;
        }

    }
    if ( ui->DebugGuidedRadioButton->isChecked() )
    {
        // add zeros to the ID field if it is necessary
        while (ui->DebugCanIDEdit->text().length() < 6)
        {
            ui->DebugCanIDEdit->setText("0"+ui->DebugCanIDEdit->text());
        }
        // add zeros to the DATA field if it is necessary
        while (ui->DebugCanMessageEdit->text().length() < 2*8)
        {
            ui->DebugCanMessageEdit->setText("0"+ui->DebugCanMessageEdit->text());
        }
        // build a message

        //set start
        if (ui->ethernetAARadioButton->isChecked())
        {
            message.RawData[0] = 0xAA;
        }
        else
        {
            if ( ui->DebugStartCustomEdit->text().length() > 2 )
            {
                QMessageBox::information(NULL, "Error!", "Your start is too long. Your message has not been sent.");
                return 1;
            }
            std::string tmp_string ;
            tmp_string = ui->DebugStartCustomEdit->text().toStdString();
            if (utility::HexStringToCharArray(tmp_string, message.RawData, ui->DebugStartCustomEdit->text().length(), 1))
            {
                QMessageBox::information(NULL, "Error!", "Your start data is not valid. It has not been send.");
                return 1;
            }
        }

        // set config
        // check if the length is ok
        if ( ui->DebugConfigurationEdit->text().length() > 2 )
        {
            QMessageBox::information(NULL, "Error!", "Your config is too long. Your message has not been sent.");
            return 1;
        }
        std::string tmp_string;
        tmp_string = ui->DebugConfigurationEdit->text().toStdString();
        if (utility::HexStringToCharArray(tmp_string, message.RawData + 1, ui->DebugConfigurationEdit->text().length(), 1))
        {
            QMessageBox::information(NULL, "Error!", "Your config is not valid. Your message has not been sent.");
            return 1;
        }



        // read ID
        unsigned char cid[3];
        if ( ui->DebugCanIDEdit->text().length() > 2*3 )
        {
            QMessageBox::information(NULL, "Error!", "Your ID is too long. Your message has not been sent.");
            return 1;
        }

        tmp_string = ui->DebugCanIDEdit->text().toStdString();
        if (utility::HexStringToCharArray(tmp_string, cid, ui->DebugCanIDEdit->text().length(), 3))
        {
            QMessageBox::information(NULL, "Error!", "Your ID is not valid. Your message has not been sent.");
            return 1;
        }

        // write the id to the message	(LITTLE ENDIAN)
        message.RawData [4] = cid [0];
        message.RawData [3] = cid [1];
        message.RawData [2] = cid [2];

        // write the message part of the message
        if ( ui->DebugCanMessageEdit->text().length() > 2*8 )
        {
            QMessageBox::information(NULL, "Error!", "Your message is too long. It has not been sent.");
            return 1;
        }

        tmp_string = ui->DebugCanMessageEdit->text().toStdString();
        if (utility::HexStringToCharArray(tmp_string, message.RawData + 5, ui->DebugCanMessageEdit->text().length(), 8))
        {
            QMessageBox::information(NULL, "Error!", "Your Message is not valid. It has not been sent.");
            return 1;
        }

        // checksum
        if ( ui->checksumComputeRadioButton->isChecked() )
        {
            // compute the correct checksum
            int crc = 0;	// the checksum
            int i;
            for (i = 1; i <= 12; i++)
            {
                crc += message.RawData[i];
                // add the value of all data, id and config bytes
            }
            crc &= 0xFF; // drop everything but the last byte
            crc ^= 0xFF; // Invert (XOR with 0xFF)

            // write the checksum to the message
            message.RawData [13] = crc;
        }
        else
        {
            // use the user provided checksum
            if ( ui->DebugChecksumCustomEdit->text().length() > 2 )
            {
                QMessageBox::information(NULL, "Error!", "Your Checksum is too long. Your message has not been sent.");
                return 1;
            }
            std::string tmp_string;
            tmp_string = ui->DebugChecksumCustomEdit->text().toStdString();
            if (utility::HexStringToCharArray(tmp_string, message.RawData + 13, ui->DebugChecksumCustomEdit->text().length(), 1))
            {
                QMessageBox::information(NULL, "Error!", "Your Checksum is not valid. Your message has not been sent.");
                return 1;
            }
        }
    }
    // copy the message
    for (int j = 0; j < 14; j++)
    {
        msg->RawData[j] = message.RawData[j];
    }
    return 0; // there was no error so it was a sucess (-:
}

void MainWindow::on_ethernetAARadioButton_clicked()
{
    ui->DebugStartCustomEdit->setEnabled(false);
    ui->DebugStartCustomEdit->setText("AA");
}

void MainWindow::on_ethernetCustomRadioButton_clicked()
{
    ui->DebugStartCustomEdit->setEnabled(true);
}

void MainWindow::on_checksumComputeRadioButton_clicked()
{
    ui->DebugChecksumCustomEdit->setEnabled(false);
}

void MainWindow::on_checksumCustomRadioButton_clicked()
{
    ui->DebugChecksumCustomEdit->setEnabled(true);
}

void MainWindow::on_DebugManualRadioButton_clicked()
{

    ui->Debug_ManualInput->setVisible(true);
    ui->Debug_GuidedInput->setVisible(false);
    ui->DebugSendNowButton->setEnabled(true);
    ui->DebugAddButton->setEnabled(true);
}

void MainWindow::on_DebugGuidedRadioButton_clicked()
{
    ui->Debug_ManualInput->setVisible(false);
    ui->Debug_GuidedInput->setVisible(true);
    ui->DebugSendNowButton->setEnabled(true);
    ui->DebugAddButton->setEnabled(true);
}

void MainWindow::on_DebugNoneRadioButton_clicked()
{
    ui->Debug_ManualInput->setVisible(false);
    ui->Debug_GuidedInput->setVisible(false);
    ui->DebugSendNowButton->setEnabled(false);
    ui->DebugAddButton->setEnabled(false);
}

void MainWindow::on_DebugSendNowButton_clicked()
{
    if ( ui->Connect_Label->text()  != "Connected" )
    {
        // Return message box
        QMessageBox::information(NULL, "Warning!", "Please connect first. Your message has not been sent.");
        return;
    }

    struct RawData message; // array with the message (as number)
    // prepare the message to be send

    if (DebugDecodeInput(&message))
    {
        // if the return value is 1 there was a problem. the user has already been
        // notified about this. so we will simply exit the function
        return;
    }
    // push the message to the network thread
    NetLock->lock();
    SendData.push(message);
    NetLock->unlock();
}

void MainWindow::on_DebugAddButton_clicked()
{
    /*
        Add a new object to the vector with periodical send objects
    */
    static int newid=0;
    PeriodicSendObject tmp;
    tmp.active = true;
    tmp.lastsent = 0;
    tmp.ID = newid;
    RawData temprawdata;

    if ( DebugDecodeInput( &temprawdata ) )
    {
        /* if there was an error the function will notify the user and will
           return 1 (so this will be executed) */
        return;
    }

    // copy the data
    for (int i=0; i < 14; i++)
    {
        tmp.data[i] = temprawdata.RawData[i];
    }

    try
    {
        tmp.interval = ui->DebugCycleTimeEdit->text().toInt(0,10);
    }
    catch ( ... )
    {
        QMessageBox::information(NULL, "Warning!", "The Input for cycle time is not valid.");
        return;
    }
    // check how the data was created
    if (ui->DebugManualRadioButton->isChecked())
    {
        tmp.creation = 1; // manual
    }
    else
    {
        tmp.creation = 2; // guided
    }

    // add the data to the vector
    PeriodicSendObjectVectorLock->lock();
    PeriodicSendObjectVector.push_back(tmp);
    PeriodicSendObjectVectorLock->unlock();

    // get the data that will be send
    if (ui->DebugManualRadioButton->isChecked())
    {
        std::string data_string(29,'0'); // (2*14)+1; contains the rawdata in hex format
        utility::CharArrayToHexString(data_string, tmp.data, 14);
        data_string[28] = '\0';
        // add data to ui element
        QTableWidgetItem *item_id = new QTableWidgetItem(0);
        QTableWidgetItem *item_status = new QTableWidgetItem(1);
        item_id->setText(QString::number(newid));
        item_status->setText("Active: yes, Cycle: " + ui->DebugCycleTimeEdit->text() +
                             ", Data: " + QString::fromStdString(data_string));
        int row = ui->Debug_MsgList->rowCount();

        ui->Debug_MsgList->setRowCount(row +1);
        ui->Debug_MsgList->setItem(row,item_id->type(),item_id);
        ui->Debug_MsgList->setItem(row,item_status->type(),item_status);



    }
    else
    {
        wchar_t strID[7];
        wchar_t strDATA[17];
        wchar_t strCRC [3];
        wchar_t strSTART [3];
        wchar_t strCFG [3];
        utility::DecodeDataToString(tmp.data,strID,strCFG,strSTART,strCRC,strDATA);
        QTableWidgetItem *item_id = new QTableWidgetItem(0);
        QTableWidgetItem *item_status = new QTableWidgetItem(1);
        item_id->setText(QString::number(newid));
        item_status->setText("Active: yes, Cycle: " + ui->DebugCycleTimeEdit->text() + ", ID: "+
                             QString::fromWCharArray(strID) + ", Data: " + QString::fromWCharArray(strDATA) +
                             ", Config: " + QString::fromWCharArray(strCFG) + ", Start: " +
                             QString::fromWCharArray(strSTART) + ", CRC: " + QString::fromWCharArray(strCRC));
        int row = ui->Debug_MsgList->rowCount();
        ui->Debug_MsgList->setRowCount(row +1);
        ui->Debug_MsgList->setItem(row,item_id->type(),item_id);
        ui->Debug_MsgList->setItem(row,item_status->type(),item_status);
    }

    // select the last element (if this code is missing ther would be some cosmetic problems)
    //        debugperiodicstatus->Row=debugperiodicstatus->RowCount - 1;

    newid++; // use an other id for the next data
}

void MainWindow::on_DebugToggleActiveButton_clicked()
{
    if ( ui->Debug_MsgList->currentRow() == -1 )
    {
    QMessageBox::information(NULL, "Error!", "Nothing selected.");
        return;
    }
    /*
        Toogles the activity for the selected item (and update the gui)
    */
    int j; // for for loops

    int id; // id of the selected item
    QString id_str;

    try
    {
        // read the ID
        id_str = ui->Debug_MsgList->item(ui->Debug_MsgList->currentRow(),0)->text();
        id = id_str.toInt(0,10);
    }
    catch ( ... )
    {
        QMessageBox::information(NULL, "Warning!", "Nothing to update!");
        // i don't know any other possibilitie how an exception could happen here
        return;
    }

    PeriodicSendObject tmp_obj; // will store the data to update the gui

    // search the vector for the ID and update the item
    PeriodicSendObjectVectorLock->lock();
    for (std::vector<PeriodicSendObject>::iterator i=PeriodicSendObjectVector.begin(); i != PeriodicSendObjectVector.end(); ++i)
    {
        if ( i->ID == id )
        {
            // update the Active Property
            i->active = !(i->active);

            // copy the item
            tmp_obj.ID = i->ID;
            tmp_obj.interval = i->interval;

            for (j=0; j < 14; j++)
            {
                tmp_obj.data[j] = i->data[j];
            }
            tmp_obj.active = i->active;
            tmp_obj.creation = i->creation;
        }
    }
    PeriodicSendObjectVectorLock->unlock();

    // convert Status to a string
    QString active_string;
    if ( tmp_obj.active )
    {
        active_string = "Yes";
    }
    else
    {
        active_string = "No";
    }

    // update the gui
    if (tmp_obj.creation == 1)
    {

        // convert the data to a string
        std::string data_string(29,'0'); // stores the data as a string; 14*2+1
        utility::CharArrayToHexString (data_string, tmp_obj.data, 14);
        data_string[28] = '\0';
        QString tmp_string("Active: " + active_string + ", Cycle: " + QString::number((int)tmp_obj.interval,10) +
                           ", Data: " + QString::fromStdString(data_string));
        ui->Debug_MsgList->item(ui->Debug_MsgList->currentRow(),1)->setText(tmp_string);
    }
    else
    {
        wchar_t strID[7];
        wchar_t strDATA[17];
        wchar_t strCRC [3];
        wchar_t strSTART [3];
        wchar_t strCFG [3];
        utility::DecodeDataToString(tmp_obj.data,strID,strCFG,strSTART,strCRC,strDATA);
        QString tmp_string("Active: " + active_string + ", Cycle: " + QString::number( (int)tmp_obj.interval ,10 ) +
                           ", ID: "+ QString::fromWCharArray(strID) + ", Data: " +
                           QString::fromWCharArray(strDATA) + ", Config: " + QString::fromWCharArray(strCFG) +
                           ", Start: " + QString::fromWCharArray(strSTART) + ", CRC: " +
                           QString::fromWCharArray(strCRC));
        ui->Debug_MsgList->item(ui->Debug_MsgList->currentRow(),1)->setText(tmp_string);
    }
}

void MainWindow::on_DebugRemoveButton_clicked()
{
    /*
        Remove the object selected in the gui from the gui and from the vector (that will be read by the SendPeriodic Thread)
    */

    // stop if nothing is selected (i don't think this works )-:)
    if ( ui->Debug_MsgList->currentRow() == -1 )
    {
    QMessageBox::information(NULL, "Error!", "Nothing selected.");
        return;
    }

    int id; // the id
    QString id_str; // the id as string
    try
    {
        // read the ID and delete the gui element
        id_str = ui->Debug_MsgList->item(ui->Debug_MsgList->currentRow(),0)->text();
       // QTableWidgetItem *item_id = ui->Debug_MsgList->item(ui->Debug_MsgList->currentRow(),0);
      //  QTableWidgetItem *item_status = ui->Debug_MsgList->item(ui->Debug_MsgList->currentRow(),1);
        ui->Debug_MsgList->removeRow(ui->Debug_MsgList->currentRow());
       // delete item_id;
      //  delete item_status;
        //ui->Debug_MsgList->setRowCount(ui->Debug_MsgList->rowCount()-1);
    }
    catch ( ... )
    {
        QMessageBox::information(NULL, "Error!", "Nothing to delete.");
        // i don't know any other possibilitie how an exception could happen here
        return;
    }

    // convert the id to an int
    try
    {
        id = id_str.toInt(0,10);
    }
    catch ( ... )
    {
       QMessageBox::information(NULL, "Error!", "Internal error.");
        // i don't think this exception can really happen
        return;
    }

    // remove from the actual vector               b
    PeriodicSendObjectVectorLock->lock();

    for (std::vector<PeriodicSendObject>::iterator i=PeriodicSendObjectVector.begin(); i != PeriodicSendObjectVector.end(); ++i)
    {
        if ( i->ID == id )
        {
            // delete when an object with the same id is found
            PeriodicSendObjectVector.erase(i);
            PeriodicSendObjectVectorLock->unlock();

            return;
        }
    }
    PeriodicSendObjectVectorLock->unlock();
    QMessageBox::information(NULL, "Error!", "Not successful.");
    return;
}

void MainWindow::on_DebugEnableCheckBox_clicked()
{
    /*
        Activates and deactives the Periodic Thread.
    */

    bool status = ui->DebugEnableCheckBox->isChecked();
    if ( status )
    {
        if(PeriodicSendObjectVector.empty())
         {
             QMessageBox::information(NULL, "Error!", "No messages to send");
             ui->DebugEnableCheckBox->setChecked(false);
             return;
         }
        if ( ui->Connect_Label->text() != "Connected" )
        {
            QMessageBox::information(NULL, "Error!", "You are not connected to ECARUS!");
            ui->DebugEnableCheckBox->setChecked(false);
            return;
        }
        std::cout<<"STARTENABLE"<<std::endl;
        peri_Timer = new QTimer();
        peri_Thread = new QThread();                         //create a new thread and a worker
        peri_worker = new PeriodicSendWorker();

        peri_worker->moveToThread(peri_Thread);             // Move the Object, which sends the messages periodically to the Thread
        peri_Timer->moveToThread(peri_Thread);
        peri_Timer->setInterval(100);

        connect(peri_Thread,  SIGNAL(started()),peri_Timer,SLOT(start()));         //thread starts Timer
        connect(peri_Timer,  SIGNAL(timeout()),peri_worker,SLOT(process_start()));   //timer activates process
        connect(peri_worker, SIGNAL(finished(int)), peri_Timer,SLOT(start(int)));        //Worker changes peri_Timer's Interval
        disconnect(peri_worker, SIGNAL(finished(int)),peri_worker,SLOT(deleteLater()));
        disconnect(peri_Thread, SIGNAL(finished()),peri_Timer,SLOT(deleteLater()));         //dont delete created Objects, once the worker finished
        disconnect(peri_Thread, SIGNAL(finished()),peri_Thread,SLOT(deleteLater()));

        peri_Thread->start();

    }
    if ( !status )
    {
        if(peri_Thread){
            peri_Thread->quit();
            connect(peri_Thread, SIGNAL(finished()),peri_Timer,SLOT(deleteLater()));
            connect(peri_Thread, SIGNAL(finished()),peri_Thread,SLOT(deleteLater()));   // do delete created objects, once the worker finished
        }

    }
}

//**************
//      MAIN TAB
//**************
void MainWindow::on_Development_Button_clicked() //TEST_BUTTON
{
    unsigned char data[8];
    data[0] = 0x12;
    data[1] = 0x2F;
    data[2] = 0x33;

    this->GetJSONInterpretation("0x300",data);  //saves DataMap with key and Value in Json_Map
    ui->JsonTableWidget->setRowCount(Json_Map.size());

    while(JsontableWidgetItem_vector.size() < Json_Map.size()*3)
    {
        QTableWidgetItem *JsontableWidgetItem = new QTableWidgetItem(JsontableWidgetItem_vector.size());
        ui->JsonTableWidget->setItem(JsontableWidgetItem_vector.size()/3,
                              JsontableWidgetItem_vector.size()%3,
                              JsontableWidgetItem);
        JsontableWidgetItem_vector.push_back(JsontableWidgetItem);
    }
    int table_index=0;
    for(QMap<QString,QString>::const_iterator mapItemIterator = Json_Map.constBegin(); mapItemIterator != Json_Map.constEnd() ; mapItemIterator++)
    {
        JsontableWidgetItem_vector.at(table_index*3)->setText("0x300");
        JsontableWidgetItem_vector.at(table_index*3+1)->setText(mapItemIterator.key());
        JsontableWidgetItem_vector.at(table_index*3+2)->setText(mapItemIterator.value());
        table_index++;
    }
    if(Json_Map.value("Forward Light")==QString("true"))
          qWarning("Forward Light true");
    else
        std::cout<<Json_Map.value("Direction Right").toStdString()<<std::endl;

    ui->headlight_Left->setVisible(!ui->headlight_Left->isVisible());
    ui->headlight_Right->setVisible(!ui->headlight_Right->isVisible());
    ui->backlight_Left->setVisible(!ui->backlight_Left->isVisible());
    ui->backlight_Right->setVisible(!ui->backlight_Right->isVisible());
    ui->indicatorlight_Left->setVisible(!ui->indicatorlight_Left->isVisible());
    ui->indicatorlight_Right->setVisible(!ui->indicatorlight_Right->isVisible());
    ui->indicatorlight_Left_Back->setVisible(!ui->indicatorlight_Left->isVisible());
    ui->indicatorlight_Right_Back->setVisible(!ui->indicatorlight_Right->isVisible());
    ui->fullbeam_Left->setVisible(!ui->fullbeam_Left->isVisible());
    ui->fullbeam_Right->setVisible(!ui->fullbeam_Right->isVisible());
    ui->brakelight_Left->setVisible(!ui->brakelight_Left->isVisible());
    ui->brakelight_Right->setVisible(!ui->brakelight_Right->isVisible());
    if(ui->Development_Button->styleSheet()=="background:transparent;background-image:url(':/new/prefix2/photos/Buttons/Red_Button_on.png');background-repeat:no-repeat;")
    {
        ui->Development_Button->setStyleSheet("background:transparent;background-image:url(':/new/prefix2/photos/Buttons/Red_Button_off.png');background-repeat:no-repeat;");
    }

    else
    {
         ui->Development_Button->setStyleSheet("background:transparent;background-image:url(':/new/prefix2/photos/Buttons/Red_Button_on.png');background-repeat:no-repeat;");
    }
}

void MainWindow::on_RemoteStartButton_clicked() //Start/stop remote control
{
    //warning_light = !
    ;

    if(ui->RemoteStartButton->styleSheet()=="background:transparent;background-image:url(':/new/prefix2/photos/Buttons/Blue_Button_on.png');background-repeat:no-repeat;") //connected->disconnected
    {
        ui->label_8->setText("Start Remote Control");
        ui->RemoteStartButton->setStyleSheet("background:transparent;background-image:url(':/new/prefix2/photos/Buttons/Blue_Button_off.png');background-repeat:no-repeat;");
        RemoteEmStopBool = true;
        if ( RemoteStatusEXT == 1 )  // RC Enabled
        {
            RemoteStatusGUI = 0; // shutdown
            disconnect(remote_thread, SIGNAL(finished()),remote_thread,SLOT(deleteLater()));
            disconnect(remote_thread, SIGNAL(gear_change(int)),this,SLOT(ChangeGearList(int)));
            disconnect(remote_thread, SIGNAL(gear_change_Released(int)),this,SLOT(ChangeGearList_Released(int)));
            disconnect(remote_thread, SIGNAL(xButton()),this,SLOT(on_headLightButton_clicked()));
            disconnect(remote_thread, SIGNAL(xButton_Released()),this,SLOT(on_headLightButton_Released()));
            disconnect(remote_thread, SIGNAL(yButton()),this,SLOT(on_fullBeamButton_clicked()));
            disconnect(remote_thread, SIGNAL(yButton_Released()),this,SLOT(on_fullBeamButton_Released()));
            disconnect(remote_thread, SIGNAL(rightTrigger()), this, SLOT(on_indicatorRightButton_clicked()));
            disconnect(remote_thread, SIGNAL(rightTrigger_Released()), this, SLOT(on_indicatorRightButton_Released));
            disconnect(remote_thread, SIGNAL(leftTrigger()), this, SLOT(on_indicatorLeftButton_clicked()));
            disconnect(remote_thread, SIGNAL(leftTrigger_Released()), this, SLOT(on_indicatorLeftButton_Released()));
            disconnect(remote_thread, SIGNAL(startButton()), this, SLOT(on_warningButton_clicked()));
            disconnect(remote_thread, SIGNAL(startButton_Released()), this, SLOT(on_warningButton_Released()));
            disconnect(remote_thread, SIGNAL(accelerate(int)),this,SLOT(on_accelerateButton_clicked(int)));
            disconnect(remote_thread, SIGNAL(brake(int)),this,SLOT(on_brakeButton_clicked(int)));
            disconnect(remote_thread, SIGNAL(horn()),this,SLOT(on_hornButton_clicked()));
            disconnect(remote_thread, SIGNAL(horn_Released()),this,SLOT(on_hornButton_Released()));
            disconnect(remote_thread, SIGNAL(steering(int)),this,SLOT(on_steering(int)));
        }
    }

    else                        //disconnected->connected
    {
        if ( (RemoteStatusEXT == 0)  && (ui->Connect_Label->text() == "Connected")) // standby and connected
        {
            ui->label_8->setText("Stop Remote Control");
            RemoteStatusGUI = 1; // start
            connect(remote_thread, SIGNAL(finished()),remote_thread,SLOT(deleteLater()));
            connect(remote_thread, SIGNAL(gear_change(int)),this,SLOT(ChangeGearList(int)));
            connect(remote_thread, SIGNAL(gear_change_Released(int)),this,SLOT(ChangeGearList_Released(int)));
            connect(remote_thread, SIGNAL(xButton(void)),this,SLOT(on_headLightButton_clicked(void)));
            connect(remote_thread, SIGNAL(xButton_Released(void)),this,SLOT(on_headLightButton_Released(void)));
            connect(remote_thread, SIGNAL(yButton(void)),this,SLOT(on_fullBeamButton_clicked(void)));
            connect(remote_thread, SIGNAL(yButton_Released()),this,SLOT(on_fullBeamButton_Released()));
            connect(remote_thread, SIGNAL(rightTrigger()), this, SLOT(on_indicatorRightButton_clicked()));
            connect(remote_thread, SIGNAL(rightTrigger_Released()), this, SLOT(on_indicatorRightButton_Released()));
            connect(remote_thread, SIGNAL(leftTrigger()), this, SLOT(on_indicatorLeftButton_clicked()));
            connect(remote_thread, SIGNAL(leftTrigger_Released()), this, SLOT(on_indicatorLeftButton_Released()));
            connect(remote_thread, SIGNAL(startButton()), this, SLOT(on_warningButton_clicked()));
            connect(remote_thread, SIGNAL(startButton_Released()), this, SLOT(on_warningButton_Released()));
            connect(remote_thread, SIGNAL(accelerate(int)),this,SLOT(on_accelerateButton_clicked(int)));
            connect(remote_thread, SIGNAL(brake(int)),this,SLOT(on_brakeButton_clicked(int)));
            connect(remote_thread, SIGNAL(horn()),this,SLOT(on_hornButton_clicked()));
            connect(remote_thread, SIGNAL(horn_Released(void)),this,SLOT(on_hornButton_Released(void)));
            connect(remote_thread, SIGNAL(steering(int)),this,SLOT(on_steering(int)));
            ui->RemoteStartButton->setStyleSheet("background:transparent;background-image:url(':/new/prefix2/photos/Buttons/Blue_Button_on.png');background-repeat:no-repeat;");
        }
        if(ui->Connect_Label->text() != "Connected")
        {
            RemoteStatusGUI = -1; // no connection
            QMessageBox::warning(this, "Error", "Please connect as IP 169.168.1.5 on \"eCARus2\" network to enable remote control");
        }
        if ( RemoteStatusEXT == -1 )
        {
            QMessageBox::warning(this, "Error","Remote Control not available. Could not Load libary. See manual/readme for more information.");
            RemoteStatusGUI = 0;
        }
        if (RemoteStatusEXT == -2 )
        {
            QMessageBox::warning(this, "Error", "Please connect a Microsoft XBox 360 Controller for Windows.");
            RemoteStatusGUI = 0;
        }
        if (RemoteStatusEXT == -3 )
        {
            QMessageBox::warning(this, "Error", "Unknown Error");
            RemoteStatusGUI = 0;
        }
    }
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    tiresRotate(value);
}

void MainWindow::on_keyAssignment_Button_clicked()
{
    ui->keyAssignment_Button->setEnabled(false);
    ui->Controller_Groupbox->setVisible(false);
    if(ui->keyAssignment_Button->styleSheet()=="background:transparent;background-image:url(':/new/prefix2/photos/Buttons/Green_Button_on.png');background-repeat:no-repeat;")
    {
        ui->keyAssignment_Button->setStyleSheet("background:transparent;background-image:url(':/new/prefix2/photos/Buttons/Green_Button_off.png');background-repeat:no-repeat;");
        ui->keyAssignment_Label->setText("Key Assignment");
        arrive_Animation();
    }
    else
    {
        ui->keyAssignment_Button->setStyleSheet("background:transparent;background-image:url(':/new/prefix2/photos/Buttons/Green_Button_on.png');background-repeat:no-repeat;");
        ui->keyAssignment_Label->setText("   Show eCARus");
        depart_Animation();
    }
}

void MainWindow::on_maxSpeedSlider_actionTriggered(int action)
{
}

void MainWindow::on_maxSpeedSlider_sliderMoved(int position)
{
}
/**
 * @brief called when the value of the slider changes
 * @param value position of the slider
 *
 * langert text.( it controls the damping factor of the accelerator )
 */
void MainWindow::on_maxSpeedSlider_valueChanged(int value)
{
    ui->maxSpeedLabel->setText(QString::number(value / 10.0));
    remote_thread->MaxSpeed = value / 10.0;
}

void MainWindow::on_modeComboBox_currentIndexChanged(int index)
{
    RemoteSetting_1 = index;
}

void MainWindow::on_recupComboBox_currentIndexChanged(int index)
{
    RemoteSetting_2 = index;
}

void MainWindow::on_boostComboBox_currentIndexChanged(int index)
{
    RemoteSetting_3 = index;
}
//*********************
//      CALIBRATION TAB
//*********************
void MainWindow::on_CAL_StartButton_clicked()
{
    CALstep=1; // beginn
    if ( ui->Connect_Label->text() != "Connected" )
    {
        return; //do nothing if there is no connection
    }
    // send the message
    char tmp[8] = {0};
    tmp[0]=0x1; // start calibration
    utility::SendCanMessage(0x290,tmp);
    ui->CAL_Label->setText("Please do not touch the accelerator and make sure it is in its idle position");
    ui->CAL_ContinueButton->setVisible(true);
    ui->CAL_StartButton->setText("Restart");
    ui->CAL_ContinueButton->setText("Continue");
}

void MainWindow::on_CAL_ContinueButton_clicked()
{
    /*
        continue button for the calibration feature.
        Checks in what step of the calibration the software is and sends the correct
        message.
        The user must not be able to press this button / call this function if there
        is no connection.
    */
    if ( CALstep == 1 )
    {
        // send the message
        char tmp[8] = {0};
        tmp[0]=0x5; // calibration
        utility::SendCanMessage(0x290,tmp);
        ui->CAL_Label->setText("Please press the accelerator fully.");
        CALstep=2;

        return;
    }
    if ( CALstep == 2 )
    {
        // send the can message
        char tmp[8] = {0};
        tmp[0]=0x9; // calibration
        utility::SendCanMessage(0x290,tmp);
        ui->CAL_Label->setText("Calibration Sucessfull! Do you want to restart?");
        CALstep=0; // reset
        ui->CAL_ContinueButton->setText("Finish");
        CALstep = 3;

        return;
    }
    if ( CALstep == 3 )
    {
        char tmp[8] = {0};
        tmp[0]=0x2; // end calibration
        utility::SendCanMessage(0x290,tmp);
        ui->CAL_Label->setText(" ");
        ui->CAL_ContinueButton->setVisible(false);
        ui->CAL_StartButton->setText("Start");
        CALstep = 0;
        return;
    }
}
//*******
//LOG TAB
//*******
void MainWindow::on_CreateLogButton_clicked()
{
    /*
        Fills the Memo1 (raw log) on demand.
    */
    ui->CreateLogButton->setEnabled(false);
    ui->SaveLogButton->setEnabled(true);

    //	Form1->Memo1->Lines->BeginUpdate(); // stop refreshing
    ui->Log_TextEdit->clear();
    HistoryLock->lock();

    ui->progressBar_Log->setValue(0);
    ui->progressBar_Log->setMaximum(RawHistory.size());
    ui->labelAvailableData->setVisible(false);
    ui->progressBar_Log->setVisible(true);

    for (std::vector<RawData>::iterator i=RawHistory.begin(); i != RawHistory.end(); ++i)
    {
        QString buf;
        QDateTime time = i->timestamp ; buf = time.toString("dd.MM") + ", " + time.toString("hh:mm:ss.zzz") + ": ";

        // Raw Data
        for(int j = 0; j < 14; j++)
        {
            buf += QString::number(i->RawData[j], 16) + " ";
        }

        // Cycle
        buf += "Cycle: " + QString::number((int)i->cycle);

        // Write
        ui->Log_TextEdit->append(buf);

        // Updata Progressbar
        ui->progressBar_Log->setValue( ui->progressBar_Log->value() + 1 );

        // Keep gui alive
        //		Application->ProcessMessages();
    }
    HistoryLock->unlock();
    //	Form1->Memo1->Lines->EndUpdate(); // refresh

    ui->progressBar_Log->setVisible(false);
    ui->labelAvailableData->setVisible(true);
    ui->CreateLogButton->setEnabled(true);
    ui->SaveLogButton->setEnabled(true);
}
void MainWindow::on_Export_Button_clicked()
{
}
void MainWindow::on_SaveLogButton_clicked()
{
 }

//*******************
//ANIMATION FUNCTIONSs
//*******************
void MainWindow::needleRotate(int speed) /// rotates the speedometer needle around the center
{

    speedometer_Needle_GraphicsItem->setPos(90,100);

    speedometer_Needle_GraphicsItem->setRotation(47+speed*2.948);

    speedometer_Needle_GraphicsItem->setPos(90,100);

    speedometer_Needle_GraphicsItem->moveBy(12.8062484749*cos(0.01745329251*(speedometer_Needle_GraphicsItem->rotation()+225)),12.8062484749*sin(0.01745329251*(speedometer_Needle_GraphicsItem->rotation()+225)));
    //the rotation method rotates the image around the top left pixel, but the wanted rotation center of the speedometer_Needle is (x,y)=(10,8), so the image has to be moved
    //sqrt(10^2+8^2)=12.8062...; 1° = 0.01745...rad; 5/8 * 360 = 225 :=start angle
}

void MainWindow::tiresRotate(int angle)
{
    //setRotation()  rotates the image around the top left pixel, but the intended rotation center is different, so the image has to be moved
    //rotation center : (27,0) -left tire
    tireLeft_GraphicsItem->setPos(50,59);
    tireLeft_GraphicsItem->setRotation(angle);
    tireLeft_GraphicsItem->setPos(50,59);
    tireLeft_GraphicsItem->moveBy(27.5*cos(0.01745329251*(tireLeft_GraphicsItem->rotation()+180))+27.5,27.5*sin(0.01745329251*(tireLeft_GraphicsItem->rotation()+180)));
    //rotation center : (27,37) - right tire
    tireRight_GraphicsItem->setPos(50,21);
    tireRight_GraphicsItem->setRotation(angle);
    tireRight_GraphicsItem->setPos(50,21);
    tireRight_GraphicsItem->moveBy(45.8039299624*cos(0.01745329251*(tireRight_GraphicsItem->rotation()+(53.8806592+180)))+27.5,45.8039299624*sin(0.01745329251*(tireRight_GraphicsItem->rotation()+(53.8806592+180)))+38);
    //sqrt(27^2+37^2)=45.8039299624
    //sqrt(0+27^2)=27; 1° = 0.01745...rad;
}

void MainWindow::arrive_Animation() // eCARus arrive Animation
{
    ui->indicatorlight_Left->setVisible(false);
    ui->indicatorlight_Right->setVisible(false);
    ui->indicatorlight_Left_Back->setVisible(false);
    ui->indicatorlight_Right_Back->setVisible(false);
    ui->headlight_Left->setVisible(false);
    ui->headlight_Right->setVisible(false);
    ui->backlight_Left->setVisible(false);
    ui->backlight_Right->setVisible(false);
    ui->fullbeam_Left->setVisible(false);
    ui->fullbeam_Right->setVisible(false);
    ui->tireLeft->setVisible(false);
    ui->tireRight->setVisible(false);
    ui->interpretedDataComboBox->setVisible(false);
    ui->interpretedDataLabel->setVisible(false);
    ui->temperatureMotor1_Label->setVisible(false);
    ui->temperatureMotor2_Label->setVisible(false);
    ui->brakelight_Left->setVisible(false);
    ui->brakelight_Right->setVisible(false);

    ui->graphicsView_2->setStyleSheet("background:transparent;");
    QGraphicsScene *eCARusScene = new QGraphicsScene(this);
    ui->graphicsView_2->setScene(eCARusScene);
    QPixmap pix(":new/prefix2/photos/Car/eCARus_wo_signs.png");
    eCARusScene->setSceneRect(0,0,521,231);
    QGraphicsItem *eCARus_GraphicsItem = eCARusScene->addPixmap(pix);
    eCARus_GraphicsItem->moveBy(700,11);

    QTimeLine *animationTimer = new QTimeLine(1000);
    animationTimer->setFrameRange(0, 100);

    QGraphicsItemAnimation *animation = new QGraphicsItemAnimation;
    animation->setItem(eCARus_GraphicsItem);
    animation->setTimeLine(animationTimer);

    for (int i = 100; i >=0; i--)
        animation->setPosAt(1-i/100.0,QPointF(i*6-3,11));

    ui->graphicsView_2->show();
    animationTimer->start();
    connect(animationTimer,SIGNAL(finished()),this,SLOT(arrive_Animation_finished()));
    connect(animationTimer,SIGNAL(finished()),animation,SLOT(deleteLater()));
    connect(animationTimer,SIGNAL(finished()),eCARusScene,SLOT(deleteLater()));
    connect(animationTimer,SIGNAL(finished()),animationTimer,SLOT(deleteLater()));
}
void MainWindow::arrive_Animation_finished()
{
    ui->keyAssignment_Button->setEnabled(true);

    ui->indicatorlight_Left->setVisible(true);
    ui->indicatorlight_Right->setVisible(true);
    ui->indicatorlight_Left_Back->setVisible(true);
    ui->indicatorlight_Right_Back->setVisible(true);
    ui->headlight_Left->setVisible(true);
    ui->headlight_Right->setVisible(true);
    ui->backlight_Left->setVisible(true);
    ui->backlight_Right->setVisible(true);
    ui->fullbeam_Left->setVisible(true);
    ui->fullbeam_Right->setVisible(true);
    ui->tireLeft->setVisible(true);
    ui->tireRight->setVisible(true);
    ui->interpretedDataComboBox->setVisible(true);
    ui->interpretedDataLabel->setVisible(true);
    ui->temperatureMotor1_Label->setVisible(true);
    ui->temperatureMotor2_Label->setVisible(true);
    ui->brakelight_Left->setVisible(true);
    ui->brakelight_Right->setVisible(true);
    ui->graphicsView_2->setScene(0);
    ui->graphicsView_2->setStyleSheet("background-image: url(':/new/prefix2/photos/Car/ECARUS_Car_1_65.png');background-color:none;background-repeat:no-repeat;");
}

void MainWindow::depart_Animation()
{
    ui->indicatorlight_Left->setVisible(false);
    ui->indicatorlight_Right->setVisible(false);
    ui->indicatorlight_Left_Back->setVisible(false);
    ui->indicatorlight_Right_Back->setVisible(false);
    ui->headlight_Left->setVisible(false);
    ui->headlight_Right->setVisible(false);
    ui->backlight_Left->setVisible(false);
    ui->backlight_Right->setVisible(false);
    ui->fullbeam_Left->setVisible(false);
    ui->fullbeam_Right->setVisible(false);
    ui->tireLeft->setVisible(false);
    ui->tireRight->setVisible(false);
    ui->interpretedDataComboBox->setVisible(false);
    ui->interpretedDataLabel->setVisible(false);
    ui->temperatureMotor1_Label->setVisible(false);
    ui->temperatureMotor2_Label->setVisible(false);
    ui->brakelight_Left->setVisible(false);
    ui->brakelight_Right->setVisible(false);

    ui->graphicsView_2->setStyleSheet("background:transparent;");
    QGraphicsScene *eCARusScene = new QGraphicsScene(this);
    ui->graphicsView_2->setScene(eCARusScene);
    QPixmap pix(":new/prefix2/photos/Car/eCARus_wo_signs.png");
    eCARusScene->setSceneRect(0,0,521,231);
    QGraphicsItem *eCARus_GraphicsItem = eCARusScene->addPixmap(pix);
    eCARus_GraphicsItem->moveBy(-2,11);
    QTimeLine *animationTimer = new QTimeLine(1000);
    animationTimer->setFrameRange(0, 100);
    QGraphicsItemAnimation *animation = new QGraphicsItemAnimation;
    animation->setItem(eCARus_GraphicsItem);
    animation->setTimeLine(animationTimer);
    for (int i = 100; i >=0; i--)
        animation->setPosAt(i/100.0,QPointF(i*6-3,11));
    ui->graphicsView_2->show();
    animationTimer->start();
    connect(animationTimer,SIGNAL(finished()),this,SLOT(depart_Animation_finished()));
    connect(animationTimer,SIGNAL(finished()),animation,SLOT(deleteLater()));
    connect(animationTimer,SIGNAL(finished()),eCARusScene,SLOT(deleteLater()));
    connect(animationTimer,SIGNAL(finished()),animationTimer,SLOT(deleteLater()));
}
void MainWindow::depart_Animation_finished()
{
    ui->keyAssignment_Button->setEnabled(true);

    ui->Controller_Groupbox->setVisible(true);
    ui->indicatorlight_Left->setVisible(false);
    ui->indicatorlight_Right->setVisible(false);
    ui->indicatorlight_Left_Back->setVisible(false);
    ui->indicatorlight_Right_Back->setVisible(false);
    ui->headlight_Left->setVisible(false);
    ui->headlight_Right->setVisible(false);
    ui->backlight_Left->setVisible(false);
    ui->backlight_Right->setVisible(false);
    ui->fullbeam_Left->setVisible(false);
    ui->fullbeam_Right->setVisible(false);
    ui->tireLeft->setVisible(false);
    ui->tireRight->setVisible(false);
    ui->interpretedDataComboBox->setVisible(false);
    ui->interpretedDataLabel->setVisible(false);
    ui->temperatureMotor1_Label->setVisible(false);
    ui->temperatureMotor2_Label->setVisible(false);
    ui->brakelight_Left->setVisible(false);
    ui->brakelight_Right->setVisible(false);
    ui->graphicsView_2->setScene(0);
  //  ui->graphicsView_2->setStyleSheet("background-image: url(':/new/prefix2/photos/Car/ECARUS_Car_1_65.png');background-color:none;background-repeat:no-repeat;");
}

//Mouseover effect of the question mark in the Debug tab
void MainWindow::on_helpWindowMouseEntered()
{
    ui->Debug_Help_Label->setFixedHeight(180);
    ui->Debug_Help_Label->setFixedWidth(200);
    ui->Debug_Help_Label->setStyleSheet("text-align:left;background-image:url(':/new/prefix2/photos/Metal_Background.png');");
    ui->Debug_Help_Label->setText("[♦] In manual mode, the message's \n       ID won't be sent in little endian\n      contrary to the 'Guided Mode'\n[♦] The checksum is computed in 3 \n      steps:\n1) Sum calculation of the config and\n      data bytes \n2) Conjunction with 0xFF\n3) Invertation");
}
void MainWindow::on_helpWindowMouseLeaved()
{
    ui->Debug_Help_Label->setFixedHeight(50);
    ui->Debug_Help_Label->setFixedWidth(50);
    ui->Debug_Help_Label->setStyleSheet("background-image:url(':/new/prefix2/photos/help.png');background-repeat:no-repeat;");
    ui->Debug_Help_Label->setText("");
}
void MainWindow::on_helpWindowMouseClicked()
{
        ui->Debug_Help_Label->setStyleSheet("text-align:left;background:white;");
}

//*********************
//      OTHER FUNCTIONS
//*********************

// The old interpretation method is to safe incoming interpreted data in a struct called LatestDataInterpreted.
//  This is not easy costumizeable, so the new method is to compare each incoming message with a JSON-File.
//  The last values are safed in the QMap Json_Map and easy to access.

// TO-DO: remove anything related to LatestDataInterpreted and
//        replace the Daten_konvertieren-function in the interprete thread with
//        the GetJSONInterpretation function
//TO-DO: complete the JSON config file <-READ WIKI ENTRY;
void MainWindow::GetJSONInterpretation(QString ID, unsigned char data[])
{
    QJsonValue value = JsonList.value(ID);
    QJsonArray array = value.toArray();     //list of the ID related information
    foreach (const QJsonValue & v, array)   //
    {
         QJsonObject key = v.toObject();

         int Value_Int = 0;
         bool Value_Bool = false;
         double Value_Double = 0.0;
         int byte_start = key.value("Byte_Start").toInt();
         int byte_end = key.value("Byte_End").toInt();
         int bit_start = key.value("Bit_Start").toInt();
         int bit_end = key.value("Bit_End").toInt();         
         QString text ="";
         text = key.value("Text").toString();
         QString unit = key.value("Unit").toString();
         double Scalefactor = key.value("Scale").toDouble();
         int AdjustValue = key.value("AdjustValue").toInt();
         int SignBitNumber = key.value("SignBitNumber").toInt();//number of the sign bit, null, if the Value is unsigned
         //calculate the message's Value
         if(byte_end-byte_start != 0)
         {
             for(int i = 0; i<=byte_end-byte_start; i++)
             {
                 Value_Int += data[i+byte_start] << 8*i;
             }
         }
         else if(bit_end-bit_start != 0)
         {
            Value_Int = (data[byte_start]>>bit_start) & (0xFF>>7-bit_end);
         }
         Value_Int+=AdjustValue;
         if(SignBitNumber != 0)
             Value_Int = this->UnsignedIntToSignedInt(Value_Int, SignBitNumber); //The value's MSB is interpreted as a sign
         if((key.value("DataType").toString())==("double"))
         {
            Value_Double = Value_Int * Scalefactor;
            Json_Map.insert(text,QString("%1").arg(Value_Double, 0, 'g', 13)+" "+unit);
         }
         else if((key.value("DataType").toString())==("int"))
         {
            Value_Int = (int)(Value_Int * Scalefactor);
            Json_Map.insert(text,QString::number(Value_Int)+" "+unit);
         }
         else if((key.value("DataType").toString())==("bool"))
         {
            Value_Bool = (data[byte_start] >> bit_start) & 0x01;
            Json_Map.insert(text,QString(Value_Bool? "true":"false"));
         }
           // qDebug() << Json_Map.value(text);
     }
}
int MainWindow::UnsignedIntToSignedInt(int unsignedint, int MSBindex) // This function gets an unsigned Integer and interprets the MSB as a sign bit and returns a signed int
{
    /*
        unsignedint = 00..001010101110 = 686 , MSBindex=9 -> has to be intepreted as a negative number
        signedint = 1...101010010 = -174
     */
    if((unsignedint & (0x01 << MSBindex)) != 0) //negative?
    {
        return -(unsignedint & ((0x01 << MSBindex) -1));
    }
    else
        return unsignedint; //unsigned is positive
}
void MainWindow::closeEvent(QCloseEvent *event)
{

}

void MainWindow::on_actionQuit_triggered()
{
    close();
}
/**
 * @brief MainWindow::on_actionGet_Help_triggered ( it opens a HTML page ( Help Window) )
 */
void MainWindow::on_actionGet_Help_triggered()
{
    QString link= "https://ecarus.wiki.tum.de/Telemetrie+%26+Fernsteuerung";
    QDesktopServices::openUrl(QUrl(link));
}

void MainWindow::on_actionAbout_triggered()
{
    QString link= "http://ecarus.wiki.tum.de/";
    QDesktopServices::openUrl(QUrl(link));
    /*SecDialog secdialog ;
    secdialog.setModal (true);
    secdialog.exec();*/
}

