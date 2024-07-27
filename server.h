//
// Created by ouidriSS on 6/27/24.
//

#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <sys/epoll.h>
#include <vector>
#include <cerrno>
#include "client.hpp"
#include <cstdlib>
#include "channel.h"
#include <fcntl.h>
#include <fstream>


# define RED "\033[31m"
# define GREEN "\033[32m"
# define YELLOW "\033[33m"
# define BLUE "\033[34m"
# define PURPLE "\033[35m"
# define CYAN "\033[36m"
# define RESET "\033[0m"
# define WHITE "\033[37m"
#define MAGENTA "\033[1;35m"
#define BRIGHT_RED "\033[1;91m"
#define BRIGHT_GREEN "\033[1;92m"
#define BRIGHT_YELLOW "\033[1;93m"
#define BRIGHT_BLUE "\033[1;94m"
#define BRIGHT_MAGENTA "\033[1;95m"
#define BRIGHT_CYAN "\033[1;96m"

class Server {
private :
    int						        Server_Socket;
    int                             Client_Socket;
    int                             epoll_fd;
    int                             port;
    int                             opt;
    struct sockaddr_in		        Server_addr;
    struct sockaddr_in		        Client_addr;
    socklen_t				        Server_addrlen;
    socklen_t				        Client_addrlen;
    std::vector<struct epoll_event> Events;
    struct epoll_event              Event;
    struct epoll_event              Client_event;
    std::string                     password;
public :
    Server(int port, std::string pass);
    int		Create_Socket();
    int     Set_ReuseSocket();
    int		Bind_Socket();
    int		Listen();
    int     Setup_Sever();
    int     Multiplexing();
    int     Handle_New_Connection();
    void    Handle_Client_Data(int i, std::map<int, Client> &client, \
            std::map<std::string, Chanel> &chanels);
    void    Handle_Close_Connection(int i);
    ~Server();
};

void   file_transfer(int sender, std::string reciever, std::string path, std::map<int, Client> &server_users);

#endif //SERVER_H
