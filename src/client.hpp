#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <fstream>

#include "buttonImplementations.hpp"
#include "gui.hpp"
#include "stringdataparser.hpp"
#include <thread>
#include <mutex>
//mainly for constants like PORT_NUMBER and DATA_STRING_BUFFER_SIZE, this is a fairly questionable #include to be frank as host and client should be as decoupled from each other as possible
#include "host.hpp"
#include "messagestrings.hpp"
#include <cmath>
#include <exception>

const float PING_COOLDOWN_TIME_IN_SECONDS = 5.0f;
const float ISLAND_SPACE_RATIO = 0.7f;
const float PI = 3.141592653589;
const int ATTEMPT_NUMBER_THRESHOLD = 2;
const std::string LAST_HOST_IP_FILENAME = "lasthostip.txt";
const std::string LAST_FACTION_CODE_FILENAME = "lastfactioncode.txt";
const float ISLAND_NAME_TEXT_FONT_SIZE = 10.0f;
const float ISLAND_NAME_TEXT_HOVER_HEIGHT = 25.0f;
const float FONT_SIZE_TO_CHARACTER_SIZE_RATIO = 0.4f;
const float UNIT_BUTTON_TOP_LEFT_COORDINATE_X = 10.0f;
const float UNIT_BUTTON_TOP_LEFT_COORDINATE_Y = 10.0f;
const float UNIT_TEXT_FONT_SIZE = 20.0f;
const float UNIT_BUTTON_HEIGHT = 20.0f;
const float UNIT_BUTTON_WIDTH = 100.0f;

enum UnitType {
    REGIMENT,
    BATTLESHIP,
    SPY
};

class Client;

struct ClientWorld;

std::string getStringFromFile(std::string fileName);
//returns true if successful, false if not
bool writeStringToFile(std::string fileName, std::string dataString);

//void clientListenThread(Client* client);

struct CommandResponse {
    std::string commmand;
    std::string response;
    unsigned id;
};
//a bit odd name, but honestly, this seems to be really descriptive to the purpose of  this class
class ClientSideThreadEncapsulation {
    unsigned nextCommandId;
    //configmtx will be dealing with all the data regarding the client socket handling
    //these options should not be getting lots of use time from the main client thread and as such, should be fine to bundle together in one mutex for the sake of simplicity
    std::mutex configMtx;
    std::mutex pendingCommandMtx;
    std::mutex commandRespondMtx;

    std::string targetIp;
    bool subThreadActive;
    bool subThreadKilled;

    //first is the command, second is command id
    std::list<std::pair<std::string, int>> pendingCommands;
    std::list<CommandResponse> commandResponses;
    bool handlePendingCommand();
protected:

    ClientSideThreadEncapsulation() {
        nextCommandId = 0;
        subThreadActive = false;
        subThreadKilled = false;
        targetIp.clear();
    }
    //returns false if there are no CommandResponses left
    bool fetchNextCommandResponse( CommandResponse& response );

    void setSubThreadActiveState(bool state) {
        configMtx.lock();
        subThreadActive = state;
        configMtx.unlock();
    }
    void killSubThread() {
        configMtx.lock();
        subThreadKilled = true;
        configMtx.unlock();
    }
    bool sendCommandToHost(std::string command, int attempNumber);
public:
    bool subThreadIsKilled() {
        bool returnBool;
        configMtx.lock();
        returnBool = subThreadKilled;
        configMtx.unlock();
        return returnBool;
    }
    std::string getTargetIp() {
        configMtx.lock();
        std::string returnString = targetIp;
        configMtx.unlock();
        return returnString;
    }
    void setTargetIp(std::string ip) {
        configMtx.lock();
        targetIp = ip;
        configMtx.unlock();
    }
    bool subThreadIsActive() {
        bool returnBool;
        configMtx.lock();
        returnBool = subThreadActive;
        configMtx.unlock();
        return returnBool;
    }
    void addPendingCommand(const std::string& commandString, int id) {
        pendingCommandMtx.lock();
        std::pair<std::string, int> command;
        command.first = commandString;
        command.second = id;
        pendingCommands.push_back(command);
        std::cout << "command '" << command.first << "' added to pending commands\n";
        pendingCommandMtx.unlock();
    }
    static void clientListenThread(Client* client);
};


