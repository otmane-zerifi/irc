# include "client.hpp"
# include "chanel.h"

void	Chanel::set_invite_only(int fd_user, bool status, std::map<int, Client> &server_users)
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
		msg = GREEN "Chanel set on invite only\n" RESET;
	else
		msg = GREEN "Chanel set off invite only\n" RESET;
	Broadcast_message(msg, server_users, fd_user);
	this->permision_info.invite_only = status;
}

void	invite_only_manager(int fd_user, std::string &chanel_name, bool status, std::map<std::string , Chanel> &chanels \
		,std::map<int, Client> &server_users)
{
	std::string msg;

	if (Check_Existng_Chanel(chanel_name, chanels))
		chanels.find(chanel_name)->second.set_invite_only(fd_user, status, server_users);
	else
	{
		msg = RED  "Chanel not Found\n" RESET;
		return ((void)(send(fd_user, msg.c_str(), msg.length(), 0)));
	}
}
