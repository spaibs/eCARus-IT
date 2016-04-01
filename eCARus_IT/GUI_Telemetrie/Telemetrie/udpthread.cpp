#include "udpthread.h"
#include <vector>
#include "configuration.h"
#include "utility.h"
#include <queue>
#include <stdlib.h>
#include <stdio.h>

std::queue<struct RawData> SendData;
std::queue<RecvBuf> RecQueue;
QMutex* NetLock;
QMutex* RecQueueLock;

UDPThread::UDPThread(QObject *parent) :
    QThread(parent)
{
}

void UDPThread::run()
{
    NetLock->lock();
    NetLock->unlock();
    finished = false;
    udpSocket = new QUdpSocket();

    /*Bind socket to any address with certain port*/
    bool a = udpSocket->bind(m_port.toInt(0,10), QUdpSocket::ShareAddress);

    while(!finished)
    {
        NetLock->lock();

        if(!SendData.empty())
        {
            char send_msg[14];
            for (int i = 0; i < 14; i++)
            {
                send_msg[i] = SendData.front().RawData[i];
            }
            SendData.pop();
            NetLock->unlock();
            /*send datagram*/
            QByteArray datagram = QByteArray::fromRawData( send_msg, sizeof(send_msg) );
            udpSocket->writeDatagram(datagram.data(),
                                     datagram.size(),
                                     QHostAddress(m_host),       //QHostAddress(m_host) oder QHostAddress::Broadcast
                                     m_port.toInt(0,10));
        }
        else
        {
            NetLock->unlock();   //move that command
        }

        /*try to receive data*/
        if(udpSocket->hasPendingDatagrams())
        {
            QByteArray datagram;
            datagram.resize(14);            
            udpSocket->readDatagram(datagram.data(), 14);

            emit receive_datagram(datagram);

            /* Push received data */

            RecvBuf tmp;
            for (int i=0; i < 14; i++)
            {
                tmp.data[i] = datagram.at(i);
            }
            tmp.timestamp = QDateTime::currentDateTime();
            tmp.ticks = tmp.timestamp.toString("szzz").toDouble();
            RecQueueLock->lock();
            RecQueue.push(tmp);
            RecQueueLock->unlock();
        }
        msleep(1);
    }  // the end of the main loop


    udpSocket->close();
    return;
}




void UDPThread::UIswitchtoconnect()
{
    /*
        changes the status displayed in the UI to connected
        This function has to be called with Synchronize(&UIswitchtoconnect)
    */
    //    Form1->connect_button->Caption = "Disconnect";
    //    Form1->connect_button->Enabled=true;
    //    Form1->ConnectDisconnect1->Enabled = true;
    //    Form1->StatusBar1->Panels->Items[0]->Text="Connected to: " + Form1->RemoteHost;
    //    Form1->StatusBar1->Panels->Items[1]->Text="NO HEARTBEAT";
    //    + ":" + Form1->RemotePort;
}

void UDPThread::SetupThread(QString host, QString port)
{
    m_host = host;
    m_port = port;
}

void UDPThread::FinishThread()
{
    finished = true;
}
