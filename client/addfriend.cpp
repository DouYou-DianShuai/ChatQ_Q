#include "addfriend.h"
#include "ui_addfriend.h"

Addfriend::Addfriend(QTcpSocket *s,QString u,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Addfriend)
{
    ui->setupUi(this);
    tcpsocket = s;
    userName = u;
    this->setWindowTitle(QString("%1正在添加好友~、~").arg(userName));
    this->setWindowIcon(QIcon(":/new/prefix1/a_5"));

}

Addfriend::~Addfriend()
{
    delete ui;
}

void Addfriend::on_pushButton_clicked()
{
    QString friendName = ui->userLineEdit->text();
    QJsonObject obj;
    obj.insert("cmd","add");
    obj.insert("user",userName);
    obj.insert("friend",friendName);

    QByteArray ba = QJsonDocument(obj).toJson();
    tcpsocket->write(ba);

    ui->userLineEdit->clear();
    this->close();


}

void Addfriend::on_cancelButton_2_clicked()
{
    ui->userLineEdit->clear();
    this->close();
}
