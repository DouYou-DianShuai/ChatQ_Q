#include "creategroup.h"
#include "ui_creategroup.h"

Creategroup::Creategroup(QTcpSocket *s,QString u,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Creategroup)
{
    ui->setupUi(this);
    tcpsocket = s;
    userName = u;
    this->setWindowTitle(QString("%1正在创建群聊~、~").arg(userName));
    this->setWindowIcon(QIcon(":/new/prefix1/a_5"));

}

Creategroup::~Creategroup()
{
    delete ui;
}

void Creategroup::on_pushButton_clicked()
{
    ui->lineEdit->clear();
    this->hide();
}

void Creategroup::on_pushButton_2_clicked()
{
    QString groupName = ui->lineEdit->text();
    QJsonObject obj;
    obj.insert("cmd","create_group");
    obj.insert("user",userName);
    obj.insert("group",groupName);
    QByteArray ba = QJsonDocument(obj).toJson();
    tcpsocket->write(ba);
    ui->lineEdit->clear();
    this->hide();
}
