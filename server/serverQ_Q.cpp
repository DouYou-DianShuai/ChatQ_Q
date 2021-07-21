#include "serverQ_Q.h"

//静态成员变量记得在类外定义后使用
ChatDataBase *ServerQ_Q::chatdb = new ChatDataBase;
ChatInfo *ServerQ_Q::chatlist = new ChatInfo;    
ThreadPool_t * ServerQ_Q::pool;
ServerQ_Q::ServerQ_Q(const char *ip, int port)
{
	//创建线程池对象
	pool = new ThreadPool_t;            //线程池对象
	threadPoolCreate(pool,8,20,100);

	//创建事件集合
	base = event_base_new();

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr(ip);

	//创建监听对象
	listener = evconnlistener_new_bind(base, listener_cb, base, 
		LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, 10, (struct sockaddr *)&server_addr, 
			sizeof(server_addr));
	if (NULL == listener)
	{
		cout << "evconnlistener_new_bind error /server-Q_Q.cpp 28/" << endl;
	}

	cout << "服务器启动完成" << endl;
	event_base_dispatch(base);       //监听集合
	    //释放对象
    evconnlistener_free(listener); 
    event_base_free(base);

}

//当有客户端发起连接的时候，会触发该函数
void ServerQ_Q::listener_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int socklen, void *arg)
{
	cout << "接受客户端的连接，fd = " << fd << endl;


    printf("客户端%d上线\n",fd);
    struct event_base *base = (struct event_base *)arg;


	struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	if (NULL == bev)
	{
		cout << "bufferevent_socket_new error /server-Q_Q.cpp 52/" << endl;
	}


	//给bufferevent设置回调函数
    bufferevent_setcb(bev,read_cb,NULL,event_cb, NULL);

    //使能回调函数
    bufferevent_enable(bev,EV_READ | EV_PERSIST);
}

void ServerQ_Q::read_cb(struct bufferevent *bev, void *ctx)
{
	char buf[1024] = {0};
	int size = bufferevent_read(bev, buf, sizeof(buf));
	if (size < 0)
	{
		cout << "bufferevent_read error /server-Q_Q.cpp 69/" << endl;
	}

	cout << buf << endl;

	Json::Reader reader;       //解析json对象
	Json::FastWriter writer;   //封装json对象
	Json::Value val;
	//解析和封装也可以直接这样用
		// Json::Reader().parse();
		// Json::FastWriter().write();

	if (!reader.parse(buf, val))    //把字符串转换成 json 对象
	{
		cout << "服务器解析数据失败" << endl;
	}

	string cmd = val["cmd"].asString();

	if (cmd == "register")   //注册功能
	{
		server_register(bev, val);	
	}
	else if (cmd == "login")
	{
		server_login(bev, val);
	}
	else if (cmd == "add")
	{
		server_add_friend(bev, val);
	}
	else if (cmd == "create_group")
	{
		server_create_group(bev, val);
	}
	else if (cmd == "add_group")
	{
		server_add_group(bev, val);
	}
	else if (cmd == "private_chat")
	{
		server_private_chat(bev, val);
	}
	else if (cmd == "group_chat")
	{
		server_group_chat(bev, val);
	}
	else if (cmd == "get_group_member")
	{
		server_get_group_member(bev, val);
	}
	else if (cmd == "offline")
	{
		server_user_offline(bev, val);
	}
	else if (cmd == "send_file")
	{
		server_send_file(bev, val);
	}
}

void ServerQ_Q::event_cb(struct bufferevent *bev, short what, void *ctx)
{
    if(what & BEV_EVENT_EOF)
    {
        bufferevent_free(bev);
    }
    else
    {
        printf("未知的错误 /server-Q_Q.cpp 138/\n");
    }
}

ServerQ_Q::~ServerQ_Q()
{
	event_base_free(base);
	threadPoolDestroy(pool);
}

void ServerQ_Q::server_register(struct bufferevent *bev, Json::Value val)
{
	chatdb->my_database_connect("userName");

	if (chatdb->my_database_user_exist(val["user"].asString()))   //用户存在
	{
		Json::Value val;
		val["cmd"] = "register_reply";
		val["result"] = "failure";

		string s = Json::FastWriter().write(val);
		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
		{
			cout << "bufferevent_write error /server-Q_Q.cpp 161/" << endl;
		}
	}
	else                                               //用户不存在
	{
		chatdb->my_database_user_password(val["user"].asString(), val["password"].asString());
		Json::Value val;
		val["cmd"] = "register_reply";
		val["result"] = "success";

		string s = Json::FastWriter().write(val);
		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
		{
			cout << "bufferevent_write error /server-Q_Q.cpp 174/" << endl;
		}
	}

	chatdb->my_database_disconnect();
}

