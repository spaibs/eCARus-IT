#ifndef PERIODICSENDTHREAD_H
#define PERIODICSENDTHREAD_H

#include <QThread>
#include <QMutex>
#include <vector>
#include "datatypes.h"
#include <QTimer>

extern std::vector <PeriodicSendObject> PeriodicSendObjectVector;	// vector with all periodic send informations.
extern QMutex* PeriodicSendObjectVectorLock;

class PeriodicSendThread : public QThread
{
    Q_OBJECT
public:
    explicit PeriodicSendThread(QObject *parent = 0);

protected:
    void run();
private:
    int least_Interval;
};


#endif // PERIODICSENDTHREAD_H
