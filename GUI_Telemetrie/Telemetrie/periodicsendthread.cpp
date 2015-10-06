#include "periodicsendthread.h"
#include "udpthread.h"
#include <iostream>
#include "utility.h"
std::vector <PeriodicSendObject> PeriodicSendObjectVector;	// vector with all periodic send informations.
QMutex* PeriodicSendObjectVectorLock;

PeriodicSendThread::PeriodicSendThread(QObject *parent) :
    QThread(parent)
{
}

void PeriodicSendThread::run()
{
    int least_Interval=0;
    bool highrestimerok;
    double currenttime;
    QTime time;
    time.start();

    // get the freq. of the high res timer (if avaible)
    //    highrestimerok = QueryPerformanceFrequency(&highrestimerfreq);

    // reinitzialise all objects
    // this must be done because otherwise if the periodic send function has been
    // suspended the programm would try to send all messages that could not be send
    // the time the feature was disabled.

    PeriodicSendObjectVectorLock->lock();

    if ( !PeriodicSendObjectVector.empty() )  // do not iterate if the vector is empty
    {
        for (std::vector<PeriodicSendObject>::iterator i=PeriodicSendObjectVector.begin(); i != PeriodicSendObjectVector.end(); ++i)
        {
            i->lastsent = 0; // init
        }
    }
    PeriodicSendObjectVectorLock->unlock();

    while ( true )
    {
        // check if there is a conection
        //            if ( Form1->connect_button->Caption == "Connect" )
        if(true)
        {
            // wait for a long time if there is no connection
          //  QThread::msleep(1000);

            // reinit
            PeriodicSendObjectVectorLock->lock();
            if ( !PeriodicSendObjectVector.empty() )  // do not iterate if the vector is empty
            {
                for (std::vector<PeriodicSendObject>::iterator i=PeriodicSendObjectVector.begin(); i != PeriodicSendObjectVector.end(); ++i)
                {
                    //i->lastsent = 0; // init
                }
            }
            //continue;
        }
        // save the current time
        if (highrestimerok)
        {
            // use the high res timer if possible
            //            QueryPerformanceCounter (&highrestimerticks);

            // convert

          //  currenttime = highrestimerticks.QuadPart *1000 / highrestimerfreq.QuadPart;
        }
        else
        {
            // fallback, less acurate
            //            currenttime = GetTickCount();
        }


      //  PeriodicSendObjectVectorLock->lock();
        if ( PeriodicSendObjectVector.empty() )
        {
            // if there are no data to send
            // wait a few milliseconds
            PeriodicSendObjectVectorLock->unlock();
            QThread::msleep(20);
            continue;
        }
        for (std::vector<PeriodicSendObject>::iterator i=PeriodicSendObjectVector.begin(); i != PeriodicSendObjectVector.end(); ++i)
        {
            if ( !i->active )
            {
                std::cout<<"not active!"<<std::endl;
                continue; // do not send inactive data
            }
            if ( time.elapsed() >= (i->lastsent+i->interval) )
            {
              std::cout<<"Nachricht: ";
                RawData tmp;
                for (int j=0; j < 14; j++)
                {
                    // copy the data
                    tmp.RawData[j] = i->data[j];
                 }

                std::string tmp_string(29,'0'); // (2*14)+1; contains the rawdata in hex format
               // std::string* tmp_string=new std::string(29,'0');
                utility::CharArrayToHexString(tmp_string, tmp.RawData, 14);

                std::cout<<""<<	tmp_string;
                std::cout<<" gesendet!!!"<<std::endl;
                std::cout<<"Mit Intervall: "<<i->interval<<std::endl;

                NetLock->lock();
                SendData.push(tmp);
                NetLock->unlock();
                // update the lastsend data
                std::cout<<"Mit Fehler: "<<(i->interval+i->lastsent)-time.elapsed()<<std::endl;
                i->lastsent = time.elapsed();

            }
        }

        PeriodicSendObjectVectorLock->unlock();

        for (std::vector<PeriodicSendObject>::iterator i=PeriodicSendObjectVector.begin(); i != PeriodicSendObjectVector.end(); ++i)
        {
            least_Interval=utility::ggT(least_Interval,i->interval);
        }

        QThread::msleep(least_Interval);   //sleep as long as possible!
        // sleep to keep the cpu utilization low

        // create a timer and wait for completation
        //        HANDLE timer;
        //        LARGE_INTEGER ft;

        //        ft.QuadPart = -2500; // wait for 250�s

        //        timer = CreateWaitableTimer(NULL, TRUE, NULL);
        //        SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
        //        WaitForSingleObject(timer, INFINITE);
        //        CloseHandle(timer);

    }
}



