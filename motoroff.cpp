#include "motoroff.h"


motorOff::motorOff()
{
//public:

}

void motorOff::run()
{
   // QProcess *motor = new QProcess;
    QProcess::execute("/motor_stop.sh");

}
