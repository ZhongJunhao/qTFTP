#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    sockd = new QUdpSocket();
    if(sockd->bind(7755))
    {
        qDebug()<<"bind succ";
    }
    dlg = new DialogGetFileName(this);
    timer = new QTimer(this);
    count = 0;
    recvCount = 0;
    connect(sockd,SIGNAL(readyRead()),this,SLOT(recvDat()));
    connect(timer,SIGNAL(timeout()),this,SLOT(timeCount()));
}

void Dialog::timeCount()
{
    //static int times = 0;
    speed = count * 5 *0.5;
    count = 0;
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::sendReQ(QUdpSocket *sock, QHostAddress &haddr, qint16 port, char *fileName ,uint16_t rw)
{
    TFTPhdr_t * TFTPhdr;
    size_t len = strlen("octet")+1 + strlen(fileName)+1 + 4;
    TFTPhdr = (TFTPhdr_t *)malloc(len);
    TFTPhdr->th_opcode = qToBigEndian((uint16_t)rw);
    char *p = (char *)TFTPhdr;
    memcpy(p+2,fileName,strlen(fileName)+1);
    p+= (2 +strlen(fileName)+1);
    qDebug()<<fileName;
    qDebug()<<strlen(fileName);
    memcpy(  p,  "octet" , strlen("octet")+1);
 //  qDebug("tt:%d  %d ",TFTPhdr,(char*)(TFTPhdr->th_stuff)  + strlen(fileName) + 1 );
   qDebug()<<(char*)(TFTPhdr->th_stuff)  + strlen(fileName) + 1;
   int temp =  sock->writeDatagram((char *)TFTPhdr,len,haddr,port);
   qDebug()<<len<<temp;
}

void Dialog::sendACK(QUdpSocket *sock, QHostAddress &haddr, qint16 port, uint16_t block)
{
    TFTPhdr_t * TFTPhdr;
    size_t len = sizeof(uint16_t) * 2;
    TFTPhdr = (TFTPhdr_t *)malloc(len);
    TFTPhdr->th_opcode = qToBigEndian((uint16_t)ACK);
    TFTPhdr->th_block = block;
    sock->writeDatagram((char *)TFTPhdr,len,haddr,port);
}

void Dialog::sendDat(QUdpSocket *sock, QHostAddress &haddr, qint16 port, char *pdata, qint16 size)
{
    TFTPhdr_t * TFTPhdr;
    TFTPhdr = (TFTPhdr_t*)malloc(size+(sizeof(uint16_t)*2));
    TFTPhdr->th_opcode = qToBigEndian((uint16_t)DATA);
    TFTPhdr->th_block = qToBigEndian((uint16_t)curBlock);
    curBlock++;
    memcpy(TFTPhdr->th_data,pdata,size);
    sock->writeDatagram((char *)TFTPhdr,size+sizeof(uint16_t)*2,haddr,port);
}


void Dialog::procPack(QUdpSocket *sock, QHostAddress &haddr, qint16 port, char *data, qint16 size)
{
    TFTPhdr_t *TFTPhdr = (TFTPhdr_t *)data;
    char strCount[100];
    char buff[600];
    int bufSize;
    switch(qFromBigEndian((uint16_t)TFTPhdr->th_opcode))
    {
    case DATA:
        qDebug()<<"get data pack";
        if(size<PACKSIZE)
        {
            file->write(TFTPhdr->th_data,size-(sizeof(uint16_t)*2));
            file->close();
            delete file;
            sprintf(strCount,"get: %d bytes \n.......finish!",recvCount);
            ui->pushButton->setEnabled(1);
            ui->pushButton_2->setEnabled(1);
            ui->textBrowser->setText(strCount);
            timer->stop();
        }
        else
        {
            file->write(TFTPhdr->th_data,SEGSIZE);
            sprintf(strCount,"get: %d bytes \n.......speed: %0.2f Kb/s",recvCount,speed);
            ui->textBrowser->setText(strCount);
        }
        sendACK(sock,haddr,port,TFTPhdr->th_block);
        count ++;
       // QString str;
        recvCount += size - 4;
        qDebug()<<strCount;
        break;

    case ACK:
        bufSize = file->read(buff,512);
        if(bufSize<0)
        {
            file->close();
            delete file;
            return;
        }
        if(bufSize<512)
        {
            sprintf(strCount,"put: %d bytes \n ....finished/s\n",recvCount);
            timer->stop();
            ui->pushButton->setEnabled(1);
            ui->pushButton_2->setEnabled(1);
        }
        else
        {
            sprintf(strCount,"put: %d bytes \n ....speed: %0.2f Kb/s\n",recvCount,speed);
        }
        ui->textBrowser->setText(QString(strCount));
        count ++;
        recvCount += bufSize - 4;
        sendDat(sockd,haddr,port,buff,bufSize);
        break;

    case ERROR:
        qDebug()<<"err:"<<TFTPhdr->th_data;
        QMessageBox::warning(this,tr("错误"),TFTPhdr->th_data);
        ui->pushButton->setEnabled(1);
        ui->pushButton_2->setEnabled(1);
        break;
    default:
        qDebug("proc Bad pack!");
        exit(0);

    }
}

void Dialog::recvDat()
{
    static QHostAddress haddr;
    static quint16 port;
    static QByteArray dataGram;
    while(sockd->hasPendingDatagrams())
    {
        dataGram.resize(sockd->pendingDatagramSize());
        sockd->readDatagram(dataGram.data(),dataGram.size(),&haddr,&port);
        procPack(sockd,haddr,port,dataGram.data(),dataGram.size());
    }
}

void Dialog::on_pushButton_clicked()
{
    dlg->show();
    if(dlg->exec() == QDialog::Accepted)
    {

        qDebug("dlg Accept!");
        QString dst = ui->edtDst->text();
        int in = dst.indexOf(':');
    //    haddr = new QHostAddress(dst.left(in));

        static quint16 port = dst.mid(in+1).toInt();
        QHostAddress haddr(dst.left(in));

        qDebug()<<dst.left(in)<<"   "<<dst.mid(in+1)<< port;

        qDebug()<<"here";
        QString fileName = dlg->fileName;
        if(fileName.isEmpty())
        {
            QMessageBox::warning(this,tr("错误"),tr("请正确输入文件名"));
            return;
        }
        qDebug()<<fileName;
        this->file = new QFile(fileName);
        if(!file->open(QIODevice::WriteOnly))
        {
            QMessageBox::warning(this,tr("错误"),tr("文件打开错误！"));
            return;
        }

        sendReQ(sockd,haddr,port,(char *)fileName.toLatin1().data(),RRQ);
        timer->start(200);
        curBlock = 0;
        recvCount = 0;
        count = 0;
        ui->pushButton->setDisabled(1);
        ui->pushButton_2->setDisabled(1);

    }
}

void Dialog::on_pushButton_2_clicked()
{
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this,tr("打开"),tr("/home"));
    if(!fileName.isEmpty())
    {
        qDebug()<<fileName;
        file = new QFile(fileName);
        if(!file->open(QFile::ReadOnly))
        {
             QMessageBox::warning(this,tr("错误"),tr("请正确输入文件名"));
             return;
        }
        QFileInfo finfo(fileName);
        QString dst = ui->edtDst->text();
        int in = dst.indexOf(':');
        static quint16 port = dst.mid(in+1).toInt();
        QHostAddress haddr(dst.left(in));
        sendReQ(sockd,haddr,port,finfo.fileName().toLatin1().data(),WRQ);
        timer->start(200);
        curBlock = 0;
        recvCount = 0;
        count = 0;
        ui->pushButton->setDisabled(1);
        ui->pushButton_2->setDisabled(1);
    }
}
