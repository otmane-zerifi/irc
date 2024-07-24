#ifndef CLIENT_HPP
#define CLIENT_HPP
#include "channel.h"
#include "server.h"
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
void send_error_message(int fd, std::string message);
bool isValidnick(int fd, std::string nickname);
bool check_cmd(int fd, std::map<int , Client> &client);
void send_command_table(int client_fd, std::string user);
std::string trim(const std::string &str);
bool double_user(int fd, std::map<int, Client> &clients, std::map<std::string , Chanel> &chanels);
std::string getTimestamp();
bool isValidChar(char c);
bool isValiduser(const std::string& user);
void send_command(int fd, std::map<int,Client>& client, std::map<std::string, Chanel>& chanels);
int fd_ofuser(std::string username, std::map<int, Client> client);
void send_message(int fd, std::map<int , Client> client);
void user_command(int fd,std::map<int,Client>& client, std::map<std::string, Chanel>& chanels);
void nick_command(int fd, std::map<int,Client>& client);
void handle_command(int fd, std::map<int,Client>& client, std::map<std::string, Chanel>& chanels);
void option_mode(int fd, std::map<int,Client>& client, std::map<std::string, Chanel>& chanels);
void join_command(int fd, std::map<int,Client>& client, std::map<std::string, Chanel>& chanels);
std::string get_user(const std::string& cmd);
void mode_command(int fd, std::map<int,Client>& client, std::map<std::string, Chanel>& chanels);
void trim(std::string& str);
void authentication(int fd, std::map<int , Client> &client, std::string& password);
void set_username(int fd, std::map<int,Client>& client, std::map<std::string, Chanel>& chanels);
void set_nickname(int fd,  std::map<int,Client>& client);
std::vector<std::string> splitString(const std::string& str);
void parss_data(int fd, std::map<int, Client>& client,  std::string& password, std::map<std::string, Chanel>& chanels);
bool check_user(int fd, std::map<int , Client> &clients, std::map<std::string , Chanel>& chanels);
#endif 
//rules << "Nickname rules:\n";
//rules << "1. Must be between 1 and 9 characters long.\n";
//rules << "2. May contain letters (A-Z, a-z), digits (0-9), and special characters.\n";
//rules << "3. Allowed special characters are: - [ ] \\ _ ^ { }.\n";
//rules << "4. The first character must be a letter or an allowed special character.\n";