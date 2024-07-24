# include "channel.h"
# include "client.hpp"

void	Chanel::set_max_user(int fd_user, double max, std::map<int, Client> &server_users)
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
			msg = RED "You are Not on Channel\n" RESET;
			return ((void)(send(fd_user, msg.c_str(), msg.length(), 0)));
		}
	}
	if (max == -1)
		msg = GREEN "Number of Client on Channel is set to unlimited\n" RESET;
	else
	{
		std::stringstream ss;
		ss << max;
		msg = GREEN "Number of Client on Channel is set to " + ss.str() + "\n" RESET;
	}
	Broadcast_message(msg, server_users, fd_user);
	this->permision_info.max_user = max;
}

void	Max_user_manager(int fd_user, double max, std::string &chanel_name , \
						std::map<std::string , Chanel> &chanels , std::map<int, Client> &server_users)
{
	std::string msg;

	if (Check_Existng_Chanel(chanel_name, chanels))
		chanels.find(chanel_name)->second.set_max_user(fd_user, max, server_users);
	else
	{
		msg = RED "Channel not Found\n" RESET;
		return ((void)(send(fd_user, msg.c_str(), msg.length(), 0)));
	}
}