#include "sendfilethread.h"

SendFileThread::SendFileThread(QJsonObject obj)
{
    port = obj.value("port").toInt();
    fileName = obj.value("filename").toString();
    fileLength = obj.value("length").toInt();
}

void SendFileThread::run()
{
    QTcpSocket sendSocket;
    sendSocket.connectToHost(QHostAddress("192.168.0.112"),port);

    if(!sendSocket.waitForConnected(10000))
    {
        this->quit();
    }
    else
    {
        qint64 sendSize = 0;
        QFile file(fileName);
        file.open(QIODevice::ReadOnly);

        qint64 len;
        char buffer[4*1024] = {0};
        do
        {
                len = 0;
                len = file.read(buffer,sizeof(buffer));
                sendSocket.write(buffer,len);
                sendSocket.flush();
                sendSize +=len;

        }while(len);
        qDebug()<<"sendSize="<<sendSize;
        file.close();
//        sendSocket.close();
//        this->quit();

    }
}
