#ifndef SENDFILETHREAD_H
#define SENDFILETHREAD_H

#include <QJsonObject>
#include <QJsonDocument>
#include <QTcpSocket>
#include <QHostAddress>
#include <QThread>
#include <QFile>

class SendFileThread : public QThread
{
    Q_OBJECT
public:
    SendFileThread(QJsonObject);
    void run();

private:
    QString fileName;
    int fileLength;
    int port;

};

#endif // SENDFILETHREAD_H
