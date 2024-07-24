# include "channel.h"
# include "client.hpp"

void	Invite_manager(int fd_user, std::string &to_invite, std::string chanel_name, std::map<int, \
		Client> &server_users, std::map<std::string , Chanel> &chanels)
{
	std::string msg;

	if (Check_UserOnServer(server_users, convert_name_to_fd(to_invite, server_users)))
	{
		if (Check_Existng_Chanel(chanel_name, chanels))
			chanels.find(chanel_name)->second.invite_user(fd_user,to_invite, server_users);
		else
		{
			msg = RED "Channel not found \n"  RESET;
			return ((void)(send(fd_user, msg.c_str(), msg.length(), 0)));
		}
	}
	else
	{
		msg = RED "User want to invite not exist on Server\n"  RESET ;
		return ((void)(send(fd_user, msg.c_str(), msg.length(), 0)));
	}
}

void	Chanel::invite_user(int fd_user, std::string &to_invite, std::map<int, Client> &server_users)
{
	std::string msg;

	if (Check_UserOnChanel(fd_user) || fd_user == admin_fd)
	{
		int fd_to_invite = convert_name_to_fd(to_invite, server_users);
		if (Check_UserOnServer(server_users, fd_to_invite) && !Check_UserOnChanel(fd_to_invite))
		{
			if (std::find(list_user_invited.begin(), list_user_invited.end(), to_invite) == list_user_invited.end())
			{
				std::string msg = (GREEN "\nYou invited to channel ") + this->chanel_global_name + " TO join (JOIN " \
				+ chanel_global_name + ")" + RESET + "\n" + getTimestamp() + " @" + server_users[fd_user].username + " :";
				send(fd_to_invite, msg.c_str(), msg.length(), 0);
				list_user_invited.push_back(to_invite);
			}
			else
			{
				std::vector<std::string>::iterator it = std::find(list_user_invited.begin(), list_user_invited.end(), to_invite);
				if ( it == list_user_invited.end())
					list_user_invited.push_back(to_invite);
				msg = WHITE "User " + to_invite  + " on queue of Invitation list\n" RESET;
				send(fd_user, msg.c_str(), msg.length(), 0);
			}
		}
		else
		{
			msg = RED  "User Already on Channel\n" RESET;
			send(fd_user, msg.c_str(), msg.length(), 0);
		}
	}
	else
	{
		msg = RED  "PERMISSION DENIED : You are not on channel \n"  RESET;
		send(fd_user, msg.c_str(), msg.length(), 0);
	}
}


