#ifndef HOST_HPP
#define HOST_HPP

#include <SFML/Network.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>

#include "world.hpp"

const unsigned PORT_NUMBER = 4446;
const unsigned DATA_STRING_BUFFER_SIZE = 5000;
//this class name is awful. What is this struct *actually* doing?

class GameHost;

struct ClientCommandInstance {
    public:
    sf::TcpSocket socket;
    std::string parseCommand(char* data, unsigned bytesOfDataReceived) {
        if( bytesOfDataReceived == 0 ) {
            return "";
        }
        std::cout << "parsing a command!\n";
        std::string command;
        data[bytesOfDataReceived];
        command = data;
        return command;
    }
};

class HostSideThreadEncapsulation {
    private:
    World world;
    bool notStopped;
    std::mutex stopMtx;
    std::mutex worldMtx;
    protected:
    HostSideThreadEncapsulation(): world() {
        notStopped = true;
    }
    std::string handleCommand(std::string commandString, bool serverSide);
    bool hostNotStopped() {
        stopMtx.lock();
        bool wasItStopped = notStopped;
        stopMtx.unlock();
        return wasItStopped;
    }
    void stopHost() {
        stopMtx.lock();
        notStopped = false;
        stopMtx.unlock();
    }

public:
    static std::string runHostListenThread(GameHost* host);
    static void runTurnListenThread(GameHost* host);
};

class GameHost: public HostSideThreadEncapsulation {
public:
    GameHost(): HostSideThreadEncapsulation() {
        std::cout << "Starting a new U235 server!\n";
    }
    std::string runHost();
};

#endif
