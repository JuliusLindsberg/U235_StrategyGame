#include "world.hpp"


bool World::checkFactionCodeValidity(std::string factionCode) {
    for(std::list<Faction>::iterator it = factions.begin(); it != factions.end(); it++) {
        if(factionCode == (*it).getFactionCode()) {
            return true;
        }
    }
    return false;
}

Faction* World::getFactionFromCode(std::string factionCode) {
    for(std::list<Faction>::iterator it = factions.begin(); it != factions.end(); it++) {
        if(factionCode == (*it).getFactionCode()) {
            return &(*it);
        }
    }
    return NULL;
}

std::string World::addFaction(std::string commandString) {
    std::string factionName = commandString.substr(2);
    if(isStarted()) {
        return "GAME ALREADY IN PROGRESS";
    }
    else if(factionName.size() < 3) {
        return "FACTION NAME TOO SHORT";
    }
    else {
        //create a random faction code and make sure this code is unique
        std::string factionCode;
        while(true) {
            factionCode.clear();
            for(unsigned i = 0; i < 4; i++) {
                //create a new random faction code. If the created code is already in use, try again
                //47-59 are the digits 0 to 9 in ascii
                factionCode.push_back((char)(47+random(10)));
            }
            bool newTry = false;
            //if there exists an faction with the same code, discard the code
            for(std::list<Faction>::iterator it = factions.begin(); it != factions.end(); it++) {
                if((*it).name == factionName) {
                    return "FACTION NAME ALREDY USED";
                }

                if((*it).factionCode == factionCode) {
                    newTry = true;
                }
            }
            //if there exists an unverified faction with the same code, discard the code
            for(unsigned i = 0; i < unverifiedFactions.size(); i++) {
                if(unverifiedFactions[i].factionCode == factionCode) {
                    newTry = true;
                }
            }
            if(!newTry) {
                break;
            }
        }
        UnverifiedFaction newUnverified;
        newUnverified.factionCode = factionCode;
        newUnverified.name = factionName;
        unverifiedFactions.push_back(newUnverified);
        return factionCode;
    }
}

std::string World::verifyFaction(std::string commandString) {
    if(isStarted()) {
        return "GAME ALREADY IN PROGRESS";
    }
    std::string factionCode = commandString.substr(2);
    //if the given factionCode matches with one of the unverified factions, verify that faction
    for(unsigned i = 0; i < unverifiedFactions.size(); i++) {
        if( unverifiedFactions[i].factionCode == factionCode ) {
            Faction newFaction(unverifiedFactions[i].name);
            newFaction.setFactionCode(factionCode);
            factions.push_back(newFaction);
            std::vector<UnverifiedFaction>::iterator erasable = unverifiedFactions.begin();// = unverifiedFactions.at(i);
            erasable+=i;
            unverifiedFactions.erase(erasable);
            return "";
        }
    }
    return "INVALID FACTION CODE";
}

std::string World::handleNonFactionCommand(std::string commandString) {
    char type = commandString[1];
    switch(type) {
        //'j' join game
    case 'j':
        {
            return addFaction(commandString);
        };
    //'v' verify joining (by demanding a verification for a game join the host can be sure that the faction code has been recieved by the client)
    case 'v':
        {
            return verifyFaction(commandString);
        };
    //returns a list of present verified factions as a string
    case 'l':
        {
            if(factions.size() == 0) {
                return "NO FACTIONS HAVE JOINED GAME";
            }
            std::string factionList;
            factionList.clear();
            for(std::list<Faction>::iterator it = factions.begin(); it != factions.end(); it++) {
                factionList = factionList + (*it).name + ", ";
            }
            return factionList;
        }
    case 'h':
        {
            return "#j<faction name> join game, #v<faction code> verify faction join, #l return list of factions joined, #h this help";
        }
    default:
        return "NON-FACTION COMMAND WAS NOT RECOGNISED";
    };
}

