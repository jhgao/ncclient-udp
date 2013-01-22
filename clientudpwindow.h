#ifndef CLIENTUDPWINDOW_H
#define CLIENTUDPWINDOW_H

#include <QMainWindow>
#include "gui/rcvprogressscene.h"

namespace Ui {
class ClientUDPWindow;
}

class ClientUDPWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit ClientUDPWindow(QWidget *parent = 0);
    ~ClientUDPWindow();
    
private:
    Ui::ClientUDPWindow *ui;
};

#endif // CLIENTUDPWINDOW_H
