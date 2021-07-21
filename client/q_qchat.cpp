#include "q_qchat.h"
#include "ui_q_qchat.h"

Q_Qchat::Q_Qchat(QTcpSocket *s,QString fri,QString group,QString u,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Q_Qchat)
{
    ui->setupUi(this);

    tcpsocket = s;
    userName = u;

    this->setWindowTitle(QString("%1的Q_Q-聊天室").arg(userName));
    this->setWindowIcon(QIcon(":/new/prefix1/a_5"));
    addFriendWidget = new Addfriend(tcpsocket,userName);
    createGroupWidget = new Creategroup(tcpsocket,userName);
    addGroupWidget = new Addgroup(tcpsocket,userName);


    chatwidget = new std::list<CWInfo *>;
    groupWidtetlist = new QList<GcwInfo*>;

    ui->friendList->setIconSize(QSize(26,26));
    ui->groupList->setIconSize(QSize(26,26));

    //将tcp连接的读信号使用此窗口接收
    connect(tcpsocket,&QTcpSocket::readyRead,[=](){

        QByteArray ba = tcpsocket->readAll();
        QJsonObject obj = QJsonDocument::fromJson(ba).object();
        QString cmd = obj.value("cmd").toString();
        if(cmd == "friend_login")    //好友上线功能
        {
            client_login_reply(obj.value("friend").toString());
        }
        else if(cmd == "add_reply")  //添加好友功能(添加回复在这个if处理函数中处理）
        {
            client_add_friend_reply(obj);
        }
        else if(cmd == "add_friend_reply") //被别人好友添加成功,会在这里处理
        {
            QString str = QString("%1把你添加成好友，现在你们已经是好友了").arg(obj.value("result").toString());
            QMessageBox::information(this,"添加好友提醒",str);
//            ui->friendList->addItem(obj.value("result").toString());
            QListWidgetItem * pItem =new QListWidgetItem(QIcon("://a_7"),obj.value("result").toString()) ;
            pItem->setSizeHint(QSize(60,36));
            ui->friendList->addItem(pItem);
        }
        else if(cmd == "create_group_reply")  //创建群聊功能
        {
            client_create_group_reply(obj);
        }
        else if(cmd == "add_group_reply")   //添加群聊功能
        {
            client_add_group_reply(obj);
        }
        else if(cmd == "private_chat_reply")   //好友间私聊，对方却不在线时
        {
            cLIent_private_chat_reply(obj.value("user_to").toString(),obj.value("result").toString());
        }
        else if(cmd == "private_chat")  //两人私聊
        {
            client_chat_reply(obj);
        }
        else if(cmd == "get_group_member_reply")  //获取群成员成功
        {
            client_get_group_member_reply(obj);
        }
        else if(cmd == "group_chat")     //群聊
        {
            client_group_chat_reply(obj);
        }
        else if(cmd == "friend_offline")  //好友下线通知
        {
            client_friend_offline(obj.value("friend").toString());
        }
        else if(cmd == "send_file_reply")  //发送文件，失败时收到服务端的回复
        {
            client_send_file_reply(obj);
        }
        else if(cmd == "send_file_port_reply")//发送文件请求成功后，if处理函数（新建一个线程一个连接来发文件）
        {
            client_send_file_port_reply(obj);
        }
        else if(cmd == "recv_file_port_reply") //有好友发文件过来，if处理函数 （新建一个线程一个连接来接收文件）
        {
            client_recv_file_port_reply(obj);
        }


    });

    //将好友按格式解包加入listwidget窗口。
    QStringList friList = fri.split('|');
    for(int i = 0; i < friList.size(); i++)
    {
        if(friList.at(i)!="")
        {
//            ui->friendList->addItem(friList.at(i));
            QListWidgetItem * pItem =new QListWidgetItem(QIcon("://a_7"), friList.at(i)) ;
            pItem->setSizeHint(QSize(60,36));
            ui->friendList->addItem(pItem);
        }
    }

    //将群名按格式解包加入listwidget窗口。
    QStringList groList = group.split('|');
    for(int i = 0; i < groList.size(); i++)
    {
        qDebug()<<groList.size();
        if(groList.at(i)!="")
        {
//            ui->groupList->addItem(groList.at(i));
            QListWidgetItem * pItem =new QListWidgetItem(QIcon("://a_9"), groList.at(i)) ;
            pItem->setSizeHint(QSize(60,36));
            ui->groupList->addItem(pItem);
        }
    }

    //双击好友槽函数
    connect(ui->friendList,&QListWidget::itemDoubleClicked,[=](){
        QString friendName = ui->friendList->currentItem()->text();
        int flag = 0;
        for(std::list<CWInfo *>::const_iterator it = chatwidget->begin(); it != chatwidget->end(); it++)
        {
                if((*it)->name == friendName)
                {
                    qDebug()<<"验证聊天窗口不会重复在堆区申请";
                    flag = 1;   //证明已经有这个聊天窗口了，后续代码无需执行
                    (*it)->w->show();
                    (*it)->w->showNormal();  //可以使最小话的窗口恢复
                    break;
                }
        }
        if(flag == 0)
        {
//            CWInfo *c = (CWInfo *)malloc(sizeof(CWInfo));
            CWInfo *c = new CWInfo;
            c->w = new ChatWidget(tcpsocket,userName,friendName,this);
            c->name = friendName;
            chatwidget->push_back(c);  //每次使用尾插法插入，方便最终遍历释放
            chatwidget->back()->w->show();  //每次插入后就显示出来，从窗口关闭的，由窗口自己做隐藏处理

        }
    });

    //双击群槽函数
    connect(ui->groupList,&QListWidget::itemDoubleClicked,[=](){
        QString groupName = ui->groupList->currentItem()->text();
        bool flag = 0;
        for(int i = 0; i < groupWidtetlist->size() ; i++)
        {
            if(groupWidtetlist->at(i)->name == groupName)
            {
                qDebug()<<"验证群聊只在堆区开辟了一次空间";
                flag = 1;
                groupWidtetlist->at(i)->w->show();
                groupWidtetlist->at(i)->w->showNormal();
                emit updata_group_member_list();
                break;
            }
        }
        if(flag == 0)
        {
             GroupChatWidget *g = new GroupChatWidget(tcpsocket,groupName,userName,this);
             g->setWindowTitle(QString("%1的 %2 群聊窗口").arg(userName,groupName));

             qDebug()<<"1";

             //Qt中的malloc 不好用，支持很差劲，尽量用new吧
            /* GcwInfo *gcwinfo = (GcwInfo *)malloc(sizeof(GcwInfo));
             qDebug()<<"2";
             qDebug()<<groupName;
             gcwinfo->name = groupName;
             qDebug()<<"3";
             gcwinfo->w = g;
             qDebug()<<"4";*/

             GcwInfo *gcwinfo = new GcwInfo;
             gcwinfo->w = g;
             gcwinfo->name = groupName;
             groupWidtetlist->push_back(gcwinfo);
             qDebug()<<"5";
             groupWidtetlist->back()->w->show();
             emit updata_group_member_list();
        }

    });
}