std::string World::handleServerCommand(std::string commandString) {
    if(commandString.size() < 2) {
        return "INVALID SERVER COMMAND STRING";
    }
    char commandType = commandString[1];
    std::vector<std::string> arguments;
    commandString = commandString.substr(2);
    if(commandString.find_first_of(':') >= 0) {
        while(true) {
            int newPoint = commandString.find_first_of(':');
            if(newPoint < 0) {
                arguments.push_back(commandString.substr(0));
                break;
            }
            arguments.push_back(commandString.substr(0, newPoint));
            commandString = commandString.substr(newPoint+1);
        }
    }
    switch(commandType) {
case 'h':
    {
        return "Server commands!\n\n !h Display this help\n!s<rimAmount(unsigned)>:<islandness(int)>:<falloutTime(int)>:<baseRegimentSupply(float)>:<baseBattleshipSupply(float)>:<spyAmount(unsigned)>:<regimentRecoveryRate(int)>:<shipRecoveryRate(int)>:<WorldName(string)> Start the game\n!t end the turn and start a new one\n!d show the game situation debug info";
    }
case 's':
    {
        //create a standard kind of world with default arguments
        if(arguments.size() == 0 || arguments.size() == 1) {
            std::string returnMessage = "";
            returnMessage = startWorld(5, 3, 1, 0.3, 0.3, 2, 0.3, 0.2, "Atolls");
            if(returnMessage == "") {
                return "DEFAULT WORLD CREATED";
            }
            else {
                return returnMessage;
            }
        }
        if(arguments.size() != 9) {
            return "INVALID AMOUNT OF ARGUMENTS IN COMMAND";
        }
        std::string returnMessage = "";
        try {
        returnMessage = startWorld((unsigned)std::stoi(arguments[0]), std::stoi(arguments[1]), std::stoi(arguments[2]), std::stof(arguments[3]), std::stof(arguments[4]),
                   (unsigned)std::stoi(arguments[5]), std::stoi(arguments[6]), std::stoi(arguments[7]), arguments[8]);
        }
        catch(std::invalid_argument){
            return "WRONG KIND OF ARGUMENTS IN COMMAND";
        }
        return returnMessage;
    }
case 'd':
    {
        debugPrint();
        return "";
    }
case 't':
    {
        return endTurn();
    }
case 'q':
    {
        return "GAME SHUTDOWN FROM INPUT";
    }
default:
    {
        return "SERVER COMMAND WAS NOT RECOGNISED";
    }
    };
    return "";
}

//parses the command and then excecutes it. Returns error strings that should be forwarded to clients in case of errors
std::string World::handleCommand(std::string commandString) {
    if(commandString.size() == 0) {
        return "EMPTY STRING";
    }
    //some commands should not have faction-code as requirement. The #(char) is for those
    if( commandString[0] == '#' ) {
        if( commandString.size() == 1 ) {
            return "NON-FACTION COMMAND SYNTAX WAS INVALID";
        }
        return handleNonFactionCommand(commandString);
    }
    else if(commandString[0] == '!') {
        return handleServerCommand(commandString);
    }
    std::vector<std::string> arguments;
    arguments.clear();
    //pick up the faction code part from the command string
    int firstPoint = commandString.find_first_of(':');
    if(firstPoint < 0) {
        return "COMMAND SYNTAX WAS INVALID";
    }
    std::string factionCode = commandString.substr(0, firstPoint);
    commandString = commandString.substr(firstPoint+1);
    //pick up the command name from the command string
    int secondPoint = commandString.find_first_of(':');
    if(secondPoint < 0) {
        return "COMMAND SYNTAX WAS INVALID";
    }
    std::string command = commandString.substr(0, secondPoint);
    commandString = commandString.substr(secondPoint+1);
    //pick up all the following arguments from the command string
    while(true) {
        //ARGUMENT SEPARATOR WILL BE CHANGED INTO CHARACTER \n IN LATER DEVELOPMENT WHEN I'M NOT USING CONSOLES FOR SENDING MESSAGES
        int nextPoint = commandString.find_first_of('%');
        if(nextPoint < 0) {
            break;
        }
        arguments.push_back(commandString.substr(0, nextPoint));
        commandString = commandString.substr(nextPoint+1);
    }
    return executeCommand(factionCode, command, arguments);
}

std::string World::factionTreatyRequest(std::string factionCode, std::vector<std::string> arguments) {
    //argument[0] is the target faction the command sender faction wants to be in peace with
    if(arguments.size() != 1) {
        return "WRONG AMOUNT OF ARGUMENTS";
    }
    Faction* subjectFaction = getFactionFromCode(factionCode);
    Faction* objectFaction = getFactionFromName(arguments[0]);
    if(subjectFaction->isAllyOf(objectFaction)) {
        return "FACTIONS ARE ALREADY ALLIES";
    }
    if( subjectFaction == objectFaction ) {
        return "FACTION ARGUMENTS GIVEN WERE THE SAME FACTION";
    }
    return addTreatyRequest(subjectFaction, objectFaction);
    //in order to succesfully make a peace treaty, both factions have to send treaty command to the server with the other faction as the target faction
    //after that, a peace treaty will take place in the next turn
}

//this function has to first make sure the command was valid(has correct faction code in the beginning of the command and then assure given move is viable for the faction)
//then change the state of the game according to that command
std::string World::executeCommand(std::string factionCode, std::string command, std::vector<std::string> arguments) {
    if(!checkFactionCodeValidity(factionCode)) {
        return "FACTION CODE WAS NOT VALID";
    }
    if(command == "move") {
        return moveTroop(factionCode, arguments);
    }
    else if(command == "treaty") {
        return factionTreatyRequest(factionCode, arguments);
    }
    else if(command == "war") {
        //naturally, declaring war is unilateral: as the saying goes, wars begin when you will but will not end when you please
    }
    else {
        return "UNKNOWN COMMAND";
    }
}
