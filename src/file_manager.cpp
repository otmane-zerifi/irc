# include "server.h"
# include "channel.h"
# include "client.hpp"

void   file_transfer(int sender, std::string reciever, std::string path, std::map<int, Client> &server_users)
{
    std::string msg;

    if (Check_UserOnServer(server_users, convert_name_to_fd(reciever, server_users)))
    {
        std::vector<char> buffer(1024);
        std::ifstream file (path.c_str());

        if (!file.is_open())
        {
            msg = RED "Path not found\n" RESET;
            return ((void)(send(sender, msg.c_str(), msg.length(), 0)));
        }
        while (file)
        {
            file.read(buffer.data(), 1024);
            std::streamsize byte = file.gcount();
            if (byte > 0)
                send(convert_name_to_fd(reciever, server_users), buffer.data(), byte, 0);
        }
        file.close();
    }
    else
    {
        msg = RED "User not found\n" RESET;
        return ((void)(send(sender, msg.c_str(), msg.length(), 0)));
    }
}