Q_Qchat::~Q_Qchat()
{
    delete ui;
}

//好友上线if处理函数
void Q_Qchat::client_login_reply(QString fri)
{
    QString str = QString("好友%1上线").arg(fri);
    QMessageBox::information(this,"好友上线提醒",str);
}

//添加好友if处理函数
void Q_Qchat::client_add_friend_reply(QJsonObject &obj)
{
    if(obj.value("result").toString() == "user_not_exist")
    {
        QMessageBox::warning(this,"添加好友提醒","用户名不存在，添加失败");
    }
    else if(obj.value("result").toString()== "already_friend")
    {
        QMessageBox::warning(this,"添加好友提醒","你们已经是好友关系，无需重复添加");
    }
    else if(obj.value("result").toString()== "success")
    {
        QMessageBox::warning(this,"添加好友提醒",QString("添加好友%1成功!!!").arg(obj.value("friend").toString()));
//        ui->friendList->addItem(obj.value("friend").toString());
        QListWidgetItem * pItem =new QListWidgetItem(QIcon("://a_7"),obj.value("friend").toString()) ;
        pItem->setSizeHint(QSize(60,36));
        ui->friendList->addItem(pItem);
    }

}

//创建群聊if处理函数
void Q_Qchat::client_create_group_reply(QJsonObject &obj)
{
    if(obj.value("result").toString() == "group_exist")
    {
        QMessageBox::warning(this,"创建群聊提示","群聊已存在,创建失败");
    }
    else if(obj.value("result").toString() == "success")
    {
        QMessageBox::warning(this,"创建群聊提示",QString("创建成功，群名称为:%1").arg(obj.value("group").toString()));
        QListWidgetItem * pItem =new QListWidgetItem(QIcon("://a_9"), obj.value("group").toString()) ;
        pItem->setSizeHint(QSize(60,36));
        ui->groupList->addItem(pItem);
    }
}

