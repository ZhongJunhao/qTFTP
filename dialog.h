#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "dialoggetfilename.h"
typedef quint16 uint16_t;
#include "tftp.h"
#include <QMessageBox>
#include <QtEndian>
#include <QFile>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QHostAddress>
#include <QTimer>
#include <QFileDialog>
#include <QFileInfo>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
private:
    void sendReQ(QUdpSocket *sock,QHostAddress &haddr,qint16 port,char *fileName , uint16_t rw);
    void sendACK(QUdpSocket *sock,QHostAddress &haddr,qint16 port,uint16_t block);
    void sendDat(QUdpSocket *sock,QHostAddress &haddr,qint16 port,char *pdata,qint16 size);
    void procPack(QUdpSocket *sock,QHostAddress &haddr,qint16 port,char *data,qint16 size);

    QUdpSocket *sockd;
    QFile * file;
    uint16_t curBlock;
    DialogGetFileName *dlg ;
    QTimer *timer;
    int count;
    int recvCount;
    double speed;
private slots:
    void on_pushButton_clicked();
    void recvDat();
    void timeCount();
    void on_pushButton_2_clicked();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
