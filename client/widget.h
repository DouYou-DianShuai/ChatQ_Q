#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QHostAddress>
#include <QMessageBox>
#include <QIcon>
#include <QJsonObject>
#include <QJsonDocument>
#include "register.h"
#include <QCloseEvent>
#include "q_qchat.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    void client_register_handler(QString);
    void client_login_handler(QString,QString,QString);

private slots:
    void on_registerButton_clicked();

    void on_loginButton_clicked();

private:
    Ui::Widget *ui;
    QTcpSocket *tcpSocket;
    Register *w_register;
    QString userName;
    Q_Qchat *c;
protected:
    void closeEvent(QCloseEvent *event);//重写关闭事件
    void keyPressEvent(QKeyEvent *event);//重写回车事件

};

#endif // WIDGET_H
