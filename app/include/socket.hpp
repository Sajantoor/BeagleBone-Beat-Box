
#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <string>
#include <thread>

static constexpr const int BUFFER_SIZE = 1024;
static constexpr const int PORT = 12345;

/**
 * Represents a message that is sent or recieved from the UDP socket.
 * Contains the message, the ip address of the sender and the port of the
 * sender.
 */
class UdpMessage {
   public:
    UdpMessage(std::string message, std::string ip, unsigned int port) {
        this->message = message;
        this->ip = ip;
        this->port = port;
    }

    std::string getMessage(void) { return message; }
    std::string getIp(void) { return ip; }
    unsigned int getPort(void) { return port; }
    void setMessage(std::string message) { this->message = message; }

   private:
    std::string message;
    std::string ip;
    unsigned int port;
};

/*
 Handles the UDP socket implementation
*/
class Socket {
   public:
    Socket(void);
    /**
     * CLoses the socket and stops the recieving thread.
     */
    void closeSocket(void);
    /**
     * Recieves a message from the UDP socket. UdpMessage is allocated using new
     * and must be freed by the caller.
     */
    UdpMessage* receive();
    /**
     * Sends a message
     */
    void send(UdpMessage* message);
    /**
     * Returns true if the socket is currently recieving messages.
     */
    bool getIsRecieving(void);
    /**
     * Stops recieving messages from the socket.
     */
    void stopRecieving(void);

    /**
     * Sends a message to the web server
     */
    void sendMessageToWebServer(std::string message);

   private:
    int socketFd;
    bool isRecieving;
    static Socket* instance;
};

#endif
