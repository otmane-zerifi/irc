#include "channel.h"
#include "client.hpp"
#include "string.h"

std::string Chanel::getchanelname()
{
	return(chanel_global_name);
}

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

void	Chanel::Broadcast_message(std::string msg, std::map<int, Client> &server_users, int fd)
{
	if (Check_UserOnServer(server_users, admin_fd))
	{
		std::string new_msg = admin_fd == fd ? msg : ("\n" + msg + getTimestamp() + " @" + server_users[admin_fd].username + " :");
		send(admin_fd, new_msg.c_str(), new_msg.length(), 0);
	}
	for (std::map<int , Client >::iterator it = users.begin(); it != users.end(); it ++)
	{
		if(Check_UserOnServer(server_users, it->first))
		{
			std::cerr <<"fd = " << fd << "it->first = " << it->first << std::endl;
			std::string all_msg = (it->first == fd)  ?  msg : ("\n" + msg + getTimestamp() + " @" + server_users[it->first].username + " :");
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

int	convert_name_to_fd(std::string name, std::map<int, Client> &server_users)
{
	std::map<int, Client>::iterator it;
	for (it = server_users.begin() ; it != server_users.end() ; it ++)
	{
		if (it->second.username == name)
			return (it->first);
	}
	return (0);
}

int	Check_Existng_Chanel(std::string &name , std::map<std::string, Chanel> &chanels)
{
	if (chanels.find(name) != chanels.end())
		return (1);
	return (0);
}

int	Chanel::Check_UserOnChanel(int fd_user)
{
	std::map<int, Client>::iterator it = users.find(fd_user);
	if (it != users.end() || fd_user == admin_fd)
		return (1);
	return (0);
}

int	Check_UserOnServer(std::map<int, Client> &server_users , int fd_user)
{
	std::map<int, Client>::iterator it = server_users.find(fd_user);
	if (it != server_users.end())
		return (1);
	return (0);
}

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

void	Chanel::set_admin(int new_admin)
{
	admin_fd = new_admin;
}

void	Chanel::Add_User(int fd_new_user, Client &client, std::string &pass, \
		std::map<int, Client> &server_users)
{
	if (this->permision_info.invite_only)
	{
		std::vector<std::string>::iterator check = std::find(list_user_invited.begin(), \
		list_user_invited.end(), convert_fd_to_name(fd_new_user, server_users));
		if (check == list_user_invited.end())
		{
			std::string msg = RED "To Join Chanel You need invitation\n" RESET;
			return((void)(send(fd_new_user, msg.c_str(), msg.length(), 0)));
		}
	}
	if (this->password_info.active)
	{
		std::string msg = "Password to join chanel is requiered now : ";
		send(fd_new_user, msg.c_str(), msg.length(), 0);
		if (pass != password_info.password)
		{
			std::string msg = RED "Wrong Password" RESET;
			return((void)(send(fd_new_user, msg.c_str(), msg.length(), 0)));
		}
	}
	if (this->permision_info.max_user > 0 && this->permision_info.max_user > users.size())
	{
		std::string msg = RED "Chanel reach max number of users\n" RESET;
		return((void)(send(fd_new_user, msg.c_str(), msg.length(), 0)));
	}
	users.insert(std::pair<int, Client>(fd_new_user, client));
	std::string msg = std::string(GREEN"You have been join to Chanel (") + \
	chanel_global_name + ")\n" RESET;
	send(fd_new_user, msg.c_str(), msg.length(), 0);
}

void	Chanel::kick_user(int username_fd, std::string &username_to_kick, std::map<int, Client> &server_users)
{
	std::string msg;

	if (username_to_kick == convert_fd_to_name(admin_fd, server_users))
	{
		msg = RED "Cannot kick admin of Chanel\n" RESET;
		return ((void)(send(username_fd, msg.c_str(), msg.length(), 0)));
	}
	
	if (username_fd != admin_fd)
	{
		if (this->users.find(username_fd) != this->users.end())
		{
			std::map<std::string , int>::iterator check = permision_info.op_privileges.find(convert_fd_to_name\
															(username_fd, server_users));
			if (check == permision_info.op_privileges.end() || \
			(check != permision_info.op_privileges.end() && check->second != 1))
			{
				msg = RED  "PERMISSION DENIED\n"  RESET;
				return ((void)(send(username_fd, msg.c_str(), msg.length(), 0)));
			}
		}
		else
		{
			msg = RED "USER NOT ON CHANEL\n" RESET;
			return ((void)(send(username_fd, msg.c_str(), msg.length(), 0)));
		}
	}
	if (users.find(convert_name_to_fd(username_to_kick, server_users)) != users.end())
	{
		users.erase(convert_name_to_fd(username_to_kick, server_users));
		msg = RED "You have been KICKED from " + chanel_global_name + " Chanel by user " + \
		convert_fd_to_name(username_fd, server_users) + "\n" RESET;
		send(convert_name_to_fd(username_to_kick, server_users), msg.c_str(), msg.length(), 0);
	}
	else
	{
		msg = RED "ERROR : User want to kick (" + username_to_kick + ") Not found on Chanel\n" RESET ;
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
			std::string msg = RED  "PERMISSION DENIED : You are not on chanel \n"  RESET;
			return ((void)(send(fd_user_cmd, msg.c_str(), msg.length(), 0)));
		}
	}
	else
	{
		std::string msg = RED  "Chanel not found\n" RESET;
		 send(fd_user_cmd, msg.c_str(), msg.length(), 0);
	}
}

void	Chanel::set_new_topic(int fd_user, std::string &topic, std::map<int, Client> &server_users)
{
	std::string msg;

	if (fd_user != admin_fd)
	{
		if (Check_UserOnChanel(fd_user))
		{
			if (this->permision_info.restrictive_topic == true)
			{
				std::map<std::string , int>::iterator check = permision_info.op_privileges.find(convert_fd_to_name(fd_user, server_users));
				if (check == permision_info.op_privileges.end() || \
				(check != permision_info.op_privileges.end() && check->second != 1))
				{
					msg = RED  "PERMISSION DENIED\n"  RESET;
					return ((void)(send(fd_user, msg.c_str(), msg.length(), 0)));
				}
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

void	Invite_manager(int fd_user, std::string &to_invite, std::string chanel_name, std::map<int, \
		Client> &server_users, std::map<std::string , Chanel> &chanels)
{
	std::string msg;

	if (Check_UserOnServer(server_users, fd_user) && Check_UserOnServer(server_users, \
	convert_name_to_fd(to_invite, server_users)))
	{
		if (Check_Existng_Chanel(chanel_name, chanels))
			chanels.find(chanel_name)->second.invite_user(fd_user,to_invite, server_users);
		else
		{
			msg = RED "Chanel not found \n"  RESET;
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
				std::string msg = (GREEN "You invited to chanel ") + this->chanel_global_name + " TO join (JOIN " \
				+ chanel_global_name + ")" + RESET + "\n";
				send(fd_to_invite, msg.c_str(), msg.length(), 0);
				list_user_invited.push_back(to_invite);
			}
			else
			{
				std::vector<std::string>::iterator it = std::find(list_user_invited.begin(), list_user_invited.end(), to_invite);
				if ( it == list_user_invited.end())
					list_user_invited.push_back(to_invite);
				msg = RED "User " + to_invite  + " on queue of Invitation list\n" RESET;
				send(fd_user, msg.c_str(), msg.length(), 0);
			}
		}
		else
		{
			msg = RED  "User Already on Chanel\n" RESET;
			send(fd_user, msg.c_str(), msg.length(), 0);
		}
	}
	else
	{
		msg = RED  "You are not on Chanel to sent invitation\n" RESET;
		send(fd_user, msg.c_str(), msg.length(), 0);
	}
}

void	Password_manager(int fd_user, std::string &new_pass ,bool status, std::string &chanel_name , \
		std::map<int, Client> &server_users, std::map<std::string , Chanel> &chanels)
{
	std::string msg;

	if (Check_Existng_Chanel(chanel_name ,chanels))
		chanels.find(chanel_name)->second.set_password(fd_user, new_pass, status, server_users);
	else
	{
		msg = RED "Chanel not Found\n" RESET;
		send(fd_user, msg.c_str(), msg.length(), 0);
	}
}

void	Chanel::set_password(int &fd_user, const std::string &pass, bool status, std::map<int, Client> &server_users)
{
	std::string msg;

	if (fd_user != admin_fd)
	{
		if (users.find(fd_user) != users.end())
		{
			std::map<std::string , int>::iterator check = permision_info.op_privileges.find(convert_fd_to_name(fd_user, server_users));
			if (check == permision_info.op_privileges.end() || \
			(check != permision_info.op_privileges.end() && check->second != 1))
			{
				msg = RED "PERMISSION DENIED\n" RESET;
				return ((void)(send(fd_user, msg.c_str(), msg.length(), 0)));
			}
		}
		else
		{
			msg = RED "YOU are Not on Chanel\n" RESET;
			return ((void)(send(fd_user, msg.c_str(), msg.length(), 0)));
		}
	}
	if (status)
		msg = GREEN "Set ON password of Chanel\n" RESET;
	else
		msg = GREEN "Set OFF password of Chanel\n" RESET;
	Broadcast_message(msg, server_users, fd_user);
	this->password_info.active = status;
	this->password_info.password = pass;
}


void	Chanel::set_invite_only(int fd_user, bool status, std::map<int, Client> &server_users)
{
	std::string msg;

	if (fd_user != admin_fd)
	{
		if (users.find(fd_user) != users.end())
		{
			std::map<std::string , int>::iterator check = permision_info.op_privileges.find(\
			convert_fd_to_name(fd_user, server_users));
			if (check == permision_info.op_privileges.end() || \
			(check != permision_info.op_privileges.end() && check->second != 1))
			{
				msg = RED "PERMISSION DENIED\n"  RESET;
				return ((void)(send(fd_user, msg.c_str(), msg.length(), 0)));
			}
		}
		else
		{
			msg = RED  "USER NOT ON CHANEL\n" RESET;
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



// void	chanel::set_restrictive_topic(std::string &username, bool status)
// {
// 	if (username != admin.username)
// 	{
// 		if (users.find(username) != users.end())
// 		{
// 			std::map<std::string , int>::iterator check = password_info.op_privileges.find(username);
// 			if (check == password_info.op_privileges.end() || \
// 			(check != password_info.op_privileges.end() && check->second != 1))
// 				return ((void)std::cout << RED << "PERMISSION DENIED" << RESET << std::endl);
// 		}
// 		else
// 			return ((void)std::cout << RED << "USER NOT ON CHANEL" << RESET << std::endl);
// 	}
// 	this->permission_info.restrictive_topic = status;
// }

void	Chanel::set_max_user(int fd_user, double max, std::map<int, Client> &server_users)
{
	std::string msg;

	if (fd_user != admin_fd)
	{
		if (users.find(fd_user) != users.end())
		{
			std::map<std::string , int>::iterator check = permision_info.op_privileges.find(convert_fd_to_name(fd_user, server_users));
			if (check == permision_info.op_privileges.end() || \
			(check != permision_info.op_privileges.end() && check->second != 1))
			{
				msg = RED "PERMISSION DENIED\n" RESET;
				return ((void)(send(fd_user, msg.c_str(), msg.length(), 0)));
			}
		}
		else
		{
			msg = RED "You are Not on Chanel\n" RESET;
			return ((void)(send(fd_user, msg.c_str(), msg.length(), 0)));
		}
	}
	if (max == -1)
		msg = GREEN "Number of Client on Chanel is set to unlimited\n" RESET;
	else
	{
		std::stringstream ss;
		ss << max;
		msg = GREEN "Number of Client on Chanel is set to " + ss.str() + "\n" RESET;
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
		msg = RED "Chanel not Found\n" RESET;
		return ((void)(send(fd_user, msg.c_str(), msg.length(), 0)));
	}
}

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
			std::map<std::string , int>::iterator check = permision_info.op_privileges.find(convert_fd_to_name(fd_user, server_users));
			if (check == permision_info.op_privileges.end() || \
			(check != permision_info.op_privileges.end() && check->second != 1))
			{
				msg = RED "PERMISSION DENIED\n" RESET;
				return ((void)(send(fd_user, msg.c_str(), msg.length(), 0)));
			}
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