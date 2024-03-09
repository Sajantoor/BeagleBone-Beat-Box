#include <iostream>
#include <socket.hpp>

// Inspired by:
// https://www.geeksforgeeks.org/udp-server-client-implementation-c/

Socket::Socket(void) {
    // Create a new IPv4 UDP socket
    int socketFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketFd == -1) {
        std::cerr << "Failed to create socket" << std::endl;
    }

    struct sockaddr_in serverAddress;

    serverAddress.sin_family = AF_INET;  // IPv4
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    // Bind the socket to the port
    if (bind(socketFd, (struct sockaddr*)&serverAddress,
             sizeof(serverAddress)) == -1) {
        std::cerr << "Failed to bind socket" << std::endl;
    }

    this->socketFd = socketFd;
}

void Socket::closeSocket(void) {
    isRecieving = false;
    close(this->socketFd);
}

UdpMessage* Socket::receive(void) {
    // Receive a message from the socket
    char buffer[BUFFER_SIZE];
    sockaddr_in clientAddress;
    socklen_t clientAddressLen = sizeof(clientAddress);

    int bytesRead =
        recvfrom(this->socketFd, buffer, sizeof(buffer) - 1, 0,
                 (struct sockaddr*)&clientAddress, &clientAddressLen);

    // Fix potential buffer overflow
    buffer[BUFFER_SIZE - 1] = '\0';

    if (bytesRead == -1) {
        std::cerr << "Failed to receive message" << std::endl;
        return nullptr;
    }

    // get the sender information
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddress.sin_addr), ip, INET_ADDRSTRLEN);

    // Convert the message to a string
    std::string message(buffer, bytesRead);
    return new UdpMessage(message, ip, ntohs(clientAddress.sin_port));
}

void Socket::send(UdpMessage* message) {
    // Get the address of the server
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(message->getPort());
    inet_pton(AF_INET, message->getIp().c_str(), &serverAddress.sin_addr);

    // Send a message to the server
    int bytesSent =
        sendto(this->socketFd, message->getMessage().c_str(),
               message->getMessage().size(), 0,
               (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    if (bytesSent == -1) {
        std::cerr << "Failed to send message" << std::endl;
    }
}

bool Socket::getIsRecieving(void) { return isRecieving; }

void Socket::stopRecieving(void) { this->isRecieving = false; }
