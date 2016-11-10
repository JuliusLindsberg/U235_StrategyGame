#include <SFML/Network.hpp>
#include <iostream>
#include "world.hpp"
//#include "names.hpp"

const std::string VERSION = "Alpha";

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
    std::cout << "Welcome to U235, the cold war-ish island strategy game!\nThis host build is version " << VERSION << ".\n";
    //int breakCounter = 10;

    World world = World(/*5, factionNames, 3, 2, 1, 2, 2, 1, "Atolls"*/);

    while(true) {
        std::cout << "Please enter a command! 'q' or 'Q' means quit\n     ";
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
    }
    /*while(true) {
        std::cout << "\n\n\n ---Would you like to host a server(s or S) or be a client(c or C)?---\n";
        char userInput = 0;
        std::cin >> userInput;
        if(userInput == 's' || userInput == 'S') {
            //Initiate server
            int playerAmount;
            std::cout << "Starting a new match. For how many players should the new match be?\n";
            std::cin >> playerAmount;

            break;
        }
        else if(userInput == 'c' || userInput == 'C') {
            //Initiate client
        }
        else {
            breakCounter--;
            if(breakCounter == 0) {
                std::cout << "U235 closed\n";
                return 0;
            }
            std::cout << "Your answer was not understood. Please try again!(will ask for " << breakCounter << " more times before closing)\n";
        }
    }*/
    return 0;
}
