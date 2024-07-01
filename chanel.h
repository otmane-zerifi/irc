
#ifndef CHANEL_HPP
#define CHANEL_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <sys/epoll.h>
#include <vector>
#include <map>
#include <cerrno>


# define RED "\033[31m"
# define GREEN "\033[32m"
# define YELLOW "\033[33m"
# define BLUE "\033[34m"
# define PURPLE "\033[35m"
# define CYAN "\033[36m"
# define RESET "\033[0m"

struct s_password
{
	bool		active;
	std::string	password;
}				t_password;

struct s_permision
{
	bool							invite_only;
	bool							restrictive_topic;
	int								max_user;
	std::map<std::string , int>		op_privileges;
}									t_permission;

class Chanel
{
    private :
        std::string						topic;
		User							admin;
        std::map<std::string , User>	users;
		struct t_password				password_info;
		struct t_permission				permision_info;
	public :
		Chanel(User &admin, const std::string &topic);
		~Chanel();
		void	set_password(const std::string &pass);
		void	remove_password();
		void	set_invite_only(bool status);
		void	set_restrictive_topic(bool status);
		void	set_max_user(int max);
		void	set_op_privileges(std::string &username);
};

#endif