#ifndef EXECTHREAD_H
#define EXECTHREAD_H

#include <QThread>

class ExecThread : public QThread
{
    Q_OBJECT
public:
    explicit ExecThread(QObject *parent = 0);
    void run();
signals:
    
public slots:
    
};

#endif // EXECTHREAD_H
