#ifndef CLIENT_HPP
#define CLIENT_HPP
#include "channel.h"
#include <iostream>
#include <ctime>
#include <sstream>
#include <vector>
#include <string>
#include <map>

class Client {
    public :
    int fd;
    char buffer[1024];
    bool auth;
    std::vector<std::string> arg;
    std::string buff;
    std::string username;
    std::string nickname;
};

bool isValidnick(int fd, std::string nickname);
std::string getTimestamp();
bool isValidChar(char c);
bool isValiduser(const std::string& user);
void parss_data(int fd, std::map<int, Client> &client, std::string pass , std::map<std::string, Chanel> &chanels);
int fd_ofuser(std::string username, std::map<int, Client> client);
void send_message(int fd, std::map<int , Client> client);
std::string get_user(const std::string& cmd);
void trim(std::string& str);
std::vector<std::string> splitString(const std::string& str);
#endif 
//rules << "Nickname rules:\n";
//rules << "1. Must be between 1 and 9 characters long.\n";
//rules << "2. May contain letters (A-Z, a-z), digits (0-9), and special characters.\n";
//rules << "3. Allowed special characters are: - [ ] \\ _ ^ { }.\n";
//rules << "4. The first character must be a letter or an allowed special character.\n";