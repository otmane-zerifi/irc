#include "client.hpp"
#include "server.h"

std::string get_message(std::string str)
{
    std::string::size_type pos = str.find(':'); // Find the position of ':'
    if (pos != std::string::npos && str.length() > pos + 1) {
        return trim(str.substr(pos + 1)) + "\n";
              // Return substring after ':'
    } else {
        return ""; // Return empty string if ':' is not found
    }
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
    if(client[fd].arg.size() > 4 || client[fd].arg.size() < 4)
    {
        send(fd, "INCOMPLET ARGUMENT\n", 20, 0);
        return ;
    }
   int fd_user =  fd_ofuser(client[fd].arg[1], client);
   if(fd_user < 0)
        return((void)send(fd, "USER NOT FOUND\n", 16, 0));
   std::string message = "message from @" + client[fd].username  + " :" + get_message(client[fd].buffer);
   std::string new_messag = (fd_user == fd) ?  message : "\n" + message + getTimestamp() + " @" + client[fd_user].username + " :";
   if(new_messag.empty())
        return((void)send(fd, "NO MESSAGE\n", 12, 0));
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
                std::string msg = "-->\nmessage from @" + client[fd].username + " in channel #" + client[fd].arg[1] + " : " + client[fd].arg[3] + "\n";
                chanels.find(client[fd].arg[1])->second.Broadcast_message(msg, client, fd);
            }
            else
                send(fd, "YOU ARE NOT IN THIS CHANNEL , JOIN IT\n", 39, 0);
        }
         else 
            send(fd, "INVALIDE ADDRESSEE\n", 20, 0);
}