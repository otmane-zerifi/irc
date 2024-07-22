# include "chanel.h"
# include "client.hpp"

void	priviliges_manager(int fd_user, std::string &user_to_rank, int status, std::string &chanel_name , \
						std::map<std::string , Chanel> &chanels , std::map<int, Client> &server_users)
{
	std::string msg;

	if (Check_Existng_Chanel(chanel_name, chanels))
		chanels.find(chanel_name)->second.set_op_privileges(fd_user, user_to_rank, status, server_users);
	else
	{
		msg = RED "Chanel not Found\n" RESET;
		return ((void)(send(fd_user, msg.c_str(), msg.length(), 0)));
	}
}

void	Chanel::set_op_privileges(int fd_user, std::string &user_to_rank, int status,std::map<int, Client> &server_users)
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
			msg = RED "You are Not on Chanel\n" RESET;
			return ((void)(send(fd_user, msg.c_str(), msg.length(), 0)));
		}
	}
	if (convert_fd_to_name(admin_fd, server_users) == user_to_rank)
	{
		msg = RED "Admin already have privileges\n" RESET;;
		return ((void)(send(fd_user, msg.c_str(), msg.length(), 0)));
	}
	if (users.find(convert_name_to_fd(user_to_rank, server_users)) != users.end())
	{
		this->permision_info.op_privileges[user_to_rank] = status;
		msg = GREEN "User " + user_to_rank + " Apply ranking " + ((status == 1) ? "UP" : "DOWN") + " on it\n" RESET;
		return ((void)(send(fd_user, msg.c_str(), msg.length(), 0)));
	}
	else
	{
		msg = RED "User want to set rank " + std::string(((status == 1) ? "UP" : "DOWN")) +  " on it not found\n" RESET;;
		return ((void)(send(fd_user, msg.c_str(), msg.length(), 0)));
	}
}