#ifndef Q_QCHAT_H
#define Q_QCHAT_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include "addfriend.h"
#include "addgroup.h"
#include "creategroup.h"
#include <QCloseEvent>
#include "sendfilethread.h"
#include "recvfilethread.h"

class ChatWidget;
#include <chatwidget.h>

class GroupChatWidget;
#include <groupchatwidget.h>

namespace Ui {
class Q_Qchat;
}

struct CWInfo {
    ChatWidget *w;
    QString name;
};

struct GcwInfo {
    GroupChatWidget *w;
    QString name;

};

class Q_Qchat : public QWidget
{
    Q_OBJECT

public:
    explicit Q_Qchat(QTcpSocket *,QString,QString,QString,QWidget *parent = 0);
    ~Q_Qchat();
    void client_login_reply(QString);
    void client_add_friend_reply(QJsonObject &);
    void client_create_group_reply(QJsonObject &);
    void client_add_group_reply(QJsonObject &);
    void cLIent_private_chat_reply(QString,QString);
    void client_chat_reply(QJsonObject &);
    void client_get_group_member_reply(QJsonObject &);
    void client_group_chat_reply(QJsonObject &);
    void client_friend_offline(QString);
    void client_send_file_reply(QJsonObject &);
    void client_send_file_port_reply(QJsonObject &);
    void client_recv_file_port_reply(QJsonObject &);

private slots:
    void on_addButton_clicked();
    
    void on_createGroupButton_clicked();

    void on_addgroupButton_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::Q_Qchat *ui;
    QTcpSocket *tcpsocket;
    QString userName;
    Addfriend *addFriendWidget ;
    Creategroup *createGroupWidget;
    Addgroup *addGroupWidget;

    std::list<CWInfo *> *chatwidget;
    QList<GcwInfo *> *groupWidtetlist;
signals:
    void mySignal();
    void signal_to_chatwidget(QJsonObject &);
    void signal_to_chatwidget_offline(QString,QString);
    void signal_to_groupWidget_list(QJsonObject &);
    void signal_to_groupWidget_chat(QJsonObject &);
    void updata_group_member_list();  //用于更新群成员列表


protected:
    void closeEvent(QCloseEvent *event);

};

#endif // Q_QCHAT_H
