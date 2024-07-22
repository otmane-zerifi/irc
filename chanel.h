
#ifndef CHANEL_HPP
#define CHANEL_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <string>
#include <unistd.h>
#include <sys/epoll.h>
#include <vector>
#include <map>
#include <cerrno>
#include <algorithm>
#include <sstream> 


# define RED "\033[31m"
# define GREEN "\033[32m"
# define YELLOW "\033[33m"
# define BLUE "\033[34m"
# define PURPLE "\033[35m"
# define CYAN "\033[36m"
# define RESET "\033[0m"

class Client;

typedef struct s_password
{
	bool		active;
	std::string	password;
}				t_password;

typedef struct s_permission
{
	bool						invite_only;
	bool						restrictive_topic;
	double						max_user;
	std::map<std::string , int>	op_privileges;
}						t_permission;

class Chanel
{
    public :
		std::string					chanel_global_name;
        std::string					topic;
		int							admin_fd;
        std::map<int , Client >		users;
		std::vector<std::string>	list_user_invited;
		t_password					password_info;
		t_permission				permision_info;

	public :
		Chanel(int admin, std::string chanel_name , std::string topic);
		~Chanel();
		int		Check_UserOnChanel(int fd_user);
		void	Add_User(int fd_new_user, Client &client, std::string &pass ,std::map<int, Client> &server_users);
		void	kick_user(int username_fd, std::string &username_to_kick, std::map<int, \
				Client> &server_users);
		void	set_new_topic(int fd_user, std::string &topic, std::map<int, Client> &server_users);
		void	invite_user(int fd_user, std::string &username, std::map<int, Client> &server_users);
		void	set_password(int &fd_user, const std::string &pass, bool status, std::map<int, Client> &server_users);
		void	set_invite_only(int fd_user, bool status, std::map<int, Client> &server_users);
		void	set_restrictive_topic(int fd_user, bool status, std::map<int, Client> &server_users);
		void	set_max_user(int fd_user, double max, std::map<int, Client> &server_users);
		void	set_op_privileges(int fd_user, std::string &user_to_rank, int status, std::map<int, Client> &server_users);
		void	Broadcast_message(std::string msg, std::map<int, Client> &server_users, int fd);
		void	set_admin(int new_admin);
		bool	permission_check(int username_fd, std::map<int, Client> &server_users);
		bool	check_list_invitation(int fd_new_user, std::map<int, Client> &server_users);
		bool	check_password(int fd_user , std::string pass, std::map<int, Client> &server_users);
};

int			Check_UserOnServer(std::map<int, Client> &server_users , int fd_user);
int			Check_Existng_Chanel(std::string &name , std::map<std::string, Chanel> &chanels);
void		join_user(std::string &chanel_name, std::map<std::string , Chanel> &chanels, \
			int fd_new_user, std::string &pass, std::map<int, Client> &server_users);
void		Kick_manager(int fd_user_cmd, std::string &user_to_kick, std::string &chanel_name, \
			std::map<std::string, Chanel> &chanels, std::map<int, Client> &server_users);
std::string	convert_fd_to_name(int fd, std::map<int, Client> &server_users);
int			convert_name_to_fd(std::string name, std::map<int, Client> &server_users);
void		topic_manager(int fd_user, std::string &topic, std::string &chanel_name ,std::map<int, Client> &server_users, \
			std::map<std::string , Chanel> &chanels);
void		Invite_manager(int fd_user, std::string &to_invite, std::string chanel_name, \
			std::map<int,Client> &server_users, std::map<std::string , Chanel> &chanels);
void		invite_only_manager(int fd_user, std::string &chanel_name, bool status, std::map<std::string , Chanel> &chanels \
			,std::map<int, Client> &server_users);
void		Password_manager(int fd_user, std::string &new_pass ,bool status, std::string &chanel_name , \
			std::map<int, Client> &server_users, std::map<std::string , Chanel> &chanels);
void		priviliges_manager(int fd_user, std::string &user_to_rank, int status, std::string &chanel_name , \
			std::map<std::string , Chanel> &chanels , std::map<int, Client> &server_users);
void		Max_user_manager(int fd_user, double max, std::string &chanel_name , \
			std::map<std::string , Chanel> &chanels , std::map<int, Client> &server_users);
void		restrictive_topic_manager(int fd_user, bool status, std::string &chanel_name, std::map<std::string , Chanel> &chanels\
 			, std::map<int, Client> &server_users);
#endif