#include "datahandler.h"

DataHandler::DataHandler(QObject *parent) :
    QObject(parent)
{
}


void DataHandler::sigWriteOutCmd(eControl_CMD cmd, const QByteArray arg)
{
    emit sig_writeOutCmd(cmd,arg);
}
