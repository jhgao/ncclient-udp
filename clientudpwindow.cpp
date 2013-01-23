#include "clientudpwindow.h"
#include "ui_clientudpwindow.h"

ClientUDPWindow::ClientUDPWindow(QWidget *parent) :
    QMainWindow(parent),m_con(0),m_conThread(0),
    ui(new Ui::ClientUDPWindow),m_isConnected(false)
{
    ui->setupUi(this);


    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(100);

    ui->graphicsView->setScene(&m_scene);
    connect(ui->graphicsView, SIGNAL(sig_resized(QSize)),
            &m_scene, SLOT(arragneToView(QSize)));
    //find a local ip
    QList<QHostAddress> ipAddressesList;
    QString ipAddress;

    ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
                ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();

    ui->lineEdit_serverAddr->setText(ipAddress);
    ui->lineEdit_serverPort->setText(QString::number(SERVER_DEFAULT_PORT));

    qDebug() << "GUI thread " << this->thread();
    //connection
    if(!m_con)  m_con = new Connection();
    connect(m_con, SIGNAL(sig_progressPercent(uint)),
            this, SLOT(updateProgress(uint)));
    connect(m_con, SIGNAL(sig_gotBlockSN(quint32)),
            this, SLOT(onGotBlock(quint32)));
    connect(m_con, SIGNAL(connected()),
            this, SLOT(onConnected()));
    connect(m_con, SIGNAL(disconnected()),
            this, SLOT(onDisconnected()));

    connect(this, SIGNAL(sig_onConAbortCmd()),
            m_con, SLOT(slot_abortWorks()));
    connect(this, SIGNAL(sig_onConConnectToHostCmd(QString,quint16)),
            m_con, SLOT(slot_connectToHost(QString,quint16)));

    //handle connection in another thread
    if( !m_conThread)   m_conThread = new ConnectionThread(this);
    m_con->moveToThread(m_conThread);
    m_conThread->start();
}

ClientUDPWindow::~ClientUDPWindow()
{
    m_conThread->quit();
    m_conThread->wait(20);
    delete ui;
}

void ClientUDPWindow::updateProgress(const unsigned int p)
{
    ui->progressBar->setValue(p);
}

void ClientUDPWindow::onGotBlock(const quint32 bsn)
{
    m_scene.gotBlock(bsn);
}

void ClientUDPWindow::onConnected()
{
    m_isConnected = true;
    ui->groupBox_server->setDisabled(true);
    ui->pushButton_linkServer->setText("Disconnect");
}

void ClientUDPWindow::onDisconnected()
{
    m_isConnected = false;
    ui->groupBox_server->setEnabled(true);
    ui->pushButton_linkServer->setText("Connect");
}


void ClientUDPWindow::on_pushButton_linkServer_clicked()
{
    if(m_isConnected){
        emit sig_onConAbortCmd();
    }else{
        emit sig_onConConnectToHostCmd(
                    ui->lineEdit_serverAddr->text(),
                    (quint16)ui->lineEdit_serverPort->text().toInt());
    }
}
