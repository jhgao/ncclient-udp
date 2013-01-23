#include "datahandler.h"

DataHandler::DataHandler(QObject *parent) :
    QObject(parent)
{
}


void DataHandler::sigWriteOutCmd(quint16 cmd, const QByteArray arg)
{
    emit sig_writeOutCmd(cmd,arg);
}
