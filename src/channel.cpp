#include "channel.h"
#include "client.hpp"
#include "string.h"

Chanel::Chanel(int admin, std::string chanel_name , std::string topic)
{
    this->admin_fd = admin;
	this->chanel_global_name = chanel_name;
    this->topic = topic;
	// set default value of struct password
	this->password_info.active = false;
	this->password_info.password = "";
	// set default value of struct permission
	this->permision_info.invite_only = false;
	this->permision_info.restrictive_topic = false;
	this->permision_info.max_user = -1;
	// max user to -1
	this->permision_info.max_user = -1;
}

Chanel::~Chanel(){}

void		Chanel::Broadcast_message(std::string msg, std::map<int, Client> &server_users, int fd)
{
	if (Check_UserOnServer(server_users, admin_fd))
	{
		std::string new_msg = admin_fd == fd ?  msg : ("\n" + msg + getTimestamp() + " @" \
		+ server_users[admin_fd].username +  "(" + server_users[admin_fd].nickname + ") :");
		send(admin_fd, new_msg.c_str(), new_msg.length(), 0);
	}
	for (std::map<int , Client >::iterator it = users.begin(); it != users.end(); it ++)
	{
		if(Check_UserOnServer(server_users, it->first))
		{
			std::string all_msg = (it->first == fd)  ?  msg : ("\n" + msg + getTimestamp() + " @" \
			+ server_users[it->first].username + "(" + server_users[it->first].nickname + ") :");
			send(it->first, all_msg.c_str(), all_msg.length(), 0);
		}
	}
}

std::string	convert_fd_to_name(int fd, std::map<int, Client> &server_users)
{
	std::map<int, Client>::iterator it;
	for (it = server_users.begin() ; it != server_users.end() ; it ++)
	{
		if (it->first == fd)
			return (it->second.username);
	}
	return (std::string(""));
}

int			convert_name_to_fd(std::string name, std::map<int, Client> &server_users)
{
	std::map<int, Client>::iterator it;
	for (it = server_users.begin() ; it != server_users.end() ; it ++)
	{
		if (it->second.username == name)
			return (it->first);
	}
	return (0);
}

int			Check_Existng_Chanel(std::string &name , std::map<std::string, Chanel> &chanels)
{
	if (chanels.find(name) != chanels.end())
		return (1);
	return (0);
}

int			Chanel::Check_UserOnChanel(int fd_user)
{
	std::map<int, Client>::iterator it = users.find(fd_user);
	if (it != users.end() || fd_user == admin_fd)
		return (1);
	return (0);
}

int			Check_UserOnServer(std::map<int, Client> &server_users , int fd_user)
{
	std::map<int, Client>::iterator it = server_users.find(fd_user);
	if (it != server_users.end())
		return (1);
	return (0);
}

void		Chanel::set_admin(int new_admin)
{
	admin_fd = new_admin;
}