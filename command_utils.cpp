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

bool check_mode(std::map<int, Client> client, int fd)
{
    std::string option[10] = {"+i" , "-i", "+l", "-l",\
     "+k", "-k", "+o", "-o", "+t", "-t"};
    int size = client[fd].arg.size();
    if(size >= 2 && (client[fd].arg[1] == "+o" || client[fd].arg[1] == "-o" \
    || client[fd].arg[1] == "+l" || client[fd].arg[1] == "+k" ))
    {
    if(size > 4 || size < 4)
       return(send(fd, "error in setings of command\n", 29, 0), false);
    if(client[fd].arg[1] == "+l")
    {
        std::string str = client[fd].arg[3];
        for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        if (!isdigit(*it)) {
            return(send(fd, "SHOULD ENTER DIGITS\n", 21,0) , false); // If any character is not a digit, return false
        }
        }
    long int num = std::atol(str.c_str());
    if(num > INT32_MAX)
        return(send(fd, "long number\n", 13, 0), false);
    }
    return(true);
    }
    if(client[fd].arg.size() > 3)
        return(send(fd, "TO MANY ARGUMENT\n", 18, 0), false);
    if(client[fd].arg.size() < 3)
        return(send(fd, "NOT ENOUGH ARGUMENT\n", 21, 0) , false);
    for(int i = 0; i < 11; i++)
    {
        if(option[i] == client[fd].arg[1])
                return(true);
    }
    return (send(fd, "OPTION NOT FOUND\n",18, 0) ,false);
}

bool check_cmd(int fd, std::map<int , Client> &client)
{
    if(client[fd].arg.empty())
        return(false);
    std::string cmds[9] = {"SEND", "JOIN", "KICK", "INVITE", "TOPIC", "MODE", "USER", "NICK", "/help"};
    bool check =true;
    for(int i =0; i < 9; i++)
    {
    if(client[fd].arg[0] == "MODE")
        return (check_mode(client, fd));
    if(client[fd].arg[0] == "JOIN")
    {
        if(client[fd].arg.size() > 3)
            return(send(fd, "TO MANY ARGUMENT\n" , 18, 0), false);
        else if(client[fd].arg.size() < 2)
            return(send(fd, "NOT ENOUGH ARGUMENT\n", 21, 0), false);
        return (true);
    }
    else if(cmds[i] == client[fd].arg[0])
    {
    if(client[fd].arg.size() > 3 && client[fd].arg[0] != "SEND" && client[fd].arg[0] != "MODE")
       return(send(fd, "TO MANY ARGUMENT\n" , 18, 0), false);
    else if(client[fd].arg.size() < 3 && client[fd].arg[0] != "USER" && client[fd].arg[0] != "NICK" && client[fd].arg[0] != "/help")
        return(send(fd, "NOT ENOUGH ARGUMENT\n", 21, 0), false);
    check = false;
    break;
    }
    }
    if(check)
        return(send(fd, "COMMAND NOT FOUND\n", 19, 0), false);
    return true;
}