//用户登陆（登陆成功后，取出其好友列表，给好友列表好友发送上线提醒）
void ServerQ_Q::server_login(struct bufferevent *bev, Json::Value val)
{
	chatdb->my_database_connect("userName");
	if (!chatdb->my_database_user_exist(val["user"].asString()))   //用户不存在
	{
		Json::Value val;
		val["cmd"] = "login_reply";
		val["result"] = "user_not_exist";

		string s = Json::FastWriter().write(val);
		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
		{
			cout << "bufferevent_write error /server-Q_Q.cpp 194/" << endl;
		}
		return;
	}

	//判断用户是否已经在线
	struct bufferevent *to_bev = chatlist->info_get_friend_bev(val["user"].asString());
	if (NULL != to_bev)
	{
		Json::Value v;
		v["cmd"] = "login_reply";
		v["result"] = "user_lineing";

		string s = Json::FastWriter().write(v);
		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
		{
			cout << "bufferevent_write error /server-Q_Q.cpp 210/" << endl;
		}
		return;
	}


	//判断密码是否正确
	if (!chatdb->my_database_password_correct(val["user"].asString(), 
						val["password"].asString()))    //密码不正确
	{
		Json::Value val;
		val["cmd"] = "login_reply";
		val["result"] = "password_error";

		string s = Json::FastWriter().write(val);
		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
		{
			cout << "bufferevent_write error /server-Q_Q.cpp 227/" << endl;
		}
		return;
	}

	//从数据库中取出好友列表，放入内存（链表中）。
	Json::Value v;
	string s, name;

	//向链表中添加用户
	User u = {val["user"].asString(), bev};
	chatlist->online_user->push_back(u);

	//获取好友列表并且通过传出参数返回
	string friend_list, group_list;
	chatdb->my_database_get_friend_group(val["user"].asString(), friend_list, group_list);

	v["cmd"] = "login_reply";
	v["result"] = "success";
	v["friend"] = friend_list;
	v["group"] = group_list;
	s = Json::FastWriter().write(v);
	if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
	{
		cout << "bufferevent_write error /server-Q_Q.cpp 251/" << endl;
	}

	//向好友发送上线提醒
	int start = 0, end = 0, flag = 1;
	while (flag) 
    {   
    	end = friend_list.find('|', start);
        if (-1 == end)
        {   
			name = friend_list.substr(start, friend_list.size() - start);
        	flag = 0;
        }
		else
		{
			name = friend_list.substr(start, end - start);
		}

		for (list<User>::iterator it = chatlist->online_user->begin(); 
			it != chatlist->online_user->end(); it++)
		{
			if (name == it->name)
			{
				v.clear();
				v["cmd"] = "friend_login";
				v["friend"] = val["user"];
				s = Json::FastWriter().write(v);
				if (bufferevent_write(it->bev, s.c_str(), strlen(s.c_str())) < 0)
				{
					cout << "bufferevent_write error /server-Q-Q.cpp 280/" << endl;
				}
			}
		}
        start = end + 1;
    }   
	
	chatdb->my_database_disconnect();
}

