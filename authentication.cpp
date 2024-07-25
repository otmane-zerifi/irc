#include "client.hpp"
#include "server.h"

bool isValidnick(int fd, std::string nickname)
{
    if(!isValiduser(nickname))
    {
        send_error_message(fd, "INVALIDE NICKNAME\n");
        return(false);
    }
    return true;
}

bool check_user(int fd, std::map<int , Client> &clients, std::map<std::string, Chanel> &chanels)
{
    std::string username = clients[fd].arg[1];
        if(!isValiduser(username))
        {
            send_error_message(fd, "INVALIDE USERNAME\n\033[35mENTER VALID USERNAME:");
            return false;
        }
        for (std::map<int, Client>::iterator jt = clients.begin(); jt != clients.end(); ++jt) {
            if (username == jt->second.username) {
            send_error_message(fd, "EXIST USER\n");
            return false; 
            }
        for(std::map<std::string , Chanel>::iterator it = chanels.begin(); it != chanels.end(); ++it)
        {
            if(username == it->first)
                send_error_message(fd, "IS A CHANEL NAME\n");
            return (false);
        }
        }
    return true; // No duplicate usernames found
}

void notification_user(int fd, std::string message)
{
    std::string color = GREEN + message + RESET;
    send(fd, color.c_str(), color.length(), 0);
}

void authentication(int fd, std::map<int , Client> &client, std::string& password)
{
    std::string buff = client[fd].buff;
    if(buff != password)
        send_error_message(fd, "INCORRECT PASSWORD\033[37m\nENTRE PASSWORD : ");
    else if(buff == password)
    {
        client[fd].auth = true;
        notification_user(fd, "----------------->WELCOME TO IRC SERVER<-----------------\n\033[34mENTER YOUR USERNAME:");
    }
}

void set_username(int fd, std::map<int,Client>& client, std::map<std::string, Chanel>& chanels)
{
    std::string buffer = client[fd].buff;
    if(!buffer.empty() && double_user(fd, client, chanels))
    {
        client[fd].username = client[fd].buff;
        std::string message = BLUE "ENTER YOUR NIKENAME:" RESET;
        send(fd, message.c_str(), message.length() , 0);
    }
}

void set_nickname(int fd,  std::map<int,Client>& client)
{
    std::string buffer = client[fd].buff;
   if(!buffer.empty() && !client[fd].username.empty() && isValidnick(fd, buffer))
    {
        client[fd].nickname = client[fd].buff;
        std::string message = GREEN "Welcome, for manual enter /help\n" BLUE + getTimestamp() + " @" + client[fd].username + " :" RESET;
        send(fd, message.c_str(), message.length() , 0);
    }
}