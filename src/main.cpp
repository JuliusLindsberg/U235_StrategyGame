//#include <SFML/Network.hpp>
#include <iostream>
#include "world.hpp"
#include "host.hpp"
#include "client.hpp"
//#include "names.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "messagestrings.hpp"

const std::string VERSION = "Pre-Alpha";

int debugMode = 0;

int random(int R) {
	int randomz = 0;
	randomz = rand() % R + 1;
	return randomz;
	//seed init(laita alla oleva rivi ohjelman alkupuolille)
	//srand ( time(NULL) );
}


int main()
{
    srand ( time(NULL) );
    //std::string factionNames[] = { "SOVIET_UNION","USA","KEKKOSLANDIA","CHINA", "NORTH-KOREA", "OSD", "LOLOLOL", "ZOMG", "DEBUGNAME", "JAPAN", "KOREA", "FRANCE", "GREAT-BRITANNIA", "UKRAINE" };
    //int rimIslandAdditionValues[] = { 2, -1, 2, -2 };
    std::cout << "Welcome to U235, the cold war-ish island strategy game!\nThis build is version " << VERSION << ".\n";
    int breakCounter = 10;

    //World world = World(/*5, factionNames, 3, 2, 1, 2, 2, 1, "Atolls"*/);


        /*std::cout << "Please enter a command! 'q' or 'Q' means quit\n     ";
        std::string command;
        getline(std::cin, command);
        if( command == "q" || command == "Q" ) {
            break;
        }
        std::string commandMessage = world.handleCommand(command);
        std::cout << "\nCommand effect: '" << commandMessage << "'\n";
        if( commandMessage == "GAME SHUTDOWN FROM INPUT" ) {
            break;
        }
    }*/
    while(true) {
        std::cout << "\n\n\n ---Would you like to host a server(s or S) or be a client(c or C) or offline debug mode(d or D)?---\n";
        char userInput = 0;
        std::cin >> userInput;
        if(userInput == 's' || userInput == 'S') {
            //Initiate server
            GameHost host;
            host.runHost();
            break;
        }
        else if(userInput == 'c' || userInput == 'C') {
            //Initiate client
            sf::Font ubuntuFont;
            if(!ubuntuFont.loadFromFile("resources/UbuntuMono-R.ttf")) {
                std::cerr << "Error: font resources/UbuntuMono-R.ttf failed to load!\n";
            }
            Client client(&ubuntuFont, 10);
            client.runClient();
            break;
        }
        else if(userInput == 'd' || userInput == 'D') {
            World world;
            while(true) {
                std::cout << "Please enter a command! 'q' or 'Q' means quit\n     ";
                std::string command;
                command.clear();
                getline(std::cin, command);
                if( command == "q" || command == "Q" ) {
                    break;
                }
                std::string commandMessage = world.handleCommand(command, true);
                std::cout << "\nCommand effect: '" << commandMessage << "'\n";
                if( commandMessage == MSG::GAME_SHUTDOWN_FROM_INPUT ) {
                    break;
                }
            }
            break;
        }
        else {
            breakCounter--;
            if(breakCounter == 0) {
                std::cout << "U235 closed\n";
                return 0;
            }
            std::cout << "Your answer was not understood. Please try again!(will ask for " << breakCounter << " more times before closing)\n";
        }
    }
    return 0;
}
