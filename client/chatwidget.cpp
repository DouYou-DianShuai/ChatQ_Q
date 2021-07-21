#include "chatwidget.h"
#include "ui_chatwidget.h"

ChatWidget::ChatWidget(QTcpSocket *s,QString u,QString f,Q_Qchat *q, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatWidget)
{
    ui->setupUi(this);
    tcpsocket = s;
    userName = u;
    friendName = f;
    qqchat = q;   //这个互相包含的对象在QT中允许，但是需要在头文件声明前声明
    this->setWindowTitle(QString("%1的聊天窗口 正在同 %2 聊天").arg(userName,friendName));
    this->setWindowIcon(QIcon(":/new/prefix1/a_5"));

    //对方不在线时
    connect(qqchat,&Q_Qchat::signal_to_chatwidget_offline,[=](QString user_to,QString s){
        if(user_to == friendName)
        {
            ui->textEdit->append(s);
        }
    });

    //接到对方消息时
    connect(qqchat,&Q_Qchat::signal_to_chatwidget,[=](QJsonObject obj){
        if(obj.value("cmd").toString() == "private_chat")   //如果是私聊
        {
            if(obj.value("user_from").toString() == friendName)  //保证只有对应好友窗口显示消息
            {
                QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss");
                ui->textEdit->append(QString("<font color=\'blue\'>%1 %2发送:</b>\n").arg(current_date,
                                             obj.value("user_from").toString()));
                ui->textEdit->append(QString("%1\n").arg( obj.value("text").toString()));

            }
        }


    });
}

ChatWidget::~ChatWidget()
{
    delete ui;
}

//发送消息按钮
void ChatWidget::on_pushButton_clicked()
{
    QString text = ui->textEdit_2->toPlainText();
    QJsonObject obj;
    obj.insert("cmd","private_chat");
    obj.insert("user_from",userName);
    obj.insert("user_to",friendName);
    obj.insert("text",text);
    QByteArray ba = QJsonDocument(obj).toJson();
    tcpsocket->write(ba);

    QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss");
    ui->textEdit_2->clear();
    ui->textEdit->append(QString("<font color=\'green\'>%1 我发送:</b>\n").arg(current_date));
    ui->textEdit->append(QString("%1\n").arg(text));




}

//退出聊天按钮
void ChatWidget::on_pushButton_2_clicked()
{
    this->hide();
}

//发送文件按钮槽函数
void ChatWidget::on_pushButton_3_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,"选择要发送的文件",QCoreApplication::applicationFilePath());
    if(fileName.isEmpty())
    {
        QMessageBox::warning(this,"发送文件提示","请选择一个文件");
    }
    else
    {
        QFile file(fileName);
        file.open(QIODevice::ReadOnly);
        QJsonObject obj;
        obj.insert("cmd","send_file");
        obj.insert("from_user",userName);
        obj.insert("to_user",friendName);
        obj.insert("length",file.size());
        obj.insert("filename",fileName);
        QByteArray ba = QJsonDocument(obj).toJson();
        tcpsocket->write(ba);
    }
}
