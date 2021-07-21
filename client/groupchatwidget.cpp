#include "groupchatwidget.h"
#include "ui_groupchatwidget.h"



GroupChatWidget::GroupChatWidget(QTcpSocket *s, QString g, QString u,Q_Qchat *q, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GroupChatWidget)
{
    ui->setupUi(this);
    tcpsocket = s;
    groupName = g;
    userName = u;
    qchat = q;
    this->setWindowIcon(QIcon(":/new/prefix1/a_5"));
    current_date_time = QDateTime::currentDateTime();

    //用于获取群成员信息，更新群成员
    connect(qchat,&Q_Qchat::updata_group_member_list,[=](){
        QJsonObject obj;
        obj.insert("cmd","get_group_member");
        obj.insert("group",groupName);
        QByteArray ba = QJsonDocument(obj).toJson();
        tcpsocket->write(ba);
        qDebug()<<"进来这里";
    });

        //接收到群成员字符串信息来显示群成员
    connect(qchat,&Q_Qchat::signal_to_groupWidget_list,[=](QJsonObject obj){
        qDebug()<<"看看有没有进来这里";
        if(obj.value("group").toString() == groupName)
        {
                 ui->listWidget->clear();
                 QStringList friList = obj.value("member").toString().split('|');
                 for(int i = 0; i < friList.size(); i++)
                 {
                    if(friList.at(i)!="")
                    {
                         QListWidgetItem * pItem =new QListWidgetItem(QIcon("://a_7"), friList.at(i)) ;
                         pItem->setSizeHint(QSize(60,36));
                         ui->listWidget->addItem(pItem);
                    }
                }

        }
    });

    //接收到群消息
    connect(qchat,&Q_Qchat::signal_to_groupWidget_chat,[=](QJsonObject obj){
        if(obj.value("group").toString() == groupName)
        {
            QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss");
            ui->textEdit->append(QString("<font color=\'blue\'>%1 %2发送:</b>\n").arg(current_date,
                                         obj.value("user").toString()));
            ui->textEdit->append(QString("%1\n").arg( obj.value("text").toString()));

        }
    });
}

GroupChatWidget::~GroupChatWidget()
{
    delete ui;
}

void GroupChatWidget::on_pushButton_clicked()
{
    QString text = ui->textEdit_2->toPlainText();
    QJsonObject obj;
    obj.insert("cmd","group_chat");
    obj.insert("user",userName);
    obj.insert("group",groupName);
    obj.insert("text",text);
    QByteArray ba = QJsonDocument(obj).toJson();
    tcpsocket->write(ba);

    QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss");
    ui->textEdit_2->clear();
    ui->textEdit->append(QString("<font color=\'green\'>%1 我发送:</b>\n").arg(current_date));
    ui->textEdit->append(QString("%1\n").arg(text));
}