//添加好友，操作数据库判断好友是否存在于数据库用户。
void  ServerQ_Q::server_add_friend(struct bufferevent *bev, Json::Value val)
{
	Json::Value v;
	string s;

	chatdb->my_database_connect("userName");

	if (!chatdb->my_database_user_exist(val["friend"].asString()))   //添加的好友不存在
	{
		v["cmd"] = "add_reply";
		v["result"] = "user_not_exist";

		s = Json::FastWriter().write(v);
		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
		{
			cout << "bufferevent_write error /server-Q_Q.cpp 306/" << endl;
		}
		return;
	}

	//如果上步验证通过，则验证两人是否已经为好友关系。(如果不是，则添加好友--修改双方数据库信息)
	if (chatdb->my_database_is_friend(val["user"].asString(), val["friend"].asString()))
	{
		v.clear();
		v["cmd"] = "add_reply";
		v["result"] = "already_friend";

		s = Json::FastWriter().write(v);
		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
		{
			cout << "bufferevent_write error /server-Q_Q.cpp 321/" << endl;
		}
		return;
	}
	
	//修改双方的数据库
	chatdb->my_database_add_new_friend(val["user"].asString(), val["friend"].asString());
	chatdb->my_database_add_new_friend(val["friend"].asString(), val["user"].asString());

	v.clear();
	v["cmd"] = "add_reply";
	v["result"] = "success";
	v["friend"] = val["friend"];

	s = Json::FastWriter().write(v);
	if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
	{
		cout << "bufferevent_write error /server-Q_Q.cpp 338/" << endl;
	}

	for (list<User>::iterator it = chatlist->online_user->begin(); 
					it != chatlist->online_user->end(); it++)
	{
		if (val["friend"] == it->name)
		{
			v.clear();
			v["cmd"] = "add_friend_reply";
			v["result"] = val["user"];

			s = Json::FastWriter().write(v);
			if (bufferevent_write(it->bev, s.c_str(), strlen(s.c_str())) < 0)
			{
				cout << "bufferevent_write error /server-Q_Q.cpp 353/" << endl;
			}
		}
	}

	chatdb->my_database_disconnect();
}

void ServerQ_Q::server_create_group(struct bufferevent *bev, Json::Value val)
{
	chatdb->my_database_connect("groupChatName");

	//判断群是否存在
	if (chatdb->my_database_group_exist(val["group"].asString()))
	{
		Json::Value v;
		v["cmd"] = "create_group_reply";
		v["result"] = "group_exist";

		string s = Json::FastWriter().write(v);
		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
		{
			cout << "bufferevent_write error /server-Q_Q.cpp 375/" << endl;
		}
		return;
	}

	//把群信息写入数据库
	chatdb->my_database_add_new_group(val["group"].asString(), val["user"].asString());
	chatdb->my_database_disconnect();

	chatdb->my_database_connect("userName");
	//修改数据库个人信息
	chatdb->my_database_user_add_group(val["user"].asString(), val["group"].asString());
	//修改群链表
	chatlist->info_add_new_group(val["group"].asString(), val["user"].asString());

	Json::Value value;
	value["cmd"] = "create_group_reply";
	value["result"] = "success";
	value["group"] = val["group"];

	string s = Json::FastWriter().write(value);
	if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
	{
		cout << "bufferevent_write error /server-Q_Q.cpp 398/" << endl;
	}
}

void ServerQ_Q::server_add_group(struct bufferevent *bev, Json::Value val)
{
	//判断群是否存在
	if (!chatlist->info_group_exist(val["group"].asString()))
	{
		Json::Value v;
		v["cmd"] = "add_group_reply";
		v["result"] = "group_not_exist";

		string s = Json::FastWriter().write(v);
		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
		{
			cout << "bufferevent_write error /server-Q_Q.cpp 414/" << endl;
		}
		return;
	}

	//判断用户是否在群里
	if (chatlist->info_user_in_group(val["group"].asString(), val["user"].asString()))
	{
		Json::Value v;
		v["cmd"] = "add_group_reply";
		v["result"] = "user_in_group";

		string s = Json::FastWriter().write(v);
		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
		{
			cout << "bufferevent_write error /server-Q_Q.cpp 429/" << endl;
		}
		return;
	}

	//修改数据库（用户表 群表）
	chatdb->my_database_connect("userName");
	chatdb->my_database_user_add_group(val["user"].asString(), val["group"].asString());
	chatdb->my_database_disconnect();

	chatdb->my_database_connect("groupChatName");
	chatdb->my_database_group_add_user(val["group"].asString(), val["user"].asString());
	chatdb->my_database_disconnect();

	//修改链表
	chatlist->info_group_add_user(val["group"].asString(), val["user"].asString());

	Json::Value v;
	v["cmd"] = "add_group_reply";
	v["result"] = "success";
	v["group"] = val["group"];

	string s = Json::FastWriter().write(v);
	if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
	{
		cout << "bufferevent_write error /server-Q_Q.cpp 454/" << endl;
	}
}

