#ifndef SERVER_HPP
#define SERVER_HPP
#include <iostream>
#include <cstring>      // For memset
#include <cstdlib>      // For exit, EXIT_FAILURE
#include <unistd.h>     // For close
#include <sys/types.h>  // For socket, bind, listen, accept
#include <sys/socket.h> // For socket, bind, listen, accept
#include <netinet/in.h> // For sockaddr_in
#include <arpa/inet.h>  // For htons
#endif