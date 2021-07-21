#include "recvfilethread.h"

RecvFileThread::RecvFileThread(QJsonObject &obj)
{
    totle = 0;
    port = obj.value("port").toInt();
    fileLength = obj.value("length").toInt();

    QString  pathName = obj.value("filename").toString();
    QStringList strList = pathName.split('/');
    fileName = strList.at(strList.size()-1);
}

void RecvFileThread::run()
{
    file = new QFile(fileName);
    file->open(QIODevice::WriteOnly);
    qDebug()<<fileLength;

    recvSocket = new QTcpSocket;
    connect(recvSocket,&QTcpSocket::readyRead,this,[=](){
        QByteArray ba = recvSocket->readAll();
        totle += ba.size();
        file->write(ba);
        qDebug()<<totle;
        qDebug()<<"1";


        if(totle>=fileLength)
        {
            qDebug()<<"5";
            file->close();
            recvSocket->close();
            delete file;
            delete recvSocket;
            this->quit();
        }

    },Qt::DirectConnection);//这第五个参数，可以使槽函数运行在接收者所在线程

    recvSocket->connectToHost(QHostAddress("192.168.0.112"),port);
    if(!recvSocket->waitForConnected(10000))
    {
        this->quit();
        qDebug()<<"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    }

    exec();
}