void ServerQ_Q::server_private_chat(struct bufferevent *bev, Json::Value val)
{
	struct bufferevent *to_bev = chatlist->info_get_friend_bev(val["user_to"].asString());
	if (NULL == to_bev)
	{
		Json::Value v;
		v["cmd"] = "private_chat_reply";
		v["result"] = "offline";
		v["user_to"] = val["user_to"].asString();

		string s = Json::FastWriter().write(v);
		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
		{
			cout << "bufferevent_write error /server-Q_Q.cpp 471/" << endl;
		}
		return;
	}

	string s = Json::FastWriter().write(val);
	if (bufferevent_write(to_bev, s.c_str(), strlen(s.c_str())) < 0)
	{
		cout << "bufferevent_write error /server-Q_Q.cpp 479/" << endl;
	}

	Json::Value v;
	v["cmd"] = "private_chat_reply";
	v["result"] = "success";

	s = Json::FastWriter().write(v);
	if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
	{
		cout << "bufferevent_write error /server-Q_Q.cpp 489/" << endl;
	}
}

void ServerQ_Q::server_group_chat(struct bufferevent *bev, Json::Value val)
{
	for (list<Group>::iterator it = chatlist->group_info->begin(); it != chatlist->group_info->end(); it++)
	{
		if (val["group"].asString() == it->name)
		{
			for (list<GroupUser>::iterator i = it->l->begin(); i != it->l->end(); i++)
			{
				struct bufferevent *to_bev = chatlist->info_get_friend_bev(i->name);
				if (to_bev != NULL)
				{
					string s = Json::FastWriter().write(val);
					if (bufferevent_write(to_bev, s.c_str(), strlen(s.c_str())) < 0)
					{
						cout << "bufferevent_write error /server-Q_Q.cpp 507/" << endl;
					}
				}
			}
		}
	}

	Json::Value v;
	v["cmd"] = "group_chat_reply";
	v["result"] = "success";

	string s = Json::FastWriter().write(v);
	if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
	{
		cout << "bufferevent_write error /server-Q_Q.cpp 521/" << endl;
	}
}

void ServerQ_Q::server_get_group_member(struct bufferevent *bev, Json::Value val)
{
	string member = chatlist->info_get_group_member(val["group"].asString());

	Json::Value v;
	v["cmd"] = "get_group_member_reply";
	v["member"] = member;
	v["group"] = val["group"];

	string s = Json::FastWriter().write(v);
	if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
	{
		cout << "bufferevent_write error /server-Q_Q.cpp 537/" << endl;
	}
	
}

void ServerQ_Q::server_user_offline(struct bufferevent *bev, Json::Value val)
{
	//从链表中删除用户
	for (list<User>::iterator it = chatlist->online_user->begin();
		it != chatlist->online_user->end(); it++)
	{
		if (it->name == val["user"].asString())
		{
			chatlist->online_user->erase(it);
			break;
		}
	}

	chatdb->my_database_connect("userName");

	//获取好友列表并且返回
	string friend_list, group_list;
	string name, s;
	Json::Value v;

	chatdb->my_database_get_friend_group(val["user"].asString(), friend_list, group_list);

	//向好友发送下线提醒
	int start = 0, end = 0, flag = 1;
	while (flag) 
    {   
    	end = friend_list.find('|', start);
        if (-1 == end)
        {
			name = friend_list.substr(start, friend_list.size() - start);
        	flag = 0;
        }
		else 
		{
			name = friend_list.substr(start, end - start);
		}

		for (list<User>::iterator it = chatlist->online_user->begin(); 
			it != chatlist->online_user->end(); it++)
		{
			if (name == it->name)
			{
				v.clear();
				v["cmd"] = "friend_offline";
				v["friend"] = val["user"];
				s = Json::FastWriter().write(v);
				if (bufferevent_write(it->bev, s.c_str(), strlen(s.c_str())) < 0)
				{
					cout << "bufferevent_write" << endl;
				}
			}
		}
        start = end + 1;
    }   
	
	chatdb->my_database_disconnect();
}


