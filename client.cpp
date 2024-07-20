#include "server.h"
#include "client.hpp"

bool isValidChar(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || 
           c == '-' || c == '[' || c == ']' || 
           c == '\\' || c == '_' || c == '^' || 
           c == '{' || c == '}';
}

// Validate the nickname
bool isValiduser(const std::string& user) {
    if (user.empty() || user.size() > 9) {
        return false;
    }

    // First character must be a letter or special character
    if (!std::isalpha(static_cast<unsigned char>(user[0])) &&
        user[0] != '-' && user[0] != '[' && user[0] != ']' && 
        user[0] != '\\' && user[0] != '_' && user[0] != '^' && 
        user[0] != '{' && user[0] != '}') {
        return false;
    }

    // All characters must be valid
    for (std::string::const_iterator it = user.begin(); it != user.end(); ++it) {
        if (!isValidChar(*it)) {
            return false;
        }
    }

    return true;
}

bool double_user(int fd, std::map<int, Client> &clients) {

        std::string username = clients[fd].buff;
        if(!isValiduser(username))
        {
        send(fd, "INVALIDE USERNAME\nENTER VALID USERNAME:", 40, 0);
            return false;
        }
        for (std::map<int, Client>::iterator jt = clients.begin(); jt != clients.end(); ++jt) {
            if (username == jt->second.username) {
                send(fd, "EXIST USER" , 11 , 0);
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
   std::string message = "message from @" + client[fd].arg[1]  + " :" + get_message(client[fd].buffer);
   if(fd_user < 0)
        send(fd, "USER NOT FOUND\n", 16, 0);
   else if(message.empty())
        send(fd, "NO MESSAGE\n", 12, 0);
   else
        send(fd_user, message.c_str(), message.length(), 0);
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

std::string getTimestamp() {
    char timestamp[9]; // HH:MM:SS + '\0'
    std::time_t now = std::time(0);
    std::strftime(timestamp, sizeof(timestamp), "%H:%M:%S", std::localtime(&now));
    return timestamp;
}

void send_command_table(int client_fd, std::string user) {
    std::ostringstream oss;

    // Header
    oss << "COMMAND        | DESCRIPTION                            | USAGE\n";
    oss << "---------------|----------------------------------------|---------------------------\n";

    // Command Table Rows
    oss << BLUE <<      "USER           " << RESET << "| set username                        " << RESET << "   | USER (username)     \n";
    oss << WHITE <<      "NICK           " << RESET << "| set nickname                           " << RESET << "| NICK (nickname)     \n";
    oss << BRIGHT_RED << "JOIN           " << RESET << "| Join a channel                        " << RESET << " | join #channel / join #channel #password\n";
    oss << BRIGHT_RED << "KICK           " << RESET << "| Eject a client from the channel       " << RESET << " | kick #username #channel\n";
    oss << BRIGHT_GREEN << "INVITE         " << RESET << "| Invite a client to a channel          " << RESET << " | invite #username #channel\n";
    oss << BRIGHT_YELLOW << "TOPIC          " << RESET << "| Change or view the channel topic      " << RESET << " | topic #channel [new topic]\n";
    oss << BRIGHT_BLUE << "MODE           " << RESET << "| Change the channel’s mode             " << RESET << " | mode [mode] #channel\n";
    oss << BRIGHT_MAGENTA << "MODE +i        " << RESET << "| Set Invite-only channel               " << RESET << " | mode +i #channel\n";
    oss << BRIGHT_CYAN << "MODE -i        " << RESET << "| Remove Invite-only channel            " << RESET << " | mode -i #channel\n";
    oss << WHITE << "MODE +t        " << RESET << "| Restrict TOPIC command                " << RESET << " | mode +t  #channel\n";
    oss << WHITE << "MODE -t        " << RESET << "| Allow TOPIC command                   " << RESET << " | mode -t  #channel\n";
    oss << RED << "MODE +k        " << RESET << "| Set channel key (password)            " << RESET << " | mode +k #channel <password>\n";
    oss << RED << "MODE -k        " << RESET << "| Remove channel key (password)         " << RESET << " | mode -k #channel\n";
    oss << GREEN << "MODE +o        " << RESET << "| Give channel operator privilege       " << RESET << " | mode +o <username> #channel\n";
    oss << GREEN << "MODE -o        " << RESET << "| Take channel operator privilege       " << RESET << " | mode -o <username> #channel\n";
    oss << BRIGHT_BLUE << "MODE +l        " << RESET << "| Set user limit to channel             " << RESET << " | mode +l #channel <number of user>\n";
    oss << BRIGHT_BLUE << "MODE -l        " << RESET << "| Remove user limit to channel          " << RESET << " | mode -l #channel\n";
    oss << BRIGHT_GREEN << "SEND           " << RESET << "| Send message to other client          " << RESET << " | username/channel : your message\n";
    std::string message = oss.str();
    send(client_fd, message.c_str(), message.size(), 0);
}
bool isValidnick(int fd, std::string nickname)
{
    if(!isValiduser(nickname))
    {
        send(fd, "INVALIDE NICKNAME\n", 19,0);
        return(false);
    }
    return true;
}

void parss_data(int fd, std::map<int,Client> &client, std::string password, std::map<std::string, Chanel> &chanels)
{
    (void) chanels;
    char *buffer = client[fd].buffer;
    std::string buff = client[fd].buff;
    if(!client[fd].auth && buff != password)
        send(fd, "\033[31mINCORRECT PASSWORD\033[0m\nENTRE PASSWORD : ", 46, 0);
    else if(!client[fd].auth && buff == password)
    {
        client[fd].auth = true;
        send(fd, "\033[1;32mWELCOM TO IRC SERVER\nENTER YOUR USERNAME:", 49, 0);
    }
    else if(client[fd].auth && client[fd].username.empty() && *buffer != 0 && double_user(fd, client))
    {
        client[fd].username = client[fd].buff;
        send(fd, "\033[1;32mENTER YOUR NIKENAME:", 28 , 0);
    }
    else if(client[fd].auth && client[fd].nickname.empty() && *buffer != 0 && !client[fd].username.empty() && isValidnick(fd, buff))
    {
        client[fd].nickname = client[fd].buff;
        send(fd, "Welcom, for manual enter /help\n", 31 , 0);
    }
    else if(!client[fd].username.empty() && !client[fd].nickname.empty())
    {
        std::string cmd;
        client[fd].arg = splitString(buff);
        if(!client[fd].arg.empty())
            cmd = client[fd].arg[0];
        bool check = check_cmd(fd, client);
        if (cmd == "SEND" && check)
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
            // problem ui on send on client ui 
        }
        else if(cmd == "/help")
            send_command_table(fd, client[fd].username);
        else if(cmd == "USER")
        {
            int nb = client[fd].arg.size();
            if(nb > 2)
                send(fd, "TO MANY ARGUMENT\n", 18,0);
            else if(nb < 2)
                send(fd, "NOT ENOUGH ARGUMENT\n", 21, 0);
            else
                client[fd].username = client[fd].arg[1];
        }
        else if(cmd == "NICK" && check)
        {
            int nb = client[fd].arg.size();
            if(nb > 2)
                send(fd, "TO MANY ARGUMENT\n", 18,0);
            else if(nb < 2)
                send(fd, "NOT ENOUGH ARGUMENT\n", 21, 0);
            else
                client[fd].nickname = client[fd].arg[1];
        }
        else if(cmd == "JOIN" && check)
        {
            std::string pass = "";
            if (client[fd].arg.size() > 2)
                pass = client[fd].arg[2];
            join_user(client[fd].arg[1], chanels, fd, pass, client);
        }
        else if(cmd == "KICK" && check)
        {
            Kick_manager(fd, client[fd].arg[1], client[fd].arg[2], \
            chanels, client);
        }
        else if(cmd == "INVITE" && check)
        {
            Invite_manager(fd, client[fd].arg[1], client[fd].arg[2], client, chanels);
        }
        else  if(cmd == "TOPIC" && check)
        {
            topic_manager(fd, client[fd].arg[2], client[fd].arg[1], client, chanels);
        }
        else if(cmd == "MODE" && check)
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
        std::string str= getTimestamp() + " @" + client[fd].username + " :";
        send(fd, str.c_str(), str.length(), 0);
    }
}