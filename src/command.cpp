#include "client.hpp"
#include "server.h"

void user_command(int fd,std::map<int,Client>& client, std::map<std::string, Chanel>& chanels )
{
    int nb = client[fd].arg.size();
    if(nb > 2)
        send_error_message(fd, "TO MANY ARGUMENT\n");
    else if(nb < 2)
        send_error_message(fd, "NOT ENOUGH ARGUMENT\n");
    else if(check_user(fd, client, chanels))
        client[fd].username = client[fd].arg[1];
}

void nick_command(int fd, std::map<int,Client>& client)
{
    int nb = client[fd].arg.size();
    if(nb > 2)
        send_error_message(fd, "TO MANY ARGUMENT\n");
    else if(nb < 2)
        send_error_message(fd, "NOT ENOUGH ARGUMENT\n");
    else if(isValidnick(fd, client[fd].arg[1]))
        client[fd].nickname = client[fd].arg[1];
}

void join_command(int fd, std::map<int,Client>& client, std::map<std::string, Chanel>& chanels)
{
    std::string pass = "";
    if (client[fd].arg.size() > 2)
        pass = client[fd].arg[2];
    join_user(client[fd].arg[1], chanels, fd, pass, client);
}

void option_mode(int fd, std::map<int,Client>& client, std::map<std::string, Chanel>& chanels)
{
    if (client[fd].arg[1] == "+o" || client[fd].arg[1] == "-o")
    {
        int status = 1;
        if (client[fd].arg[1] == "-o")
            status = 0;
        priviliges_manager(fd, client[fd].arg[2], status, client[fd].arg[3], chanels, client);
    }
    if (client[fd].arg[1] == "+l" || client[fd].arg[1] == "-l")
    {
        int max = -1;
        if (client[fd].arg[1] == "+l")
        {
            std::stringstream ss(client[fd].arg[3]);
            ss >> max;
        }
        Max_user_manager(fd, max, client[fd].arg[2], chanels, client);
    }
}

void mode_command(int fd, std::map<int,Client>& client, std::map<std::string, Chanel>& chanels)
{
    if (client[fd].arg[1] == "+i" || client[fd].arg[1] == "-i")
    {
        int status = 1;
        if (client[fd].arg[1] == "-i")
            status = 0;
        invite_only_manager(fd, client[fd].arg[2], status, chanels, client);
    }
    if (client[fd].arg[1] == "+k" || client[fd].arg[1] == "-k")
    {
        std::string pass;
        int status = 1;

        if (client[fd].arg[1] == "-k")
        {
            pass = "";
            status = 0;
        }
        else
            pass = client[fd].arg[3];
        Password_manager(fd, pass , status, client[fd].arg[2], client, chanels);
    }
    option_mode(fd, client, chanels);
}
