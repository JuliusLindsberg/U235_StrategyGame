#include "world.hpp"
#include "messagestrings.hpp"
#include <SFML/Graphics.hpp>

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
        return MSG::GAME_ALREADY_IN_PROGRESS;
    }
    else if(factionName.size() < 3) {
        return MSG::FACTION_NAME_TOO_SHORT;
    }
    else if(factionName.size() > 100) {
        return MSG::FACTION_NAME_TOO_LONG;
    }
    else {
        //create a random faction code and make sure this code is unique
        std::string factionCode;
        factionCode.clear();
        while(true) {
            for(unsigned i = 0; i < FACTION_CODE_LENGTH; i++) {
                //create a new random faction code. If the created code is already in use, try again
                //47-59 are the digits 0 to 9 in ascii
                factionCode.push_back((char)(47+random(10)));
            }
            bool newTry = false;
            //if there exists an faction with the same code, discard the code
            for(std::list<Faction>::iterator it = factions.begin(); it != factions.end(); it++) {
                if((*it).name == factionName) {
                    return MSG::FACTION_NAME_ALREADY_USED;
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
        return MSG::GAME_ALREADY_IN_PROGRESS;
    }
    std::string factionCode = commandString.substr(2);
    //if the given factionCode matches with one of the unverified factions, verify that faction
    for(unsigned i = 0; i < unverifiedFactions.size(); i++) {
        if( unverifiedFactions[i].factionCode == factionCode ) {
            Faction newFaction(unverifiedFactions[i].name);
            newFaction.setFactionCode(factionCode);
            newFaction.setColor(sf::Color( random(255), random(255), random(255)).toInteger() );
            factions.push_back(newFaction);
            std::vector<UnverifiedFaction>::iterator erasable = unverifiedFactions.begin();// = unverifiedFactions.at(i);
            erasable+=i;
            unverifiedFactions.erase(erasable);
            return "";
        }
    }
    return MSG::INVALID_FACTION_CODE;
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
                return MSG::NO_FACTIONS_HAVE_JOINED_GAME;
            }
            std::string factionList;
            factionList.clear();
            for(std::list<Faction>::iterator it = factions.begin(); it != factions.end(); it++) {
                factionList = factionList + (*it).name + ", ";
            }
            return factionList;
        }
    case 'p':
        {
            if(isStarted()) {
                return std::to_string(getTurnNumber());
            }
            else {
                return MSG::LOBBY;
            }
        }
    case 'h':
        {
            return "#j<faction name> join game, #v<faction code> verify faction join, #l return list of factions joined, #h this help";
        }
    default:
        return MSG::NON_FACTION_COMMAND_WAS_NOT_RECOGNICED;
    };
}

std::string World::handleServerCommand(std::string commandString) {
    if(commandString.size() < 2) {
        return MSG::INVALID_SERVER_COMMAND_STRING;
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
        return "Server commands!\n\n !h Display this help\n!s<rimAmount(unsigned)>:<islandness(int)>:<falloutTime(int)>:<baseRegimentSupply(float)>:<baseBattleshipSupply(float)>:<spyAmount(unsigned)>:<regimentRecoveryRate(int)>:<shipRecoveryRate(int)>:<WorldName(string)>:<turnTimeInterval(float as seconds)> Start the game\n!t end the turn and start a new one\n!d show the game situation debug info";
    }
case 's':
    {
        //create a standard kind of world with default arguments
        if(arguments.size() == 0 || arguments.size() == 1) {
            std::string returnMessage = "";
            returnMessage = startWorld(5, 3, 1, 0.3, 0.3, 2, 0.3, 0.2, "Atolls", 30.0f);
            if(returnMessage == "") {
                return MSG::DEFAULT_WORLD_CREATED;
            }
            else {
                return returnMessage;
            }
        }
        if(arguments.size() != 10) {
            return MSG::INVALID_AMOUNT_OF_ARGUMENTS_IN_COMMAND;
        }
        std::string returnMessage = "";
        try {
        returnMessage = startWorld((unsigned)std::stoi(arguments[0]), std::stoi(arguments[1]), std::stoi(arguments[2]), std::stof(arguments[3]), std::stof(arguments[4]),
                   (unsigned)std::stoi(arguments[5]), std::stoi(arguments[6]), std::stoi(arguments[7]), arguments[8], std::stof(arguments[9]));
        }
        catch(std::invalid_argument){
            return MSG::WRONG_KIND_OF_ARGUMENTS_IN_COMMAND;
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
        return MSG::GAME_SHUTDOWN_FROM_INPUT;
    }
default:
    {
        return MSG::SERVER_COMMAND_WAS_NOT_RECOGNISED;
    }
    };
    return "";
}

