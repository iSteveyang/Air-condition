#ifndef MOTOROFF_H
#define MOTOROFF_H
#include <QProcess>
#include <QThread>
class motorOff:public QThread
{
public:
    motorOff();
    void run();
};

#endif // MOTOROFF_H
