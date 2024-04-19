#ifndef _MESSAGE_HANDLER_HPP_
#define _MESSAGE_HANDLER_HPP_

#include <iostream>

#include "generateBeat.hpp"
#include "shutdown.hpp"
#include "socket.hpp"

/**
 Responsible for handling the messages received from the
 UDP socket and calling the appropriate functions in the Sampler class.
 Also responsible for calling the shutdown funciton.
*/
class MessageHandler {
   public:
    MessageHandler(Socket* socket, GenerateBeat* generateBeat,
                   Shutdown* shutdown);
    /**
     * Starts the message handler on a new thread. This function will only shut
     * down once it receives a stop message from the UDP socket.
     */
    void init(void);

   private:
    Socket* socket;
    GenerateBeat* generateBeat;
    Shutdown* shutdown;

    void handleUDPMessages(void);
    void handleChangeMode(std::string mode);
    void handleChangeVolume(std::string volume);
    void handleChangeTempo(std::string tempo);
    void handlePlaySound(std::string sound);
    void handleShutdown(void);
};

#endif