//parses the command and then excecutes it. Returns error strings that should be forwarded to clients in case of errors
std::string World::handleCommand(std::string commandString, bool serverSide) {
    std::cout << "handling server command!";
    if(commandString.size() == 0) {
        return MSG::EMPTY_STRING;
    }
    //some commands should not have faction-code as requirement. The #(char) is for those
    if( commandString[0] == '#' ) {
        if( commandString.size() == 1 ) {
            return MSG::NON_FACTION_COMMAND_SYNTAX_WAS_INVALID;
        }
        return handleNonFactionCommand(commandString);
    }
    else if(commandString[0] == '!') {
        if(!serverSide) { return MSG::UNAUTHORIZED; }
        return handleServerCommand(commandString);
    }
    std::vector<std::string> arguments;
    std::string command, factionCode;
    std::string returnString = parseFactionCommand(commandString, arguments, command, factionCode);
    if( returnString != "" ) {
        return returnString;
    }

    return executeCommand(factionCode, command, arguments);
}

std::string World::factionTreatyRequest(std::string factionCode, std::vector<std::string> arguments) {
    //argument[0] is the target faction the command sender faction wants to be in peace with
    if(arguments.size() != 1) {
        return MSG::WRONG_AMOUNT_OF_ARGUMENTS;
    }
    Faction* subjectFaction = getFactionFromCode(factionCode);
    Faction* objectFaction = getFactionFromName(arguments[0]);
    if(subjectFaction->isAllyOf(objectFaction)) {
        return MSG::FACTIONS_ARE_ALREADY_ALLIES;
    }
    if( subjectFaction == objectFaction ) {
        return MSG::FACTION_ARGUMENTS_GIVEN_WERE_THE_SAME_FACTION;
    }
    return addTreatyRequest(subjectFaction, objectFaction);
    //in order to succesfully make a peace treaty, both factions have to send treaty command to the server with the other faction as the target faction
    //after that, a peace treaty will take place in the next turn
}

std::string World::declareWar(std::string factionCode, std::vector<std::string> arguments) {
    if(arguments.size() != 1) {
        return MSG::WRONG_AMOUNT_OF_ARGUMENTS;
    }
    Faction* subject = getFactionFromCode(factionCode);
    Faction* object = getFactionFromName(arguments[0]);
    if( subject == NULL ) {
        return MSG::FACTION_CODE_WAS_INVALID;
    }
    if( object == NULL ) {
        return MSG::FACTION_WAS_NOT_FOUND;
    }
    if(subject == object) {
        return MSG::FACTION_ARGUMENTS_GIVEN_WERE_THE_SAME_FACTION;
    }
    if( isDeclaringWar(subject, object) ) {
        return MSG::WAR_ALREADY_DECLARED;
    }
    if(!subject->isAllyOf(object)) {
        return MSG::THE_FACTIONS_ARE_ALREADY_AT_WAR;
    }

    warDeclarations.push_back(std::pair<Faction*, Faction*> (subject, object));
    return "";
}

std::string World::getFactionWorldString(std::string factionCode, std::vector<std::string> arguments) {
    if(arguments.size() != 0) {
        return MSG::THIS_FUNCTION_TAKES_NO_ARGUMENTS;
    }
    Faction* faction = getFactionFromCode(factionCode);
    if(faction == NULL) {
        return MSG::FACTION_CODE_WAS_INVALID;
    }
    std::string returnString;
    worldInStringFormatForFaction(returnString, faction);

    return returnString;
}

//this function has to first make sure the command was valid(has correct faction code in the beginning of the command and then assure given move is viable for the faction)
//then change the state of the game according to that command
std::string World::executeCommand(std::string factionCode, std::string command, std::vector<std::string> arguments) {
    if(!checkFactionCodeValidity(factionCode)) {
        return MSG::FACTION_CODE_WAS_INVALID;
    }
    if(command == "move") {
        return moveTroop(factionCode, arguments);
    }
    else if(command == "treaty") {
        return factionTreatyRequest(factionCode, arguments);
    }
    else if(command == "war") {
        //naturally, declaring war is unilateral: as the saying goes, wars begin when you will but will not end when you please
        return declareWar(factionCode, arguments);
    }
    else if( command == "gamestate" ) {
        return getFactionWorldString(factionCode, arguments);
    }
    else {
        return MSG::UNKNOWN_COMMAND;
    }
}
