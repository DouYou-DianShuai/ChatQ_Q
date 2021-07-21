#ifndef LISTQ_Q_H
#define LISTQ_Q_H

#include <event.h>
#include <list>
#include "databaseQ_Q.h"

using namespace std;

#define MAXNUM    1024     //群最大个数

struct User
{
	string name;
	struct bufferevent *bev;
};
typedef struct User User;

struct GroupUser
{
	string name;
};
typedef struct GroupUser GroupUser;

struct Group
{
	string name;
	list<GroupUser> *l;
};
typedef struct Group Group;

class ServerQ_Q;

class ChatInfo
{
	friend class ServerQ_Q;
private:
	list<User> *online_user;     //保存所有在线的用户信息
	list<Group> *group_info;     //保存所有群信息
	ChatDataBase *mydatabase;    //数据库对象

public:
	ChatInfo();
	~ChatInfo();

	bool info_group_exist(string);
	bool info_user_in_group(string, string);
	void info_group_add_user(string, string);
	struct bufferevent *info_get_friend_bev(string);
	string info_get_group_member(string);
	void info_add_new_group(string, string);
};

#endif
