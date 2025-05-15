# include "channel.h"
# include "client.hpp"


void	Chanel::kick_user(int username_fd, std::string &username_to_kick, std::map<int, Client> &server_users)
{
	std::string msg;

	if (username_to_kick == convert_fd_to_name(admin_fd, server_users))
	{
		msg = RED "Cannot kick admin of Channel\n" RESET;
		return ((void)(send(username_fd, msg.c_str(), msg.length(), 0)));
	}
	if (username_fd != admin_fd)
	{
		if (this->users.find(username_fd) != this->users.end())
		{
			if (!permission_check(username_fd, server_users))
				return ;
		}
		else
		{
			msg = RED  "PERMISSION DENIED : You are not on channel \n"  RESET;
			return ((void)(send(username_fd, msg.c_str(), msg.length(), 0)));
		}
	}
	if (users.find(convert_name_to_fd(username_to_kick, server_users)) != users.end())
	{
		std::vector<std::string>::iterator check = std::find(list_user_invited.begin(), list_user_invited.end(), username_to_kick);
        if (check != list_user_invited.end())
            list_user_invited.erase(check);
		users.erase(convert_name_to_fd(username_to_kick, server_users));
		int fd = convert_name_to_fd(username_to_kick, server_users);
		msg = RED "\nYou have been KICKED from " + chanel_global_name + " Channel by user " + \
		convert_fd_to_name(username_fd, server_users) + "\n" RESET + getTimestamp() + " @" + server_users[fd].username + "(" + server_users[fd].nickname + ") :";
		send(fd, msg.c_str(), msg.length(), 0);
	}
	else
	{
		msg = RED "ERROR : User want to kick (" + username_to_kick + ") Not found on Channel\n" RESET ;
		return ((void)(send(username_fd, msg.c_str(), msg.length(), 0)));
	}
}

void	Kick_manager(int fd_user_cmd, std::string &user_to_kick, std::string &chanel_name, \
		std::map<std::string, Chanel> &chanels, std::map<int, Client> &server_users)
{
	if (Check_Existng_Chanel(chanel_name, chanels))
	{
		if (chanels.find(chanel_name)->second.Check_UserOnChanel(fd_user_cmd))
			chanels.find(chanel_name)->second.kick_user(fd_user_cmd, user_to_kick, server_users);
		else
		{
			std::string msg = RED  "PERMISSION DENIED : You are not on channel \n"  RESET;
			return ((void)(send(fd_user_cmd, msg.c_str(), msg.length(), 0)));
		}
	}
	else
	{
		std::string msg = RED  "Channel not found\n" RESET;
		 send(fd_user_cmd, msg.c_str(), msg.length(), 0);
	}
}
