#include "addgroup.h"
#include "ui_addgroup.h"

Addgroup::Addgroup(QTcpSocket *s,QString u,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Addgroup)
{
    ui->setupUi(this);
    tcpsocket = s;
    userName = u;

    this->setWindowTitle(QString("%1正在申请加群~、~").arg(userName));
    this->setWindowIcon(QIcon(":/new/prefix1/a_5"));


}

Addgroup::~Addgroup()
{
    delete ui;
}

void Addgroup::on_pushButton_clicked()
{
    ui->lineEdit->clear();
    this->hide();
}

void Addgroup::on_pushButton_2_clicked()
{
    QString groupName = ui->lineEdit->text();
    QJsonObject obj;
    obj.insert("cmd","add_group");
    obj.insert("user",userName);
    obj.insert("group",groupName);
    QByteArray ba = QJsonDocument(obj).toJson();
    tcpsocket->write(ba);
    ui->lineEdit->clear();
    this->hide();
}