//加入群if处理函数
void Q_Qchat::client_add_group_reply(QJsonObject &obj)
{
    if(obj.value("result").toString() == "group_not_exist")
    {
        QMessageBox::warning(this,"添加群提示","群不存在，添加失败");
    }
    else if(obj.value("result").toString() == "user_in_group")
    {
        QMessageBox::warning(this,"添加群提示","你已经是群成员，无法重复添加");
    }
    else if(obj.value("result").toString() == "success")
    {
        QMessageBox::warning(this,"添加群提示",QString("添加成功，群名称为:%1").arg(obj.value("group").toString()));
//        ui->groupList->addItem(obj.value("group").toString());
        QListWidgetItem * pItem =new QListWidgetItem(QIcon("://a_9"), obj.value("group").toString()) ;
        pItem->setSizeHint(QSize(60,36));
        ui->groupList->addItem(pItem);
    }
}

//两人私聊，对方不在线if处理函数
void Q_Qchat::cLIent_private_chat_reply(QString user_to,QString res)
{
    if(res == "offline")
    {
//        QMessageBox::warning(this,"发送提醒","好友不在线,本版本暂不支持离线私信");
         emit signal_to_chatwidget_offline(user_to,
              "<font color=\'red\'>//////////对方不在线，消息发送失败（本版本暂不支持离线私信）///////////</b>\n");
    }
}

//两人私聊，双方都在线if处理函数,接收到对方消息后
void Q_Qchat::client_chat_reply(QJsonObject &obj)
{
    int flag = 0;
    for(std::list<CWInfo *>::const_iterator it = chatwidget->begin(); it != chatwidget->end(); it++)
    {
        if((*it)->name == obj.value("user_from").toString())
        {
            flag = 1;   //证明已经有这个聊天窗口了，后续代码无需执行
            (*it)->w->show();
            (*it)->w->showNormal();  //可以使最小话的窗口恢复
            break;
        }
    }
    if(flag == 0)  //聊天窗口没有打开过
    {
//        CWInfo *c = (CWInfo *)malloc(sizeof(CWInfo));
        CWInfo *c = new CWInfo;
        c->w = new ChatWidget(tcpsocket,userName,obj.value("user_from").toString(),this);
        c->name = obj.value("user_from").toString();
        chatwidget->push_back(c);  //每次使用尾插法插入，方便最终遍历释放
        chatwidget->back()->w->show();  //每次插入后就显示出来，从窗口关闭的，由窗口自己做隐藏处理

    }
    emit signal_to_chatwidget(obj);

}

//获取群成员信息后的if处理函数
void Q_Qchat::client_get_group_member_reply(QJsonObject &obj)
{
    emit signal_to_groupWidget_list(obj);
}

//接收到群消息的if处理函数
void Q_Qchat::client_group_chat_reply(QJsonObject &obj)
{
    bool flag = 0;
    for(int i = 0; i < groupWidtetlist->size() ; i++)
    {
        if(groupWidtetlist->at(i)->name == obj.value("group").toString())
        {
            qDebug()<<"验证群聊只在堆区开辟了一次空间";
            flag = 1;
            groupWidtetlist->at(i)->w->show();
            groupWidtetlist->at(i)->w->showNormal();
            emit updata_group_member_list();  //更新群成员信息
            break;
        }
    }
    if(flag == 0)
    {
         GroupChatWidget *g = new GroupChatWidget(tcpsocket,obj.value("group").toString(),userName,this);
         g->setWindowTitle(QString("%1的 %2 群聊窗口").arg(userName,obj.value("group").toString()));

         qDebug()<<"1";

         //Qt中的malloc 不好用，支持很差劲，尽量用new吧
        /* GcwInfo *gcwinfo = (GcwInfo *)malloc(sizeof(GcwInfo));
         qDebug()<<"2";
         qDebug()<<groupName;
         gcwinfo->name = groupName;
         qDebug()<<"3";
         gcwinfo->w = g;
         qDebug()<<"4";*/

         GcwInfo *gcwinfo = new GcwInfo;
         gcwinfo->w = g;
         gcwinfo->name = obj.value("group").toString();
         groupWidtetlist->push_back(gcwinfo);
         qDebug()<<"5";
         groupWidtetlist->back()->w->show();
         emit updata_group_member_list();  //更新群成员信息
    }

    emit signal_to_groupWidget_chat(obj);
}

