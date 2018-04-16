
#include "host.hpp"

std::string HostSideThreadEncapsulation::handleCommand(std::string commandString, bool serverSide) {
    worldMtx.lock();
    //std::cout << "worldMtx locked\n";
    std::string returnString = world.handleCommand(commandString, serverSide);
    //std::cout << "handlecommand handled\n";
    worldMtx.unlock();
    //std::cout << "worldMtx unlocked\n";
    return returnString;
}

std::string GameHost::runHost() {
    std::string userInput;
    std::cout << "Input a server command here:";
    std::thread hostListenThread ( runHostListenThread, this );
    std::thread turnListenThred( runTurnListenThread, this );
    while(hostNotStopped()) {
        std::cin >> userInput;
        std::cout << "Command response: '" << handleCommand(userInput, true) << "'\n";
    }
    return MSG::HOST_HAS_QUIT;
}

std::string HostSideThreadEncapsulation::runHostListenThread(GameHost* host) {
    sf::TcpListener listener;

    if( listener.listen(PORT_NUMBER) != sf::Socket::Done ) {
        std::cerr << "Error in GameHost::runHostThread(): tcpListener was unable to listen to port " << PORT_NUMBER << ".\n";
    }
    ClientCommandInstance client;
    while(host->hostNotStopped()) {
        if( listener.accept(client.socket) != sf::Socket::Done ) {
            std::cerr << "Error in GameHost::runHostThread(): tcpListener failed to accept a connection.\n";
        }
        //std::cout << "connection accepted\n";
        unsigned bytesOfDataRecieved = 0;
        char* data = new char[DATA_STRING_BUFFER_SIZE];
        for(unsigned i = 0; i < DATA_STRING_BUFFER_SIZE; i++) {
            data[i] = 0;
        }
        client.socket.receive(data, DATA_STRING_BUFFER_SIZE, bytesOfDataRecieved);
        std::string commandString = client.parseCommand(data, bytesOfDataRecieved);
        std::string returnString = host->handleCommand(commandString, false);
        delete data;
        //char* dataToSend = new char[returnString.size()+1];
        std::cout << "CommandString: " << commandString << "\n";
        client.socket.send( returnString.c_str(), returnString.size()+1 );
        client.socket.disconnect();
    }
    //host stop request made by rest of the program, shut down server normally
    return MSG::SERVER_SHUT_DOWN_ON_PROGRAM_REQUEST;
}

void HostSideThreadEncapsulation::runTurnListenThread(GameHost* host)
{
    while(host->hostNotStopped())
    {
        host->worldMtx.lock();
        if( host->world.getTimeLeft() < sf::seconds(0.0f) )
        {
            //this mutex implementation is a bit funny: as handleCommand() locks the worldMtx it has to be called with worldMtx unlocked
            host->worldMtx.unlock();
            host->handleCommand("!t", true);
            host->worldMtx.lock();
        }
        host->worldMtx.unlock();
        sf::sleep(sf::milliseconds(5));
    }
}