struct ClientIsland;

struct ClientFaction {
    std::string name;
    ClientIsland* baseIsland;
    sf::Color color;
};

struct ClientUnit {
    std::string name;
    std::string clientName;
    float condition;
    ClientFaction* faction;
    ClientIsland* targetIsland;
    UnitType type;
};
struct ClientIsland {
    std::list<ClientIsland*> linkedIslands;
    std::list<ClientUnit> units;
    ClientFaction* owner;
    std::string name;
};

struct ClientRim {
    std::list<ClientIsland*> islands;
};

struct ClientWorld: public WorldData {
    std::list<ClientRim> rims;
    std::list<ClientFaction> factions;
    std::list<ClientIsland> islands;
    std::list<std::pair<ClientFaction*, ClientFaction*>> treatyRequests;
    std::list<std::pair<ClientFaction*, ClientFaction*>> peaceTreaties;
    std::list<std::pair<ClientFaction*, ClientFaction*>> warDeclarations;
    //this has all the world data and getter functions, but it behaves quite differently from the server side world, as the client world is supposed to show only actions that reflect
    //the player's decisions
    ClientWorld(std::string& worldString) {
        refreshFromString(worldString);
    }
    ClientWorld() {

    }
    //this function creates a client-side representation of the world from a string that can be sent through the internet
    void refreshFromString(std::string& worldString);
    ClientFaction* getFactionFromName(std::string name) {
        for(auto it = factions.begin(); it != factions.end(); it++) {
            if(name == (*it).name) {
                return &(*it);
            }
        }
        return nullptr;
    }
    ClientIsland* getIslandFromName(std::string name) {
        for(auto it = islands.begin(); it != islands.end(); it++) {
            if( (*it).name == name ) {
                return &(*it);
            }
        }
        return nullptr;
    }
    void debugPrint();
};

class IslandButton: public purriGUI::Interactable {
protected:
    sf::Text nameText;
    ClientIsland* island;
    Client* client;

public:
    IslandButton(sf::CircleShape shape, Client* _client, purriGUI::GUI* _hud, ClientIsland* _island, purriGUI::SignalListener* slot = nullptr): Interactable(shape, _hud, slot) {
        island = _island;
        client = _client;
        setClickingExclusiveSelectsState(true);
        //init nameText
        if(_island == NULL) {
            std::cerr << "Error in IslandButton contsrutctor: NULL pointer was inserted into this function\n";
            return;
        }
        nameText.setString(_island->name);
        nameText.setCharacterSize(ISLAND_NAME_TEXT_FONT_SIZE);
        nameText.setFillColor(sf::Color::Black);
        nameText.setFont(*(_hud->getFont()));
        nameText.setPosition(shape.getPosition().x, shape.getPosition().y);
        nameText.setOrigin(_island->name.size()*ISLAND_NAME_TEXT_FONT_SIZE*FONT_SIZE_TO_CHARACTER_SIZE_RATIO/2, ISLAND_NAME_TEXT_HOVER_HEIGHT);
    }
    ClientIsland* getClientIsland() {
        return island;
    }
    virtual ~IslandButton(){}
    static IslandButton* createIslandButton(purriGUI::GUI& hud, Client* _client, sf::CircleShape shape, ClientIsland* _island, purriGUI::SignalListener* slot = nullptr) {
        IslandButton* islandButton = new IslandButton(shape, _client, &hud, _island, slot);
        hud.addInteractableToList((Interactable*)islandButton);
        return islandButton;
    }
    virtual void onSelection();
    virtual void loopChildFunctionality(sf::RenderWindow& window) {
        window.draw(nameText);
    }
};

