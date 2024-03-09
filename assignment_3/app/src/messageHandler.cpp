#include "messageHandler.hpp"

MessageHandler::MessageHandler(Socket* socket)
    : socket(socket) {}

void MessageHandler::init(void) { handleUDPMessages(); }


void MessageHandler::handleUDPMessages(void) {
    while (true) {
        UdpMessage* message = socket->receive();
        std::string messageString = message->getMessage();

        // Remove newline character
        messageString = messageString.substr(0, messageString.find("\n"));

        delete message;
    }
}
