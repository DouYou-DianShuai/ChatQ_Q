#ifndef SERVERQ_Q_H
#define SERVERQ_Q_H

#include <event.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <iostream>
#include <event2/listener.h>
#include "listQ_Q.h"
#include <jsoncpp/json/json.h>
#include <unistd.h>
#include "threadpool.h"

using namespace std;

#define IP     "192.168.0.112"
#define PORT  8888 
#define MAXSIZE  1024 * 1024

struct Arg_pool{
	int length;
	int port;
	int *f_fd;
	int *t_fd;
};

class ServerQ_Q
{
private:

	struct event_base *base;          //事件集合
	struct evconnlistener *listener;    //监听事件
	static ChatInfo *chatlist;                //链表对象（含有两个链表）
	static ChatDataBase *chatdb;              //数据库对象   
    static ThreadPool_t  *pool;            //线程池对象

private:
	static void listener_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int socklen, void *arg);
	static void client_handler(int);
	static void send_file_handler(void *);
	static void read_cb(struct bufferevent *bev, void *ctx);
	static void event_cb(struct bufferevent *bev, short what, void *ctx);

	static void server_register(struct bufferevent *bev, Json::Value val);
	static void server_login(struct bufferevent *bev, Json::Value val);
	static void server_add_friend(struct bufferevent *bev, Json::Value val);
	static void server_create_group(struct bufferevent *bev, Json::Value val);
	static void server_add_group(struct bufferevent *bev, Json::Value val);
	static void server_private_chat(struct bufferevent *bev, Json::Value val);
	static void server_group_chat(struct bufferevent *bev, Json::Value val);
	static void server_get_group_member(struct bufferevent *bev, Json::Value val);
	static void server_user_offline(struct bufferevent *bev, Json::Value val);
	static void server_send_file(struct bufferevent *bev, Json::Value val);

public:
	ServerQ_Q(const char *ip = "127.0.0.1", int port = 8000);
	~ServerQ_Q();
};

#endif
