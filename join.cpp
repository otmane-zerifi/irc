# include "chanel.h"
# include "client.hpp"

void	join_user(std::string &chanel_name, std::map<std::string , Chanel> &chanels, int fd_new_user, \
		std::string &pass,std::map<int, Client> &server_users)
{
	if (Check_UserOnServer(server_users, fd_new_user))
	{
		if (Check_Existng_Chanel(chanel_name, chanels))
		{
			if (!chanels.find(chanel_name)->second.Check_UserOnChanel(fd_new_user))
				chanels.find(chanel_name)->second.Add_User(fd_new_user, server_users.find(fd_new_user)->second,\
				pass, server_users);
			else
			{
				std::string msg(RED "YOU already on Chanel\n" RESET);
				send(fd_new_user, msg.c_str(), msg.length(), 0);
			}
		}
		else
		{
			std::string msg = std::string(GREEN"You just Create a new Chanel (") + chanel_name.c_str() + ") and now you're the admin\n" RESET;
			send(fd_new_user, msg.c_str(), msg.length(), 0);
			Chanel new_chanel(fd_new_user, chanel_name, std::string("topic name"));
			chanels.insert(std::pair<std::string, Chanel>(chanel_name, new_chanel));
			chanels.find(chanel_name)->second.set_admin(fd_new_user);
		}
	}
	else
		std::cout << RED << "Client not exist" << RESET << std::endl;
}


void	Chanel::Add_User(int fd_new_user, Client &client, std::string &pass, \
		std::map<int, Client> &server_users)
{
	if (this->permision_info.invite_only && !check_list_invitation(fd_new_user, server_users))
		return ;
	if (this->password_info.active && !check_password(fd_new_user, pass, server_users))
		return ;
	if (this->permision_info.max_user >= 0 && this->permision_info.max_user <= users.size())
	{
		std::string msg = RED "Chanel reach max number of users\n" RESET;
		return((void)(send(fd_new_user, msg.c_str(), msg.length(), 0)));
	}
	users.insert(std::pair<int, Client>(fd_new_user, client));
	std::string msg = std::string(GREEN"You have been join to Chanel (") + \
	chanel_global_name + ")\n" RESET;
	send(fd_new_user, msg.c_str(), msg.length(), 0);
}