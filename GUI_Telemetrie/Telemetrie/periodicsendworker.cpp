#include "periodicsendworker.h"
#include "udpthread.h"
#include <iostream>
#include "utility.h"
#include <QTimer>
std::vector <PeriodicSendObject> PeriodicSendObjectVector;	// vector with all periodic send informations.
QMutex* PeriodicSendObjectVectorLock;

PeriodicSendWorker::PeriodicSendWorker()
{

    PeriodicSendObjectVectorLock->lock();
    if ( !PeriodicSendObjectVector.empty() )  // do not iterate if the vector is empty
    {
        for (std::vector<PeriodicSendObject>::iterator i=PeriodicSendObjectVector.begin(); i != PeriodicSendObjectVector.end(); ++i)
        {
            i->lastsent = 0; // init
        }
    }



    PeriodicSendObjectVectorLock->unlock();
}
PeriodicSendWorker::~PeriodicSendWorker() {

}

void PeriodicSendWorker::process_stop()
{

}

void PeriodicSendWorker::process_start()
{
    int greatest_Interval=0;
    bool highrestimerok;

    // get the freq. of the high res timer (if avaible)
    //    highrestimerok = QueryPerformanceFrequency(&highrestimerfreq);

    // reinitzialise all objects
    // this must be done because otherwise if the periodic send function has been
    // suspended the programm would try to send all messages that could not be send
    // the time the feature was disabled.

        // reinit
        PeriodicSendObjectVectorLock->lock();
        if ( !PeriodicSendObjectVector.empty() )  // do not iterate if the vector is empty
        {
            for (std::vector<PeriodicSendObject>::iterator i=PeriodicSendObjectVector.begin(); i != PeriodicSendObjectVector.end(); ++i)
            {
                    //i->lastsent = 0; // init
            }
        }



        if ( PeriodicSendObjectVector.empty() )
        {                                            // if there are no data to send
            QThread::msleep(20);                    // wait a few milliseconds
            PeriodicSendObjectVectorLock->unlock();
            return;
        }

        time = QTime::currentTime();
        for (std::vector<PeriodicSendObject>::iterator i=PeriodicSendObjectVector.begin(); i != PeriodicSendObjectVector.end(); ++i)
        {
            QTime time_zero(0,0,0);
            if ( !i->active )
            {
                std::cout<<"not active!"<<std::endl;
                continue; // do not send inactive data
            }
            if (abs(time.msecsTo(time_zero))>= (i->lastsent+i->interval) )  //if more time has elapsed than the messages' intervall duration then SendData()
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
                std::cout<<"Mit Fehler: "<<abs((i->interval+i->lastsent)-abs(time.msecsTo(time_zero)))<<std::endl;    //write the sending delay error to the command window
                i->lastsent = abs(time.msecsTo(time_zero)); //save current time
            }
        }

        PeriodicSendObjectVectorLock->unlock();

        for (std::vector<PeriodicSendObject>::iterator i=PeriodicSendObjectVector.begin(); i != PeriodicSendObjectVector.end(); ++i)
        {
            greatest_Interval=utility::ggT(greatest_Interval,i->interval);  //calculate the greatest interval which allows to send all messages accurately
        }

        emit finished(greatest_Interval);
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



