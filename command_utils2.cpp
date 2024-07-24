#include "client.hpp"

void send_command_table(int client_fd, std::string user) {
    std::ostringstream oss;

    // Header
    oss << "COMMAND        | DESCRIPTION                            | USAGE\n";
    oss << "---------------|----------------------------------------|---------------------------\n";

    // Command Table Rows
    oss << BLUE <<      "/user          " << RESET << "| set username                        " << RESET << "   | /user (username)     \n";
    oss << WHITE <<      "/nick          " << RESET << "| set nickname                           " << RESET << "| /nick(nickname)     \n";
    oss << BRIGHT_RED << "/join          " << RESET << "| Join a channel                        " << RESET << " | /join #channel | /join #channel #password\n";
    oss << BRIGHT_RED << "/kick          " << RESET << "| Eject a client from the channel       " << RESET << " | /kick #username #channel\n";
    oss << BRIGHT_GREEN << "/invite        " << RESET << "| Invite a client to a channel          " << RESET << " | /invite #username #channel\n";
    oss << BRIGHT_YELLOW << "/topic         " << RESET << "| Change or view the channel topic      " << RESET << " | /topic #channel [new topic]\n";
    oss << BRIGHT_BLUE << "/mode          " << RESET << "| Change the channel’s mode             " << RESET << " | /mode [mode] #channel\n";
    oss << BRIGHT_MAGENTA << "/mode +i       " << RESET << "| Set Invite-only channel               " << RESET << " | /mode +i #channel\n";
    oss << BRIGHT_CYAN << "/mode -i       " << RESET << "| Remove Invite-only channel            " << RESET << " | /mode -i #channel\n";
    oss << WHITE << "/mode +t       " << RESET << "| Restrict TOPIC command                " << RESET << " | /mode +t  #channel\n";
    oss << WHITE << "/mode -t       " << RESET << "| Allow TOPIC command                   " << RESET << " | /mode -t  #channel\n";
    oss << RED << "/mode +k       " << RESET << "| Set channel key (password)            " << RESET << " | /mode +k #channel <password>\n";
    oss << RED << "/mode -k       " << RESET << "| Remove channel key (password)         " << RESET << " | /mode -k #channel\n";
    oss << GREEN << "/mode +o       " << RESET << "| Give channel operator privilege       " << RESET << " | /mode +o <username> #channel\n";
    oss << GREEN << "/mode -o       " << RESET << "| Take channel operator privilege       " << RESET << " | /mode -o <username> #channel\n";
    oss << BRIGHT_BLUE << "/mode +l       " << RESET << "| Set user limit to channel             " << RESET << " | /mode +l #channel <number of user>\n";
    oss << BRIGHT_BLUE << "/mode -l       " << RESET << "| Remove user limit to channel          " << RESET << " | /mode -l #channel\n";
    oss << BRIGHT_GREEN << "/send          " << RESET << "| Send message to other client          " << RESET << " | /send username|channel  (your message)\n";
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
            send_error_message(fd, "INVALIDE USERNAME\n\033[35mENTER VALID USERNAME:");
            return false;
        }
        for (std::map<int, Client>::iterator jt = clients.begin(); jt != clients.end(); ++jt) {
            if (username == jt->second.username) {
                send_error_message(fd, "EXIST USER\n");
                return false;
            }
        }
        for(std::map<std::string , Chanel>::iterator it = chanels.begin(); it != chanels.end(); ++it)
        {
            if(username == it->first)
            {
                send_error_message(fd, "IS A CHANEL NAME\n");
            return (false);
            }
        }
    return true; // No duplicate usernames found
}

std::string getTimestamp() {
    char timestamp[9]; // HH:MM:SS + '\0'
    std::time_t now = std::time(0);
    std::strftime(timestamp, sizeof(timestamp), "%H:%M:%S", std::localtime(&now));
    return timestamp;
}