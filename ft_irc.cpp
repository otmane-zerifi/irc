#include "server.h"
#include <map>
void	print(std::map<int , std::string> &user)
{
	std::map<int, std::string>::iterator it;
	for (it = user.begin() ; it != user.end() ; it++)
		std::cout << "USER #" << it->first << " name : " << it->second << std::endl;
}
bool isValidPort(const std::string& portStr) {
    // Check if the port string is a valid integer
    for (std::string::const_iterator it = portStr.begin(); it != portStr.end(); ++it) {
        if (!std::isdigit(static_cast<unsigned char>(*it))) {
            return false;
        }
    }
    // Convert string to integer
    int port = std::atoi(portStr.c_str());

    // Check if the port number is within the valid range
    return port > 1023 && port <= 65535;
}

int main(int ac, char **av)
{
	if (ac != 3) {
        std::cerr << "Usage: " << av[0] << " <port> <password>" << std::endl;
        return 1;
    }
	std::string portStr = av[1];
    if (!isValidPort(portStr)) {
        std::cerr << "Error: Invalid port number. Please enter a port number between 1024 and 65535." << std::endl;
        return 1;
    }
	Server srv(std::atoi(av[1]), av[2]);
	if (srv.Setup_Sever())
	{
	std::cout << "Starting server on port " << av[1] << " with password " << av[2] << std::endl;
		srv.Multiplexing();
	}
	return (0);
}