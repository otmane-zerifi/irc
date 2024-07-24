#include "server.h"
#include "client.hpp"
#include "channel.h"

void handle_command(int fd, std::map<int,Client>& client, std::map<std::string, Chanel>& chanels)
{
    std::string cmd;
    client[fd].arg = splitString(client[fd].buff);
    if(!client[fd].arg.empty())
        cmd = client[fd].arg[0];
    bool check = check_cmd(fd, client);
    if (cmd == "/send" && check)
        send_command(fd, client, chanels);
    else if(cmd == "/help")
        send_command_table(fd, client[fd].username);
    else if(cmd == "/user")
        user_command(fd, client, chanels);
    else if(cmd == "/nick" && check)
        nick_command(fd, client);
    else if(cmd == "/join" && check)
        join_command(fd, client, chanels);
    else if(cmd == "/kick" && check)
        Kick_manager(fd, client[fd].arg[1], client[fd].arg[2], chanels, client);
    else if(cmd == "/invite" && check)
        Invite_manager(fd, client[fd].arg[1], client[fd].arg[2], client, chanels);
    else  if(cmd == "/topic" && check)
        topic_manager(fd, client[fd].arg[2], client[fd].arg[1], client, chanels);
    else if(cmd == "/mode" && check)
        mode_command(fd, client, chanels);
}

void parss_data(int fd, std::map<int,Client>& client, std::string& password, std::map<std::string, Chanel>& chanels)
{
    (void) chanels;
    char *buffer = client[fd].buffer;
    std::string buff = client[fd].buff;
    if(!client[fd].auth)
        authentication(fd, client, password);
    else if(client[fd].auth && client[fd].username.empty())
        set_username(fd, client, chanels);
    else if(client[fd].auth && client[fd].nickname.empty())
        set_nickname(fd, client);
    else if(!client[fd].username.empty() && !client[fd].nickname.empty())
    {
        handle_command(fd, client, chanels);
    std::string str = PURPLE  + getTimestamp() + " @" + client[fd].username + " :" RESET;
    send(fd, str.c_str(), str.length(), 0);
    }
}