//好友下线通知时的if处理函数
void Q_Qchat::client_friend_offline(QString fri)
{
    QString str = QString("好友%1下线").arg(fri);
    QMessageBox::information(this,"下线提醒",str);
}

//发送文件请求通过，和服务器建立连接的if处理函数（另起一个线程，重写和服务器建立一个文件传输用的连接，用于传输文件）
void Q_Qchat::client_send_file_port_reply(QJsonObject &obj)
{
    SendFileThread *mySendThread = new SendFileThread(obj);
    mySendThread->start();

    int aaa = obj.value("length").toInt();
    qDebug()<<aaa;
    QMessageBox::information(this,"文件传输成功提醒",
    QString("成功向好友%1发送了大小为%2字节的文件").arg(obj.value("user").toString()).arg(aaa));
}

//接收文件if处理函数
void Q_Qchat::client_recv_file_port_reply(QJsonObject &obj)
{
    RecvFileThread *myRecvThread = new RecvFileThread(obj);
    myRecvThread->start();

    int aaa = obj.value("length").toInt();
    qDebug()<<aaa;
    QMessageBox::information(this,"文件传输成功提醒",
       QString("成功接收到好友%1发送的大小为%2字节的文件").arg(obj.value("user").toString()).arg(aaa));
}

//发送文件请求失败时，接收到失败原因的if处理函数
void Q_Qchat::client_send_file_reply(QJsonObject &obj)
{
    if(obj.value("result").toString()=="offline")
    {
        QMessageBox::warning(this,"发送文件提醒","对方不在线,发送失败");
    }
    else if(obj.value("result").toString() == "timeout")
    {
        QMessageBox::warning(this,"发送文件提醒","连接超时，发送失败");
    }
}

//添加好友按钮槽函数
void Q_Qchat::on_addButton_clicked()
{
        addFriendWidget->show();
}

//创建群聊按钮槽函数
void Q_Qchat::on_createGroupButton_clicked()
{

        createGroupWidget->show();
}

//添加群按钮槽函数
void Q_Qchat::on_addgroupButton_clicked()
{
        addGroupWidget->show();
}

//退出并返回登录界面槽函数
void Q_Qchat::on_pushButton_clicked()
{
    this->close();
    tcpsocket->disconnect(SIGNAL(readyRead()));  //断开此信号与槽的连接
    emit mySignal();

}

//退出程序按钮槽函数（相当于关闭）
void Q_Qchat::on_pushButton_2_clicked()
{
    this->close();
}

//重写关闭事件
void Q_Qchat::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton button;

        button=QMessageBox::question(this,tr("退出Q_Q"),QString(tr("确认退出Q_Q程序")),QMessageBox::Yes|QMessageBox::No);

        if(button==QMessageBox::No)

        {

            event->ignore(); // 忽略退出信号，程序继续进行

        }

        else if(button==QMessageBox::Yes)
        {
            //通知服务器，本用户要下线了，让服务器更新内存的时实在线数据
            QJsonObject obj;
            obj.insert("cmd","offline");
            obj.insert("user",userName);
            QByteArray ba = QJsonDocument(obj).toJson();
            tcpsocket->write(ba);
            tcpsocket->flush();

            //释放所有添加或申请窗口
            delete addFriendWidget;
            delete createGroupWidget;
            delete addGroupWidget;

            //释放所有私聊窗口
            for(std::list<CWInfo *>::const_iterator it = chatwidget->begin(); it != chatwidget->end(); it++)
            {
                delete (*it)->w;
//                free(*it);
                delete (*it);
            }
            delete chatwidget;

            //释放所有群聊窗口
            QList<GcwInfo*>::const_iterator it = groupWidtetlist->begin();
            int i = 0;
            for(i = 0; i < groupWidtetlist->size() ; i++)
            {
                delete groupWidtetlist->at(i)->w;
                delete (*it+i);
            }
//            for(i=i ; i >0 ; i--)
//            {
//                delete groupWidtetlist->at(i-1);
//            }

            delete groupWidtetlist;


            event->accept(); // 接受退出信号，程序退出

         }

}
