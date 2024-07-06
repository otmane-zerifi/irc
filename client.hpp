#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <iostream>
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
    std::string cmd[2];
    std::string username;
    std::string nickname;
};
void parss_data(int fd, std::map<int, Client> &client, std::string pass);
int fd_ofuser(std::string username, std::map<int, Client> client);
void send_message(int fd, std::map<int , Client> client);
std::string get_user(const std::string& cmd);
void trim(std::string& str);
std::vector<std::string> splitString(const std::string& str);
#endif 