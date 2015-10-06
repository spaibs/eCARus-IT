#ifndef INTERPRETETHREAD_H
#define INTERPRETETHREAD_H

#include <QObject>
#include <vector>
#include <QThread>
#include <QMutex>
#include "datatypes.h"
#include <QTimer>
#include <QJsonObject>

/* extern Definition of global variables */
extern std::vector <struct InterpretedData> InterpretedHistory;
extern std::vector <struct RawData> RawHistory;
extern QMutex* HistoryLock;
extern QMutex* RecQueueLock;
extern QMutex* NetLock;

/**
 * @brief The InterpreteThread class interpretes the incoming UDP messages.
 * Messages with known ID's are saved to LatestDataInterpreted.
 * Each message will be saved to the vector LatestRawData and
 * the data log will be saved in InterpretedHistory and RawHistory
 */
class InterpreteThread : public QThread
{
    Q_OBJECT
public:
    explicit InterpreteThread(QObject *parent = 0);
    /**
     * @brief Daten_konvertieren
     * @param[IN] Ethernet_Msg
     *  Interpretes the data from a ethernet message, by examining the message's ID, that is send to the function
     *  and stores it in the global variable LatestDataInterpreted.
     *  The data belonging to other IDs will not be changed.
     */
    void Daten_konvertieren(Ethernet_Msg Msg);
    void InterpreteFromJson(Ethernet_Msg Msg);
    void init_LatestDataInterpreted(struct InterpretedData*);
    void init_receivedMsgVector(std::vector <Ethernet_Msg>*);

    void init_LatestRawData(struct RawData*);
    void init_AvailableData(int*);
    bool rearok ;
    bool frontok;

protected:
    /**
     * @brief run()
     *  -Main function. will loop as long as the thread is not terminated.
     *  -It dequeues the messages from the Queue RecQueue and convertes them: RecvBuf to Ethernet_Msg
     *  -Does the interpretation and displaying by calling the needed functions.
     *  -Data will be saved in the LatestRawData vector
     *  -The interpretation is the task of Daten_konvertieren(Ethernet_Msg msg)
     *  -The data log will be saved in InterpretedHistory and RawHistory
     */

    void run();

private:
    long HeartBeatCycle;
    long Time_ms [2048];
    int AvaibleData;
    int *ptr_AvaibleData;		///< can ids can be in the range 0 - 0x7FF
    QTimer* frontTimeOut;
    QTimer* rearTimeOut;

    struct RawData LatestRawData; ///< stores the last received data;
    struct RawData	*ptr_LatestRawData;
    struct InterpretedData LatestDataInterpreted; ///< stores the most recent version of all interpreted data
    struct InterpretedData *ptr_LatestDataInterpreted;
    //QJsonObject JsonList;
    //QJsonObject JsonSubList;
    //QJsonObject* ptr_JsonList;
    Ethernet_Msg receivedMsg;
    std::vector <Ethernet_Msg> receivedMsgVector; ///< stores the most recent version of all receivedMsg Data
    std::vector <Ethernet_Msg> *ptr_receivedMsgVector;  	///< GUI Version of all important datatypes

private slots:
    /**
     * @brief frontTimedOut checks the functionality of the front K60
     */
    void frontTimedOut();
    /**
     * @brief frontTimedOut checks the functionality of the rear K60's
     */
    void rearTimedOut();
};

#endif // INTERPRETETHREAD_H
