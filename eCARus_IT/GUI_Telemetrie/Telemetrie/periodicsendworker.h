#ifndef PERIODICSENDWORKER_H
#define PERIODICSENDWORKER_H

#include <QThread>
#include <QMutex>
#include <vector>
#include "datatypes.h"
#include <QTimer>

extern std::vector <PeriodicSendObject> PeriodicSendObjectVector;	// vector with all periodic send informations.
extern QMutex* PeriodicSendObjectVectorLock;
extern QTime time;
/**
 * @brief The PeriodicSendWorker periodically sends messages created in the debug tab. Each message's cycle is individually adjustable
 * The PeriodicSendWorker is created by the Main Thread to be sent afterwards to a new thread.
 */
class PeriodicSendWorker : public QObject
{
    Q_OBJECT
public:
    PeriodicSendWorker();
    ~PeriodicSendWorker();

public slots:
    void process_start();
    void process_stop();
signals:
    void finished(int interval);
private:
    int greatest_Interval;

};


#endif // PERIODICSENDTHREAD_H
