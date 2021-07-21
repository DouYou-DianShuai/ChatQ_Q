#include "register.h"
#include "ui_register.h"

Register::Register(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Register)
{
    ui->setupUi(this);

    //设置密码文本框，只能输入数字和英文,使用正则表达式 (没有对账号文本框作设置，因为为了简单，账号直接使用的用户名。
    QRegExpValidator *pRevalidotor = new QRegExpValidator(QRegExp("[a-zA-Z0-9]{25}"), this);
    ui->passwdLineEdit->setValidator(pRevalidotor);

    this->setWindowTitle("注册窗口");
}

Register::~Register()
{
    delete ui;
}

//注册确认按钮槽函数
void Register::on_loginButton_clicked()
{
    QString user = ui->userLineEdit->text();
    QString password = ui->passwdLineEdit->text();
    ui->userLineEdit->clear();
    ui->passwdLineEdit->clear();
    this->hide();

    emit mySignal(user,password);

}

//重写关闭事件，为了发送信号，打开登录窗口的文本框输入
void Register::closeEvent(QCloseEvent *event)
{
        QMessageBox::StandardButton button;

        button=QMessageBox::question(this,tr("退出注册窗口"),QString(tr("您还没有注册，确认退出注册窗口吗?")),QMessageBox::Yes|QMessageBox::No);

        if(button==QMessageBox::No)

        {

            event->ignore(); // 忽略退出信号，程序继续进行

        }

        else if(button==QMessageBox::Yes)
        {
            emit trueSignal();
            event->accept(); // 接受退出信号，程序退出

        }
}
