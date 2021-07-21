#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDateTime>
#include <QFileDialog>
#include <QCoreApplication>
#include <QFile>

class Q_Qchat;
#include <q_qchat.h>

namespace Ui {
class ChatWidget;
}

class ChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWidget(QTcpSocket *,QString,QString,Q_Qchat *,QWidget *parent = 0);
    ~ChatWidget();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::ChatWidget *ui;
    QTcpSocket *tcpsocket;
    QString userName;
    QString friendName;
    Q_Qchat *qqchat;

    QDateTime current_date_time = QDateTime::currentDateTime();
};

#endif // CHATWIDGET_H
