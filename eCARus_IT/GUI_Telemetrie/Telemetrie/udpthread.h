#ifndef UDPTHREAD_H
#define UDPTHREAD_H

#include <QThread>
#include <QMutex>
#include <QtNetwork>
#include <string>
#include <vector>
#include <queue>
#include "datatypes.h"

extern std::queue<struct RawData> SendData;
extern std::queue<RecvBuf> RecQueue;
extern QMutex* NetLock;
extern QMutex* RecQueueLock;
/**
 * @brief The UDPThread class is able to send and receive messages
 *  Messages, which are resided in the UdpSocket,  will be enqueued to the RecvBuf-Queue RecQueue
 *  Messages, which are supposed to be send by the software, will be resided in the UdpSocket
 */

class UDPThread : public QThread
{
    Q_OBJECT
public:
    explicit UDPThread(QObject *parent = 0);
    void UIswitchtoconnect();
    void SetupThread(QString,QString);
    void FinishThread();

protected:
    /**
     * @brief run
     * Binds to the udp socket
     * SENDING: The method dequeues the messages, which are to be sent, from the message queue SendData
     *          and writes the RawData to the udp socket
     * RECEIVING: run() converts the datagrams of the udp socket to RecvBuf and pushes the data to the queue RecQueue
     */
    void run();

private:
    struct LogCommand LogData;
    bool finished;
    QUdpSocket *udpSocket;
    QString m_host;
    QString m_port;

signals:
    void receive_datagram(QByteArray );

};


#endif // UDPTHREAD_H
