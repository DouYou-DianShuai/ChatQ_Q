#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    //设置密码文本框，只能输入数字和英文,使用正则表达式 (没有对账号文本框作设置，因为为了简单，账号直接使用的用户名。
    QRegExpValidator *pRevalidotor = new QRegExpValidator(QRegExp("[a-zA-Z0-9]{25}"), this);
    ui->passwdLineEdit->setValidator(pRevalidotor);

    //如果服务器不在线，本客户端无法正常使用，这些功能将在服务器在线时才可用
    ui->passwdLineEdit->setEnabled(false);
    ui->userLineEdit->setEnabled(false);
    ui->registerButton->setEnabled(false);
    ui->loginButton->setEnabled(false);

    //创建注册窗口，在点击注册按钮时才会显示。 (对方点击注册窗口的注册按钮会触发自定义信号mySignal,对方关闭注册窗口会触发自定义信号trueSignal）
    w_register = new Register;

    c = NULL;

    qDebug()<<"aaa";
    //信号mySignal
//    void (Register::*signalText)(QString , QString) = &Register::mySignal;  //如果信号发生重载，那么需要这样强制转换后使用新定义的signalText
    connect(w_register,&Register::mySignal,[=](QString user,QString password){
    //只要注册窗口退出，即可继续输入登录信息
    ui->userLineEdit->setEnabled(true);
    ui->passwdLineEdit->setEnabled(true);
      qDebug()<<user;
      qDebug()<<password;
    QJsonObject obj;
    obj.insert("cmd","register");
    obj.insert("user",user);
    obj.insert("password",password);
    QByteArray ba = QJsonDocument(obj).toJson();
        tcpSocket->write(ba);
    });
    //信号trueSignal
    connect(w_register,&Register::trueSignal,[=](){
        ui->userLineEdit->setEnabled(true);
        ui->passwdLineEdit->setEnabled(true);
    });



    //Tcpclient (连接成功会触发connected信号，来消息会触发readyRead信号)
    tcpSocket = new QTcpSocket(this);
    tcpSocket->connectToHost(QHostAddress("192.168.0.112"),8888);
    connect(tcpSocket,&QTcpSocket::connected,[=](){

        ui->passwdLineEdit->setEnabled(true);
        ui->userLineEdit->setEnabled(true);
        ui->registerButton->setEnabled(true);
        ui->loginButton->setEnabled(true);
        this->setWindowTitle("Q_Q      环境没有问题，可进行登录或注册");
//        QMessageBox::information(this,"连接提示","成功与服务器建立连接,可以正常登录，或者注册");
    });
    connect(tcpSocket,&QTcpSocket::readyRead,[=](){

        QByteArray ba = tcpSocket->readAll();
        QJsonObject obj = QJsonDocument::fromJson(ba).object();
        QString cmd = obj.value("cmd").toString();
        if(cmd == "register_reply")     //注册后调用的if处理函数
        {
            client_register_handler(obj.value("result").toString());
        }
        else if(cmd == "login_reply")    //登录后，调用的if处理函数
        {
            client_login_handler(obj.value("result").toString(),
                                 obj.value("friend").toString(),
                                 obj.value("group").toString());
        }
    });

}

Widget::~Widget()
{
    delete ui;
      qDebug()<<"bbb";
}

//从服务器中返回注册状态后，在槽函数中调用了此函数，来给使用者反馈注册成功与否。
void Widget::client_register_handler(QString res)
{
    if(res == "success")
    {
        QMessageBox::information(this,"注册提示","注册成功");
    }
    else if(res == "failure")
    {
        QMessageBox::information(this,"注册提示","注册失败");
    }
}

//从服务器返回登录状态后，在槽函数中调用了此函数，来给使用者反馈登录成功与否。
void Widget::client_login_handler(QString res , QString fri , QString group)
{
    if(res == "user_not_exist")
    {
        QMessageBox::warning(this,"登录提示","用户不存在");
    }
    else if(res == "password_error")
    {
        QMessageBox::warning(this,"登录提示","密码错误");
    }
    else if(res == "user_lineing")
    {
        QMessageBox::warning(this,"登录提示","该用户已经登录，不可重复登录");
    }
    else if(res == "success")
    {
        this->hide();
        tcpSocket->disconnect(SIGNAL(readyRead()));  //断开此信号与槽的连接
        c = new Q_Qchat(tcpSocket,fri,group,userName);  //将tcpSocket给到新窗口，由新窗口继续管理readyRead信号
        c->show();

            //用户返回登录界面
            connect(c,&Q_Qchat::mySignal,[=](){
                    delete c;
                    c = NULL;
                    ui->passwdLineEdit->clear();
                    ui->userLineEdit->clear();
                    this->show();
                    connect(tcpSocket,&QTcpSocket::readyRead,[=](){

                        QByteArray ba = tcpSocket->readAll();
                        QJsonObject obj = QJsonDocument::fromJson(ba).object();
                        QString cmd = obj.value("cmd").toString();
                        if(cmd == "register_reply")     //注册后调用的if处理函数
                        {
                            client_register_handler(obj.value("result").toString());
                        }
                        else if(cmd == "login_reply")    //登录后，调用的if处理函数
                        {
                            client_login_handler(obj.value("result").toString(),
                                                 obj.value("friend").toString(),
                                                 obj.value("group").toString());
                        }
                    });
            });


    }

}

//注册按钮的槽函数
void Widget::on_registerButton_clicked()
{
    //显示注册窗口进行注册
    w_register->show();

    //使登录窗口无法登录，知道注册完毕后恢复登录
    ui->userLineEdit->setEnabled(false);
    ui->passwdLineEdit->setEnabled(false);
}


//登录按钮槽函数
void Widget::on_loginButton_clicked()
{
    QString user = ui->userLineEdit->text();
    QString password = ui->passwdLineEdit->text();

    QJsonObject obj;
    obj.insert("cmd","login");
    obj.insert("user",user);
    obj.insert("password",password);
    QByteArray ba = QJsonDocument(obj).toJson();
    tcpSocket->write(ba);
    userName = user;
}

//关闭事件，为了使Q_Q窗口关闭时，同时关闭注册窗口。（注意：无法在析构函数中关闭，具体愿意见笔记）
void Widget::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton button;

        button=QMessageBox::question(this,tr("退出Q_Q"),QString(tr("确认退出Q_Q程序")),QMessageBox::Yes|QMessageBox::No);

        if(button==QMessageBox::No)

        {

            event->ignore(); // 忽略退出信号，程序继续进行

        }

        else if(button==QMessageBox::Yes)

        {
//            w_register->close();
            delete w_register;
            event->accept(); // 接受退出信号，程序退出

        }

}

void Widget::keyPressEvent(QKeyEvent *event)
{
     if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
     {
         ui->loginButton->click();
     }
}
