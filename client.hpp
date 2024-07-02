#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <map>
class Client {
    public :
    int fd;
    char buffer[1024];
    bool auth;
    std::string cmd[2];
    std::string username;
    std::string nickname;
};
void parss_data(int fd, std::map<int, Client> &client);
int fd_ofuser(std::string username, std::map<int, Client> client);
void send_message(int fd, std::map<int , Client> client);
std::string get_user(const std::string& cmd);
#endif 