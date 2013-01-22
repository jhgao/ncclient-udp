#include "clientudpwindow.h"
#include "ui_clientudpwindow.h"

ClientUDPWindow::ClientUDPWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ClientUDPWindow)
{
    ui->setupUi(this);
}

ClientUDPWindow::~ClientUDPWindow()
{
    delete ui;
}
