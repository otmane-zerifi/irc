#include "server.h"
#include <map>
void	print(std::map<int , std::string> &user)
{
	std::map<int, std::string>::iterator it;
	for (it = user.begin() ; it != user.end() ; it++)
		std::cout << "USER #" << it->first << " name : " << it->second << std::endl;
}

int main()
{
	Server srv(8080, "PASS");
	if (srv.Setup_Sever())
	{
		srv.Multiplexing();
	}
	// std::map<int , std::string> user;
	// user.insert(0, "HAMID");
	// user.insert(1, "HAMIDO");
	// user.insert(1, "HAMIDX");

	// user.erase(0);
	// print(user);
	return (0);
}