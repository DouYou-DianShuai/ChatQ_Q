#ifndef GROUPCHATWIDGET_H
#define GROUPCHATWIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDateTime>

class Q_Qchat;
#include "q_qchat.h"

namespace Ui {
class GroupChatWidget;
}

class GroupChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GroupChatWidget(QTcpSocket *,QString,QString,Q_Qchat *, QWidget *parent = 0);
    ~GroupChatWidget();

private slots:
    void on_pushButton_clicked();

private:
    Ui::GroupChatWidget *ui;
    QTcpSocket *tcpsocket;
    QString userName;
    QString groupName;
    Q_Qchat *qchat;
    QDateTime current_date_time;
};

#endif // GROUPCHATWIDGET_H
