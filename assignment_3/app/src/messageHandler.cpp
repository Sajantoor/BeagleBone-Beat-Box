#include "messageHandler.hpp"

MessageHandler::MessageHandler(Socket* socket, GenerateBeat* generatebeat,
                               Shutdown* shutdown)
    : socket(socket), generateBeat(generatebeat), shutdown(shutdown) {}

void MessageHandler::init(void) { handleUDPMessages(); }

void MessageHandler::handleChangeMode(std::string mode) {
    // modes are "rock" and "custom"
    // modes are NONE, ROCK_DRUM, CUSTOM, NUM_BEATS
    if (mode == "rock") {
        generateBeat->setBeatType(ROCK_DRUM);
    } else if (mode == "custom") {
        generateBeat->setBeatType(CUSTOM);
    } else if (mode == "none") {
        generateBeat->setBeatType(NONE);
    } else {
        std::cout << "Invalid mode: " << mode << std::endl;
    }
}

void MessageHandler::handleChangeVolume(std::string volume) {
    if (volume == "+") {
        generateBeat->increaseVolume();
    } else if (volume == "-") {
        generateBeat->decreaseVolume();
    } else {
        std::cout << "Invalid volume: " << volume << std::endl;
    }
}

void MessageHandler::handleChangeTempo(std::string tempo) {
    if (tempo == "+") {
        generateBeat->increaseTempo();
    } else if (tempo == "-") {
        generateBeat->decreaseTempo();
    } else {
        std::cout << "Invalid tempo: " << tempo << std::endl;
    }
}

void MessageHandler::handlePlaySound(std::string sound) {
    // sounds are snare, baseDrum, hiHat
    if (sound == "snare") {
        generateBeat->playSnare();
    } else if (sound == "baseDrum") {
        generateBeat->playBaseDrum();
    } else if (sound == "hiHat") {
        generateBeat->playHiHat();
    } else {
        std::cout << "Invalid sound: " << sound << std::endl;
    }
}

void MessageHandler::handleShutdown(void) {
    shutdown->performShutdown();
    socket->closeSocket();
}

void MessageHandler::handleUDPMessages(void) {
    while (!shutdown->isShutdown()) {
        UdpMessage* message = socket->receive();
        std::string messageString = message->getMessage();

        // Remove newline character
        messageString = messageString.substr(0, messageString.find("\n"));
        // split messageString into command and argument
        std::string command = messageString.substr(0, messageString.find(" "));
        std::string argument =
            messageString.substr(messageString.find(" ") + 1);

        // changeMode [mode]
        if (command == "changeMode") {
            handleChangeMode(argument);
        }

        // changeVolume [+/-]
        else if (command == "changeVolume") {
            handleChangeVolume(argument);
        }

        // changeTempo [+/-]
        else if (command == "changeTempo") {
            handleChangeTempo(argument);
        }

        // playSound [sound]
        else if (command == "playSound") {
            handlePlaySound(argument);
        }

        // shutdown
        else if (command == "shutdown") {
            handleShutdown();
        } else {
            std::cout << "Invalid message: " << messageString << std::endl;
        }

        delete message;
    }
}
