#include "execthread.h"

ExecThread::ExecThread(QObject *parent) :
    QThread(parent)
{
}

void ExecThread::run()
{
    exec();
}
