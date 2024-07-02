#include "server.h"
#include "client.hpp"

bool double_user(int fd, std::map<int, Client> &clients) {

        std::string username = clients[fd].buffer;
        for (std::map<int, Client>::iterator jt = clients.begin(); jt != clients.end(); ++jt) {
            if (username == jt->second.username) {
                send(fd, "EXIST USER" , 11 , 0);
                std::cerr << "found same user\n";
                return false; 
            }
        }
        std::cerr << "user not found\n";
    return true; // No duplicate usernames found
}

std::string get_user(const std::string& cmd) {
    std::string result;

    // Skip leading whitespace characters
    std::string cm = cmd.substr(4);
    std::string::const_iterator it = cm.begin();
    while (it != cm.end() && std::isspace(static_cast<unsigned char>(*it))) {
        ++it;
    }

    // Extract characters until the first colon (or end of string)
    while (it != cm.end() && *it != ':') {
        result += *it++;
    }

    // Remove trailing whitespace from result (if any)
    size_t endpos = result.find_last_not_of(" \t");
    if (endpos != std::string::npos) {
        result = result.substr(0, endpos + 1);
    }

    return result;
}

std::string get_message(std::string str)
{
    std::string::size_type pos = str.find(':'); // Find the position of ':'
    if (pos != std::string::npos) {
        return str.substr(pos + 1); // Return substring after ':'
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
   std::string username =  get_user(client[fd].buffer);
   std::string message = get_message(client[fd].buffer);
   std::cerr <<"username" << username << std::endl;
   std::cerr << "message" << message << std::endl;
   int fd_user =  fd_ofuser(username, client);
   send(fd_user, message.c_str(), message.length(), 0);
}

std::string seconde_arg(const std::string &str, int index) {
    // Get the substring starting from the specified index
    if (index >= static_cast<int>(str.length())) {
        return "";
    }
    std::string sub = str.substr(index);

    // Trim leading whitespace
    std::string::const_iterator start = sub.begin();
    while (start != sub.end() && std::isspace(static_cast<unsigned char>(*start))) {
        ++start;
    }

    // Trim trailing whitespace
    std::string::const_iterator end = sub.end();
    do {
        --end;
    } while (end != start && std::isspace(static_cast<unsigned char>(*end)));

    // If the entire string is whitespace, return an empty string
    if (start == sub.end()) {
        return "";
    }

    // Construct the trimmed string
    return std::string(start, end + 1);
}

void parss_data(int fd, std::map<int,Client> &client)
{
    char *buffer = client[fd].buffer;
    std::string buff = client[fd].buffer;
    std::cerr << "buff " << buff << std::endl;
    const char* messages = 
        "<<<<<<<<<<<<<<COMMAND>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n"
        RED"* JOIN   - join a channel\n"
        RED"* KICK   - Eject a client from the channel\n" // Red
        "\033[1;32m∗ INVITE - Invite a client to a channel\n" // Green
        "\033[1;33m∗ TOPIC  - Change or view the channel topic\n" // Yellow
        "\033[1;34m∗ MODE   - Change the channel’s mode:\n" // Blue
        "\033[1;35m· i: Set/remove Invite-only channel\n" // Magenta
        "\033[1;36m· t: Set/remove the restrictions of the TOPIC command to channel operators\n" // Cyan
        "\033[1;37m· k: Set/remove the channel key (password)\n" // White
        "\033[1;31m· o: Give/take channel operator privilege\n" // Red
        "\033[1;32m· l: Set/remove the user limit to channel\n"
        BLUE "SEND    : send message to other client , username:'your message'\n"
        RESET; // Green
    
    std::cerr << "fd " << fd << client[fd].username;
    if(!client[fd].auth && strcmp(buffer, PASSWORD))
        send(fd, "\033[31m INCORRECT PASSWORD\n", 26, 0);
    else if(!client[fd].auth && !strcmp(buffer, PASSWORD))
    {
        client[fd].auth = true;
        send(fd, "\033[1;32mWELCOM TO IRC SERVER\nENTER YOUR USERNAME:", 49, 0);
    }
    else if(client[fd].auth && client[fd].username.empty() && *buffer != 0 && double_user(fd, client))
    {
        client[fd].username = client[fd].buffer;
        send(fd, "\033[1;32mENTER YOUR NIKENAME:",29, 0);
    }
    else if(client[fd].auth && client[fd].nickname.empty() && *buffer != 0 && !client[fd].username.empty())
    {
        client[fd].nickname = client[fd].buffer;
        send(fd, messages, 634, 0);
    }
    else 
    {
        if (!strncmp(buffer,"SEND", 4))
        {
            std::cerr << "is in \n";
            client[fd].cmd[0] = "SEND";
            client[fd].cmd[1] = seconde_arg(buff, sizeof("JOIN"));
            if(client[fd].cmd[1].empty())
                send(fd, "NO ARGUMENT\n", 13, 0);
            else
                send_message(fd, client);
        }
       else if(!strncmp(buffer,"JOIN", 4))
        {
            client[fd].cmd[0] = "JOIN";
            client[fd].cmd[1] =  seconde_arg(buff, sizeof("JOIN"));
             if(client[fd].cmd[1].empty())
                send(fd, "NO ARGUMENT\n", 13, 0);
        }
       else if(!strncmp(buffer,"KICK", 4))
        {
            client[fd].cmd[0] = "KICK";
            client[fd].cmd[1] = seconde_arg(buff, sizeof("KICK"));
             if(client[fd].cmd[1].empty())
                send(fd, "NO ARGUMENT\n", 13, 0);
        }
       else if(!strncmp(buffer,"INVITE", 6))
        {
            client[fd].cmd[0] = "INVITE";
            client[fd].cmd[1] = seconde_arg(buff, sizeof("INVITE"));
             if(client[fd].cmd[1].empty())
                send(fd, "NO ARGUMENT\n", 13, 0);
        }
       else if(!strncmp(buffer,"TOPIC", 5))
        {
            client[fd].cmd[0] = "TOPIC";
            client[fd].cmd[1] = seconde_arg(buff, sizeof("TOPIC"));
             if(client[fd].cmd[1].empty())
                send(fd, "NO ARGUMENT\n", 13, 0);
        }
      else  if(!strncmp(buffer,"MODE", 4))
        {
            client[fd].cmd[0] = "MODE";
            client[fd].cmd[1] = seconde_arg(buff, sizeof("MODE"));
             if(client[fd].cmd[1].empty())
                send(fd, "NO ARGUMENT\n", 13, 0);
        }
        else
            send(fd, "COMMAND NOT FOUND\n", 19, 0);
    }
}