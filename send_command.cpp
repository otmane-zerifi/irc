#include "client.hpp"
#include "server.h"

std::string get_message(std::vector<std::string> &str)
{
    return(str[2] + "\n");
}

int fd_ofuser(std::string username, std::map<int, Client> client) {
    std::map<int, Client>::iterator it;
    for (it = client.begin(); it != client.end(); ++it) {
        if (it->second.username == username) {
            return it->first; // Return the key associated with the username
        }
    }
    return -1; // Return -1 if username not found
}

void send_message(int fd, std::map<int , Client> client)
{
    if(client[fd].arg.size() > 3 || client[fd].arg.size() < 3)
    {
        send_error_message(fd, "INCOMPLET ARGUMENT\n");
        return ;
    }
   int fd_user =  fd_ofuser(client[fd].arg[1], client);
   if(fd_user < 0)
        return((void)send_error_message(fd, "USER NOT FOUND\n"));
   std::string message = GREEN "message from @" + client[fd].username  + " :" + get_message(client[fd].arg);
   std::string new_messag = (fd_user == fd) ?  message : "\n" + message + PURPLE + getTimestamp() + " @" + client[fd_user].username + " :";
   if(new_messag.empty())
        return((void)send_error_message(fd, "NO MESSAGE\n"));
   else
        send(fd_user, new_messag.c_str(), new_messag.length(), 0);
}

void send_command(int fd, std::map<int,Client>& client, std::map<std::string, Chanel>& chanels)
{
    if(fd_ofuser(client[fd].arg[1], client) >= 0)
        send_message(fd, client);
    else if (Check_Existng_Chanel(client[fd].arg[1], chanels))
        {
            if(chanels.find(client[fd].arg[1])->second.Check_UserOnChanel(fd))
            {
                std::string msg = GREEN "-->\nmessage from @" + client[fd].username + " in channel #" + client[fd].arg[1] + " : " + client[fd].arg[2] + "\n" RESET;
                chanels.find(client[fd].arg[1])->second.Broadcast_message(msg, client, fd);
            }
            else
                send_error_message(fd, "YOU ARE NOT IN THIS CHANNEL , JOIN IT\n");
        }
         else 
            send_error_message(fd, "INVALIDE ADDRESSEE\n");
}