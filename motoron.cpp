#include "motoron.h"


motorOn::motorOn()
{
//public:

}

void motorOn::run()
{
    QProcess::execute("./dcm_test");

}
