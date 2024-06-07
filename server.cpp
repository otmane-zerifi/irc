#include "server.hpp"

int main()
{
    // Step 1: Create a socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Step 2: Set socket options
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1)
    {
        perror("setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Step 3: Define the address structure
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Bind to any available interface
    address.sin_port = htons(6667);       // port number

    // Step 4: Bind the socket to the address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1)
    {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Step 5: Listen for incoming connections
    if (listen(server_fd, 3) == -1)
    {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server is listening on port 6667" << std::endl;

    // Step 6: Accept incoming connections (in a loop, typically)
    while (true)
    {
        struct sockaddr_in client_address;
        socklen_t client_address_len = sizeof(client_address);
        int new_socket = accept(server_fd, (struct sockaddr *)&client_address, &client_address_len);
        if (new_socket == -1)
        {
            perror("accept failed");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        // Handle the client connection
        std::cout << "Accepted a connection" << std::endl;

        // Optional: Send a welcome message to the client
        const char *welcome_message = "Welcome to the server!\n";
        send(new_socket, welcome_message, strlen(welcome_message), 0);

        // Close the new socket after handling the client
        close(new_socket);
    }

    close(server_fd);
    return EXIT_SUCCESS;
}
