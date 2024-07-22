#include "client.hpp"
#include "server.h"

bool isValidnick(int fd, std::string nickname)
{
    if(!isValiduser(nickname))
    {
        send(fd, "INVALIDE NICKNAME\n", 19,0);
        return(false);
    }
    return true;
}

bool check_user(int fd, std::map<int , Client> &clients, std::map<std::string, Chanel> &chanels)
{
    std::string username = clients[fd].arg[1];
        if(!isValiduser(username))
        {
            send(fd, "INVALIDE USERNAME\nENTER VALID USERNAME:", 40, 0);
            return false;
        }
        for (std::map<int, Client>::iterator jt = clients.begin(); jt != clients.end(); ++jt) {
            if (username == jt->second.username) {
                send(fd, "EXIST USER\n" , 12 , 0);
            return false; 
            }
        for(std::map<std::string , Chanel>::iterator it = chanels.begin(); it != chanels.end(); ++it)
        {
            if(username == it->first)
                send(fd, "IS A CHANEL NAME\n", 18, 0);
            return (false);
        }
        }
    return true; // No duplicate usernames found
}

void authentication(int fd, std::map<int , Client> &client, std::string& password)
{
    std::string buff = client[fd].buff;
    if(buff != password)
        send(fd, "\033[31mINCORRECT PASSWORD\033[0m\nENTRE PASSWORD : ", 46, 0);
    else if(buff == password)
    {
        client[fd].auth = true;
        send(fd, "\033[1;32mWELCOM TO IRC SERVER\nENTER YOUR USERNAME:", 49, 0);
    }
}

void set_username(int fd, std::map<int,Client>& client, std::map<std::string, Chanel>& chanels)
{
    std::string buffer = client[fd].buff;
    if(!buffer.empty() && double_user(fd, client, chanels))
    {
        client[fd].username = client[fd].buff;
        send(fd, "\033[1;32mENTER YOUR NIKENAME:", 28 , 0);
    }
}

void set_nickname(int fd,  std::map<int,Client>& client)
{
    std::string buffer = client[fd].buff;
   if(!buffer.empty() && !client[fd].username.empty() && isValidnick(fd, buffer))
    {
        client[fd].nickname = client[fd].buff;
        std::string message = "Welcom, for manual enter /help\n" + getTimestamp() + " @" + client[fd].username + " :";
        send(fd, message.c_str(), message.length() , 0);
    }
}