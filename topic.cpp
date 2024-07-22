# include "chanel.h"
# include "client.hpp"

void	Chanel::set_new_topic(int fd_user, std::string &topic, std::map<int, Client> &server_users)
{
	std::string msg;

	if (fd_user != admin_fd)
	{
		if (Check_UserOnChanel(fd_user))
		{
			if (this->permision_info.restrictive_topic == true)
			{
				if (!permission_check(fd_user, server_users))
					return ;
			}
		}
		else
		{
			msg = RED  "You're not on Channel\n"  RESET;
			return ((void)(send(fd_user, msg.c_str(), msg.length(), 0)));
		}
	}
	this->topic = topic;
	msg = GREEN "New Topic set to " + this->topic + " by " + \
	convert_fd_to_name(fd_user, server_users) + "\n" RESET;
	Broadcast_message(msg, server_users, fd_user);
}

void	topic_manager(int fd_user, std::string &topic, std::string &chanel_name ,std::map<int, Client> &server_users, \
		std::map<std::string , Chanel> &chanels)
{
	if (Check_Existng_Chanel(chanel_name, chanels))
		chanels.find(chanel_name)->second.set_new_topic(fd_user, topic, server_users);
	else
	{
		std::string msg = RED  "Chanel not Found\n"  RESET;
		return ((void)(send(fd_user, msg.c_str(), msg.length(), 0)));
	}
}

void	Chanel::set_restrictive_topic(int fd_user, bool status, std::map<int, Client> &server_users)
{
	std::string msg;

	if (fd_user != admin_fd)
	{
		if (users.find(fd_user) != users.end())
		{
			if (!permission_check(fd_user, server_users))
				return ;
		}
		else
		{
			msg = RED  "You're NOT ON CHANEL\n" RESET;
			return ((void)(send(fd_user, msg.c_str(), msg.length(), 0)));
		}
	}
	if (status)
		msg = GREEN "Chanel set on restrictive topic\n" RESET;
	else
		msg = GREEN "Chanel set off restrictive topic\n" RESET;
	Broadcast_message(msg, server_users, fd_user);
	this->permision_info.restrictive_topic = status;
}

void	restrictive_topic_manager(int fd_user, bool status, std::string &chanel_name, std::map<std::string , Chanel> &chanels\
 		, std::map<int, Client> &server_users)
{
	std::string msg;

	if (Check_Existng_Chanel(chanel_name, chanels))
		chanels.find(chanel_name)->second.set_restrictive_topic(fd_user, status, server_users);
	else
	{
		msg = RED "Chanel not Found\n" RESET;
		return ((void)(send(fd_user, msg.c_str(), msg.length(), 0)));
	}
}
