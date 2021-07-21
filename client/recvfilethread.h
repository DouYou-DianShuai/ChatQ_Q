#ifndef RECVFILETHREAD_H
#define RECVFILETHREAD_H

#include <QThread>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTcpSocket>
#include <QHostAddress>
#include <QFile>


class RecvFileThread : public QThread
{
    Q_OBJECT
public:
    RecvFileThread(QJsonObject &);
    void run();

private:
    QString fileName;
    int fileLength;
    int totle;
    int port;
    QTcpSocket *recvSocket;
    QFile *file;

};


#endif // RECVFILETHREAD_H
