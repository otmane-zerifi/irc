#include "client.hpp"

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

bool double_user(int fd, std::map<int, Client> &clients, std::map<std::string , Chanel> &chanels) {

        std::string username = clients[fd].buff;
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
        }
        for(std::map<std::string , Chanel>::iterator it = chanels.begin(); it != chanels.end(); ++it)
        {
            if(username == it->first)
            {
                send(fd, "IS A CHANEL NAME\n", 18, 0);
            return (false);
            }
        }
        std::cerr << "user not found\n";
    return true; // No duplicate usernames found
}

std::string getTimestamp() {
    char timestamp[9]; // HH:MM:SS + '\0'
    std::time_t now = std::time(0);
    std::strftime(timestamp, sizeof(timestamp), "%H:%M:%S", std::localtime(&now));
    return timestamp;
}