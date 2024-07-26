#include "client.hpp"
#include "server.h"

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

std::string trim(const std::string &str) {
    // Trim leading whitespace
    std::string::const_iterator start = str.begin();
    while (start != str.end() && std::isspace(static_cast<unsigned char>(*start))) {
        ++start;
    }

    // Trim trailing whitespace
    std::string::const_iterator end = str.end();
    do {
        --end;
    } while (end != start && std::isspace(static_cast<unsigned char>(*end)));

    // If the entire string is whitespace, return an empty string
    if (start == str.end()) {
        return "";
    } else {
        return std::string(start, end + 1);
    }
}


std::vector<std::string> splitString(const std::string& str) {
    std::vector<std::string> result;
    if(str.empty())
        return result;
    std::istringstream iss(str);
    std::string token;

    while (iss >> token) {
        result.push_back(token);
    }

    return result;
}

void send_error_message(int fd, std::string message)
{
    std::string color = RED + message  + RESET;
    send(fd, color.c_str(),color.length() , 0);
}
bool check_mode(std::map<int, Client> client, int fd)
{
    std::string option[10] = {"+i" , "-i", "+l", "-l",\
     "+k", "-k", "+o", "-o", "+t", "-t"};
    int size = client[fd].arg.size();
    if(size >= 2 && (client[fd].arg[1] == "+o" || client[fd].arg[1] == "-o" \
    || client[fd].arg[1] == "+l" || client[fd].arg[1] == "+k" ))
    {
    if(size > 4 || size < 4)
        return(send_error_message(fd, "error in setings of command\n"), false);
    if(client[fd].arg[1] == "+l")
    {
        std::string str = client[fd].arg[3];
        for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        if (!isdigit(*it)) {
            return(send_error_message(fd, "SHOULD ENTER DIGITS\n") , false); // If any character is not a digit, return false
        }
        }
    long int num = std::atol(str.c_str());
    if(num > INT32_MAX)
        return(send_error_message(fd, "long number\n"), false);
    }
    return(true);
    }
    if(client[fd].arg.size() > 3)
        return(send_error_message(fd, "TO MANY ARGUMENT\n"), false);
    if(client[fd].arg.size() < 3)
        return(send_error_message(fd, "NOT ENOUGH ARGUMENT\n") , false);
    for(int i = 0; i < 11; i++)
    {
        if(option[i] == client[fd].arg[1])
                return(true);
    }
    return (send_error_message(fd, "OPTION NOT FOUND\n") ,false);
}
bool check_dcc(int fd, std::map<int , Client> &client)
{
    int size = client[fd].arg.size();
    if(size > 4)
        return(send_error_message(fd, "TO MANY ARGUMENT\n") , false);
    else if (size < 4)
        return(send_error_message(fd, "NOT ENOUGH ARGUMENT\n"), false);
    else if(client[fd].arg[1] != "send")
        return(send_error_message(fd, "INVALIDE ARGUMENT , USAGE /dcc send [user] [file path]\n"), false);
    int fd_recev  = fd_ofuser(client[fd].arg[2], client);
    if(fd_recev >= 0)
    {
        client[fd_recev].authfile = true;
        client[fd_recev].filepath = client[fd].arg[3];
        notification_user(fd_recev, " \nuser @" + client[fd].username +  " want to send you a file do you want receive it yes/no:");
    }
    if(fd_recev < 0)
        return(send_error_message(fd, "User not found\n"), false);
    return true;
}
bool check_cmd(int fd, std::map<int , Client> &client)
{
    if(client[fd].arg.empty())
        return(false);
    if(client[fd].arg.size() == 1 && client[fd].authfile && \
    (client[fd].arg[0] == "yes" || client[fd].arg[0] == "no"))
        return true;
    std::string cmds[11] = {"/send", "/join", "/kick", "/invite", "/topic", "/mode",  \
    "/user", "/nick", "/help", "/dcc", "/privmsg"};
    bool check = true;
    for(int i = 0; i < 11; i++)
    {
    if(client[fd].arg[0] == "/dcc")
        return(check_dcc(fd, client));
    if(client[fd].arg[0] == "/mode")
        return (check_mode(client, fd));
    if(client[fd].arg[0] == "/join")
    {
        if(client[fd].arg.size() > 3)
            return(send_error_message(fd, "TO MANY ARGUMENT\n"), false);
        else if(client[fd].arg.size() < 2)
            return(send_error_message(fd, "NOT ENOUGH ARGUMENT\n"), false);
        return (true);
    }
    else if(cmds[i] == client[fd].arg[0])
    {
    if(client[fd].arg.size() > 3  && client[fd].arg[0] != "/mode")
       return(send_error_message(fd, "TO MANY ARGUMENT\n"), false);
    else if(client[fd].arg.size() < 3 && client[fd].arg[0] != "/user" && client[fd].arg[0] != "/nick" && client[fd].arg[0] != "/help")
        return(send_error_message(fd, "NOT ENOUGH ARGUMENT\n"), false);
    check = false;
    break;
    }
    }
    if(check)
        return(send_error_message(fd, "COMMAND NOT FOUND\n"), false);
    return true;
}