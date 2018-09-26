#ifndef MOTORON_H
#define MOTORON_H
#include <QProcess>
#include <QThread>
class motorOn:public QThread
{
public:
    motorOn();
    void run();
};

#endif // MOTORON_H
