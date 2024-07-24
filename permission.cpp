#include "channel.h"
#include "client.hpp"

bool	Chanel::permission_check(int username_fd, std::map<int, Client> &server_users)
{
	std::map<std::string , int>::iterator check = permision_info.op_privileges.find(convert_fd_to_name\
													(username_fd, server_users));
	if (check == permision_info.op_privileges.end() || \
	(check != permision_info.op_privileges.end() && check->second != 1))
	{
		std::string msg = RED  "PERMISSION DENIED\n"  RESET;
		return ((void)(send(username_fd, msg.c_str(), msg.length(), 0)), false);
	}
	return true;
}

bool	Chanel::check_list_invitation(int fd_new_user, std::map<int, Client> &server_users)
{
	std::vector<std::string>::iterator check = std::find(list_user_invited.begin(), \
		list_user_invited.end(), convert_fd_to_name(fd_new_user, server_users));
	if (check == list_user_invited.end())
	{
		std::string msg = RED "To Join Channel You need invitation\n" RESET;
		return((void)(send(fd_new_user, msg.c_str(), msg.length(), 0)), false);
	}
	return true;
}

bool	Chanel::check_password(int fd_new_user, std::string pass, std::map<int, Client> &server_users)
{
	std::string msg;
	if (pass != password_info.password)
	{
		if (pass == "")
			msg = RED "Password to join channel is requiered\n" RESET;
		else
			msg = RED "Wrong Password\n" RESET;
		return((void)(send(fd_new_user, msg.c_str(), msg.length(), 0)), false);
	}
	return true ;
}