// void ServerQ_Q::send_file_handler(int length, int port, int *f_fd, int *t_fd)
void ServerQ_Q::send_file_handler(void *arg)
{
	int length = (*(struct Arg_pool *)arg).length;
	int port = (*(struct Arg_pool *)arg).port;
	int * f_fd = (*(struct Arg_pool *)arg).f_fd;
	int * t_fd= (*(struct Arg_pool *)arg).t_fd;

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sockfd)
	{
		return;
	}

	int opt = 1;
	setsockopt(sockfd,SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	// 接收缓冲区
	int nRecvBuf = MAXSIZE;           
	setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBuf, sizeof(int));
	//发送缓冲区
	int nSendBuf = MAXSIZE;    //设置为1M
	setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (const char*)&nSendBuf, sizeof(int));

	struct sockaddr_in server_addr, client_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr(IP);
	bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	listen(sockfd, 10);


	int len = sizeof(client_addr);
	//接受发送客户端的连接请求
	*f_fd = accept(sockfd, (struct sockaddr *)&client_addr, (socklen_t *)&len);
	//接受接收客户端的连接请求
	*t_fd = accept(sockfd, (struct sockaddr *)&client_addr, (socklen_t *)&len);

	cout<<length<<endl;
	cout<<"////////"<<endl;
	char buf[MAXSIZE] = {0};
	size_t  sum = 0;
	int64_t size=0;
	while (1)
	{
		size = recv(*f_fd, buf, MAXSIZE, 0);
		if (size <= 0 )
		{
			cout<<"size"<<size<<endl;
			break;
		}
		else if (size > MAXSIZE)
		{
			cout<<"我错了整了个无符号整形size="<<size<<endl;
			cout<<"哈哈哈哈和"<<endl;
			break;
		}

		sum += size;
		send(*t_fd, buf, size, 0);
		if (sum >= length)
		{
			break;
		}
		memset(buf, 0, MAXSIZE);
	}

	close(*f_fd);
	close(*t_fd);
	close(sockfd);

}

void ServerQ_Q::server_send_file(struct bufferevent *bev, Json::Value val)
{
	Json::Value v;
	string s;

	//判断对方是否在线
	struct bufferevent *to_bev = chatlist->info_get_friend_bev(val["to_user"].asString());
	if (NULL == to_bev)
	{
		v["cmd"] = "send_file_reply";
		v["result"] = "offline";
		s = Json::FastWriter().write(v);
		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
		{
			cout << "bufferevent_write error /server-Q_Q.cpp 689/" << endl;
		}
		return;
	}
	
	//启动新线程，创建文件服务器
	int port = 8080, from_fd = 0, to_fd = 0;

	//创建任务变量结构体
	struct Arg_pool *arg_pool = new Arg_pool;
	arg_pool->length = val["length"].asInt();
	arg_pool->port = port;
	arg_pool->f_fd = &from_fd;
	arg_pool->t_fd = &to_fd;
	threadPoolAdd(pool,send_file_handler,arg_pool);

	v.clear();
	v["cmd"] = "send_file_port_reply";
	v["port"] = port;
	v["filename"] = val["filename"];
	v["length"] = val["length"];
	v["user"] = val["to_user"];
	s = Json::FastWriter().write(v);
	if (send(bev->ev_read.ev_fd, s.c_str(), strlen(s.c_str()), 0) < 0)
	{
		cout << "bufferevent_write error /serever-Q_Q.cpp 714/" << endl;
	}

	int count = 0;
	while (from_fd <= 0)
	{
		count++;
		usleep(100000);
		if (count == 100)
		{
			cout<<"1超时"<<endl;
			v.clear();
			v["cmd"] = "send_file_reply";
			v["result"] = "timeout";
			s = Json::FastWriter().write(v);
			if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
			{
				cout << "bufferevent_write error /server-Q_Q.cpp 732/" << endl;
			}
			return;
		}
	}
	sleep(0.01);

	//返回端口号给接收客户端
	v.clear();
	v["cmd"] = "recv_file_port_reply";
	v["port"] = port;
	v["filename"] = val["filename"];
	v["length"] = val["length"];
	v["user"] = val["from_user"];
	s = Json::FastWriter().write(v);
	if (send(to_bev->ev_read.ev_fd, s.c_str(), strlen(s.c_str()), 0) < 0)
	{
		cout << "bufferevent_write error /server-Q_Q.cpp 750/" << endl;
	}

	count = 0;
	while (to_fd <= 0)
	{
		count++;
		usleep(100000);
		if (count == 100)
		{
			cout<<"1超时"<<endl;
			cout<<"1超时"<<endl;
			v.clear();
			v["cmd"] = "send_file_reply";
			v["result"] = "timeout";
			s = Json::FastWriter().write(v);
			if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
			{
				cout << "bufferevent_write error /server-Q_Q.cpp 769/" << endl;
			}
		}
	}
}