class UnitButton: public purriGUI::Button {
public:
    ClientUnit* unit;
    UnitButton(sf::RectangleShape shape, purriGUI::GUI* _hud, sf::Text _buttonText, ClientUnit* _unit, purriGUI::SignalListener* slot = nullptr): Button(shape, _hud, _buttonText, slot) {
        std::cout << "creating new UnitButton\n";
        unit = _unit;
        setClickingExclusiveSelectsState(false);
        setDeselectingTriggers(true);
        setSelectingTriggers(true);
        setXorSelectMode(true);
    }
    virtual ~UnitButton() {
        std::cout << "deleting UnitButton\n";
    }
    static UnitButton* createUnitButton(purriGUI::GUI* gui, sf::RectangleShape shape, sf::Text _buttonText, ClientUnit* _unit, purriGUI::SignalListener* slot = nullptr) {
        UnitButton* unitButton = new UnitButton(shape, gui, _buttonText, _unit, slot);
        gui->addInteractableToList((Interactable*)unitButton);
        return unitButton;
    }
};

class IslandListener: public purriGUI::SignalListener {
    IslandButton* islandButton;
public:
    IslandListener(IslandButton* _button) {
        islandButton = _button;
    }
    virtual ~IslandListener() {}
    static IslandListener* createIslandListener(purriGUI::GUI& hud, IslandButton* _button) {
        IslandListener* listener = new IslandListener(_button);
        hud.addListenerToList((SignalListener*)listener);
        return listener;
    }
    virtual void onTrigger() {

    }
};

class Client: public ClientSideThreadEncapsulation {
private:
    std::string factionCode;
    ClientWorld world;
    std::list<IslandButton*> islandButtons;
    std::list<IslandListener*> islandListeners;
    std::list<sf::Drawable*> primitiveDrawables;
    std::list<UnitButton*> unitButtons;
    purriGUI::TextField* factionNameField;
    purriGUI::TextField* hostIpField;
    purriGUI::GUI gui;
    sf::Time lastPing;
    bool pingHost;
public:
    Client(sf::Font* font, float bordersSize): gui(font, bordersSize) {
        factionNameField = nullptr;
        hostIpField = nullptr;
    }
    void runClient();
    //this function will be called by the main thread, not the side thread
    void handleCommandResponses();
    IslandButton* getIslandButtonFromClientIsland(ClientIsland* island);
    bool showUnitButtonsOfIsland(ClientIsland* clientIsland);
    //this function makes sure the unit buttons listed in the client will be freed from the gui also
    bool freeUnitButtons();
    bool attemptMoveSelectedUnitsToIsland(ClientIsland* targetIsland);
private:
    void handleClientNonFactionCommandResponses(CommandResponse response);
    void handleClientFactionCommandResponses(CommandResponse response);
    void updateClientWorld(std::string& worldString);
    void updateIslandGui(sf::Rect<int> screenSpace);
    //returns false, if no unit button was in selected state, returns true otherwise
};

class CommandSenderListener: public purriGUI::SignalListener {
    sf::Text* commandText;
    Client* client;
public:
    CommandSenderListener( sf::Text* _commandText, Client* _client ) {
        commandText = _commandText;
        client = _client;
    }
    virtual ~CommandSenderListener() {}
    static CommandSenderListener* createCommandSenderListener(purriGUI::GUI& hud, sf::Text* _commandText, Client* _client) {
        CommandSenderListener* listener = new CommandSenderListener(_commandText, _client);
        hud.addListenerToList((SignalListener*)listener);
        return listener;
    }
    virtual void onTrigger() {
        if(commandText == nullptr || client == nullptr) {
            return;
        }
        std::cout << "pending command added by CommandSenderListener\n";
        client->addPendingCommand(commandText->getString().toAnsiString(), 0);
    }
};

#endif
