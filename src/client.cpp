
#include "client.hpp"

std::string getStringFromFile(std::string filename) {
    std::cout << "fetching string from file " << filename << "\n";
    std::fstream fs;
    std::string dataString = "";
    fs.open(filename, std::ios_base::in);
    if(fs.is_open()) {
        fs >> dataString;
        fs.close();
    }
    else {
        return "";
    }
    return dataString;
}

bool writeStringToFile(std::string fileName, std::string dataString) {
    std::cout << "writing string " << dataString << " to file " << fileName << "\n";
    std::fstream fs;
    fs.open(fileName, std::ios_base::out);
    if(fs.is_open()) {
        fs << dataString;
        fs.close();
    }
    else {
        return false;
    }
    return true;
}

void ClientSideThreadEncapsulation::clientListenThread(Client* client) {
    client->setSubThreadActiveState(true);
    while(!client->subThreadIsKilled()) {
        if(client->subThreadIsActive()) {
            client->handlePendingCommand();
        }
        //brief sleep is useful for ensuring that this thread does not block the other threads for a long time when waiting for commands
        sf::sleep(sf::milliseconds(2));
    }
}

void Client::updateClientWorld(std::string& worldString) {
    std::cout << "updating client world\n";
    world.refreshFromString(worldString);
    sf::Rect<int> rect = sf::Rect<int>(sf::Vector2i(50, 50), sf::Vector2i(550, 550));
    std::cout << "Rect values: top " << rect.top << " left: " << rect.left << " width: " << rect.width << " height: " << rect.height << "\n";
    updateIslandGui(rect);
}
//all the islands are put inside the screenSpace given
void Client::updateIslandGui(sf::Rect<int> screenSpace) {
    std::cout << "updating island GUI\n";
    for(auto it = islandButtons.begin(); it != islandButtons.end(); it++) {
        gui.freeInteractable(*it);
    }
    for(auto it = islandListeners.begin(); it != islandListeners.end(); it++) {
        gui.freeInteractable((purriGUI::Interactable*)*it);
    }
    for(auto it = primitiveDrawables.begin(); it != primitiveDrawables.end(); it++) {
        gui.freeDrawable(*it);
    }
    freeUnitButtons();
    islandButtons.clear();
    islandListeners.clear();
    //old island buttons and listeners have been deleted: time to create the new ones!
    sf::Vector2i origo = sf::Vector2i( screenSpace.left+screenSpace.width/2, screenSpace.top+screenSpace.height/2 );
    sf::Vector2i anchorPoint = sf::Vector2i(screenSpace.left, screenSpace.top+screenSpace.height/2);
    int rimCounter = 0;
    for(auto it = world.rims.begin(); it != world.rims.end(); it++) {
        if((*it).islands.size() == 0) { std::cerr << "Error in Client::updateIslandGui(): The worlds islands were badly formed, containing rims with no islands in them!\n"; continue; }
        float alpha = 2*PI/(float)(*it).islands.size();
        //beta determines the position of the first island in the rim
        float beta = 0;
        int islandCounter = 0;
        //create a faint visual circle to make it easier to recognice which island belongs to which rim
        if(world.rims.size() != rimCounter+1) {
            sf::CircleShape* rimCircle = new sf::CircleShape;
            float radius = (float)origo.x-anchorPoint.x;
            rimCircle->setFillColor(sf::Color(0, 0, 0, 20));
            rimCircle->setOrigin(radius, radius);
            rimCircle->setPosition(origo.x, origo.y);
            rimCircle->setOutlineColor(sf::Color(192, 192, 192, 70));
            rimCircle->setRadius(radius);
            primitiveDrawables.push_back(rimCircle);
            std::pair<purriGUI::DrawableData, sf::Drawable*> primitiveData(purriGUI::DrawableData(), rimCircle);
            gui.addDrawableToList( primitiveData );
        }
        //create visual representations to the islands in their respective positions
        for(auto ct = (*it).islands.begin(); ct != (*it).islands.end(); ct++) {
            sf::Vector2f position;
            std::cout << "anchorPoint.x: " << anchorPoint.x << "anchorPoint.y: " << anchorPoint.y << "\n";
            position.x =  origo.x + (origo.x-anchorPoint.x)*cos(beta+alpha*islandCounter);
            position.y = origo.y + (origo.x-anchorPoint.x)*sin(beta+alpha*islandCounter);
            std::cout << "position x: " << position.x << " position y: " << position.y << "\n";
            if(world.rims.size() == rimCounter+1) {
                position.x = origo.x;
                position.y = origo.y;
            }
            islandCounter++;
            //we have the position, create buttons!
            sf::CircleShape shape;
            shape.setPosition(position);
            shape.setRadius(10.0f);
            shape.setFillColor(sf::Color(50, 0, 00, 200));
            if( (*ct)->owner != nullptr )
            {
                shape.setFillColor((*ct)->owner->color);
            }
            shape.setOutlineColor(sf::Color::Black);
            shape.setOutlineThickness(1.0f);
            shape.setOrigin(10.0f, 10.0f);
            islandButtons.push_back( IslandButton::createIslandButton(gui, this, shape, *ct));
        }
        std::cout << "screenSpace.left: " << screenSpace.left;
        std::cout << "world rims size: " << world.rims.size() << "\n";
        anchorPoint.x +=  (origo.x-screenSpace.left)/world.rims.size();
        std::cout << anchorPoint.x << " <- anchorpoint\n";
        rimCounter++;
    }
    //now that all the buttons have been created, make visual representations in the form of lines from button to button regarding which islands are linked with each other
    for(auto it = islandButtons.begin(); it != islandButtons.end(); it++) {
        for(auto ct = (*it)->getClientIsland()->linkedIslands.begin(); ct != (*it)->getClientIsland()->linkedIslands.end(); ct++) {
            sf::VertexArray* line = new sf::VertexArray;
            //this island
            line->setPrimitiveType(sf::Lines);
            sf::Vertex a1 = sf::Vertex((*it)->getPosition());
            a1.color = sf::Color(sf::Color::Blue);
            a1.color.a = 128;
            sf::Vertex a2 = sf::Vertex(getIslandButtonFromClientIsland(*ct)->getPosition());
            a2.color = sf::Color(sf::Color::Cyan);
            a2.color.a = 128;
            line->append(a1);
            //linked island
            line->append(a2);

            primitiveDrawables.push_back(line);
            gui.addDrawableToList(line);
        }
    }
    //world.debugPrint();
}

IslandButton* Client::getIslandButtonFromClientIsland(ClientIsland* island) {
    for(auto it = islandButtons.begin(); it != islandButtons.end(); it++) {
        if((*it)->getClientIsland() == island) {
            return (*it);
        }
    }
    return nullptr;
}

void Client::runClient() {
    pingHost = false;

    sf::Clock clock;
    sf::RenderWindow window(sf::VideoMode(800, 600), "U235client");
    window.setVerticalSyncEnabled(true);

    //purriGUI* listener = purriGUI::MenuSwitchListener::createMenuSwitchListener(gui);
    purriGUI::InputBox* hostIpInputSignal = purriGUI::InputBox::createInputBox(gui);
    purriGUI::InputBox* factionNameInputSignal = purriGUI::InputBox::createInputBox(gui);

    sf::Text buttonText;
    //standard button shape in the gui
    sf::RectangleShape buttonShape;
    buttonShape.setPosition(300.0f, 300.0f);
    buttonShape.setFillColor(sf::Color(100,100,100));
    buttonShape.setOutlineColor(sf::Color::Black);
    buttonShape.setOutlineThickness(2.0f);
    buttonShape.setSize(sf::Vector2f(150.0f, 25.0f));

    buttonText.setCharacterSize(20);
    buttonText.setFillColor(sf::Color::White);
    buttonText.setFont(*gui.getFont());
    buttonText.setPosition(buttonShape.getPosition() + sf::Vector2f(5, 0));
    std::string lastIpString = getStringFromFile(LAST_HOST_IP_FILENAME);

    buttonText.setString(lastIpString);

    //IslandButton::createIslandButton(gui, islandButtonShape, &debugIsland, listener);

    buttonShape.setPosition(50, 50);

    buttonText.setPosition(buttonShape.getPosition() + sf::Vector2f(5, 0));
    hostIpField = purriGUI::TextField::createTextField(gui, buttonShape, buttonText, hostIpInputSignal);
    buttonShape.setPosition(50, 100);
    buttonText.setPosition(buttonShape.getPosition() + sf::Vector2f(5, 0));
    factionNameField = purriGUI::TextField::createTextField(gui, buttonShape, buttonText, factionNameInputSignal);
    factionNameField->setSelectingTriggers(false);
    factionNameField->deactivate();
    factionNameField->setDeselectingTriggers(true);
    hostIpField->setSelectingEmpties(false);
    hostIpField->setDeselectingTriggers(true);
    hostIpField->setSelectingTriggers(false);
    if(hostIpField->buttonText.getString().toAnsiString() != "") {
        hostIpField->setFirstSelectingEmpties(false);
        hostIpField->select();
    }
    else {
        hostIpField->buttonText.setString("HOST_IP_ADRESS");
    }
    //create main menu buttons

    //buttonShape.setPosition(400, 50);
    //buttonText.setPosition(buttonShape.getPosition() + sf::Vector2f(5, 0));
    //buttonText.setString("SEND COMMAND");

    //CommandSenderListener* commandListener = CommandSenderListener::createCommandSenderListener(gui, &factionName->buttonText, this );

    //purriGUI::Button* startButton = purriGUI::Button::createButton(gui, buttonShape, buttonText, commandListener);

    //run the thread that handles communications with host
    std::thread clientSubThread ( clientListenThread, this );

    clock.restart();
    while(window.isOpen()) {
        sf::Time loopTime = clock.getElapsedTime();
        sf::Event event;

        //start pinging the server after announced turn time left has expired
        if( world.getTimeLeft() < sf::seconds(0.0f) )
        {
            pingHost = true;
        }

        handleCommandResponses();
        while(window.pollEvent(event)) {
            gui.handleGUIEventActions(event);
            switch(event.type) {
            case sf::Event::Closed:
                {
                    window.close();
                }
            /*
            case sf::Event::TextEntered:
                {
                    hud.forwardTextInput(event.text.unicode);
                    //std::cout << "Input: '" << event.text.unicode << "'\n";
                }
            */
            default:
                {
                    break;
                }
            }
        }

        gui.manageHUD(window);
        //if(listener->menuSwitchChanged(loopTime)) {
        //    std::cout << "listener.menuSwitchChanged(): " << listener->getMenuSwitch() << "\n";
        //}
        //window.draw(buttonShape);
        ///GUI SIGNAL HANDLING
            if(hostIpInputSignal->isReady()) {
                //if the user has inputted the hosts ip, try to 'ping' it in order to get information about host
                setTargetIp(hostIpField->buttonText.getString().toAnsiString());
                addPendingCommand("#p", 0);
                hostIpInputSignal->setReady(false);
            }
            if(factionNameInputSignal->isReady()) {
                if(factionNameField != nullptr) {
                    addPendingCommand("#j" + factionNameField->buttonText.getString().toAnsiString(), 0);
                }
                factionNameInputSignal->setReady(false);
            }
        if(pingHost) {
            if(loopTime > lastPing) {
                lastPing = loopTime+sf::seconds(PING_COOLDOWN_TIME_IN_SECONDS);
                addPendingCommand("#p", 0);
            }
        }
        ///GUI SIGNAL HANDLING END

        window.display();
        window.clear(sf::Color(20, 80, 160));
    }
    killSubThread();
    clientSubThread.join();
}

void ClientWorld::refreshFromString(std::string& worldString) {
    std::cout << "refreshing world from string\n";
    StrDP::StringDataStructure worldData;
    std::string errorString = worldData.parseString(worldString);
    std::cout << "WORLDSTRING:\n" << errorString << "\n";
    if(errorString != "") {
        std::cerr << "Error: Mismatch in client and server communication.\n";
        return;
    }

    const unsigned ISLAND_RECURRING_BLOCK_SIZE = 4;
    const unsigned UNIT_RECURRING_BLOCK_SIZE = 3;

    factions.clear();
    rims.clear();
    islands.clear();
    treatyRequests.clear();
    peaceTreaties.clear();
    warDeclarations.clear();
    player = nullptr;
    //FS0 SEGMENT

    try {
        std::cout << "FS0\n";
        baseRegimentSupply = std::stof((worldData.fs.at(0).gs.at(0).data));
        baseBattleshipSupply = std::stof((worldData.fs.at(0).gs.at(1).data));
        regimentRecoveryRate = std::stof((worldData.fs.at(0).gs.at(2).data));
        shipRecoveryRate = std::stof((worldData.fs.at(0).gs.at(3).data));
        //create factions
        for(auto it = worldData.fs.at(0).gs.at(4).rs.begin(); it != worldData.fs.at(0).gs.at(4).rs.end(); it++) {
            ClientFaction faction;
            faction.name = (*it).us.at(0).data;
            faction.color = sf::Color(std::stoul( ((*it).us.at(1).data) ));
            //base islands are handled after islands are created
            factions.push_back(faction);
            // player is described in FS1 but it is more convinient to actually assign it here
            if(faction.name == worldData.fs.at(0).gs.at(5).data) {
                player = &factions.back();
            }
        }
        if(player == nullptr)
        {
            std::cerr << "Error in ClientWorld::refreshFromString(): assigning the player failed!\n";
        }

        //FS1 SEGMENT
        std::cout << "FS1\n";
        //peace treaties
        for(auto it = worldData.fs.at(1).gs.at(0).rs.begin(); it != worldData.fs.at(1).gs.at(0).rs.end(); it++) {
            std::pair<ClientFaction*, ClientFaction*> treaty;
            treaty.first = getFactionFromName((*it).us.at(0).data);
            treaty.second = getFactionFromName((*it).us.at(1).data);
            peaceTreaties.push_back(treaty);
        }
        //peace treaty requests
        for(auto it = worldData.fs.at(1).gs.at(1).rs.begin(); it != worldData.fs.at(1).gs.at(1).rs.end(); it++) {
            //std::cout << "LOOPING TREATY REQUEST\n";
            std::pair<ClientFaction*, ClientFaction*> request;
            request.first = getFactionFromName((*it).us.at(0).data);
            request.second = getFactionFromName((*it).us.at(1).data);
            treatyRequests.push_back(request);
        }
        turn = std::stoi(worldData.fs.at(1).gs.at(2).data);
        turnEndTime = sf::seconds(std::stof(worldData.fs.at(1).gs.at(3).data));
        turnTimer.restart();
        //FS2 SEGMENT
        std::cout << "FS2\n";
        //saving rims
        for(auto it = worldData.fs.at(2).gs.begin(); it != worldData.fs.at(2).gs.end(); it++) {
            ClientRim rim;
            //4 is for the amount of rs fields there are in island data
            for(unsigned c = 0; c < (*it).rs.size()/ISLAND_RECURRING_BLOCK_SIZE; c++) {
                ClientIsland island;
                //island name
                island.name = (*it).rs[c*ISLAND_RECURRING_BLOCK_SIZE+0].data;
                //island owner
                island.owner = getFactionFromName((*it).rs[c*ISLAND_RECURRING_BLOCK_SIZE+1].data);
                //island units
                //3 is for the amount of us fields it takes to describe a unit
                StrDP::RS* unitData = &(*it).rs[c*ISLAND_RECURRING_BLOCK_SIZE+2];
                for(unsigned k = 0; k < (*unitData).us.size()/UNIT_RECURRING_BLOCK_SIZE; k++) {
                    //std::cout << "looping unit\n";
                    ClientUnit unit;
                    std::string workName = (*unitData).us[k*UNIT_RECURRING_BLOCK_SIZE+0].data;
                    //std::cout << "workName: " << workName << "\n";
                    unit.name = workName;
                    int factionEnd = workName.rfind(UNIT_NAME_SEPARATOR_CHARACTER);
                    if(factionEnd < 0) {
                        std::cerr << "Error in ClientWorld::refreshFromString(): gamesate string was unexpectedly formed in the unit section.\n";
                    }
                    std::string unitSpecifier = workName.substr(factionEnd+1);
                    std::string factionName = workName.substr(0, factionEnd);
                    unit.faction = getFactionFromName(factionName);
                    //std::cout << "unitSpecifier: " << unitSpecifier << "\n";

                    if( unitSpecifier.at(0) == 'r' ) {
                        unit.clientName = "Regiment " + unitSpecifier.at(1);
                        //std::cout << "regiment\n";
                        unit.type = UnitType::REGIMENT;
                    }
                    else if(unitSpecifier.at(0) == 'b') {
                        //std::cout << "battleship\n";
                        unit.clientName = "Battleship " + unitSpecifier.at(1);
                        unit.type = UnitType::BATTLESHIP;
                    }
                    else if(unitSpecifier.at(0) == 's') {
                        //std::cout << "spy\n";
                        unit.clientName = "Spy 00" + unitSpecifier.at(1);
                        unit.type = UnitType::SPY;
                    }
                    else {
                        std::cerr << "Error in ClientWorld::refreshFromString(): gamesate string was unexpectedly formed in the unit section\n";
                    }

                    if(unit.type != UnitType::SPY) {
                        //if is not spy, get unit condition from the string
                        unit.condition = std::stof((*unitData).us[k*UNIT_RECURRING_BLOCK_SIZE+1].data);
                    }
                    else {
                        //if spy, condition is saved as MAX_CONDITION
                        //this shouldn't matter though, as spies are not
                        //supposed to participate in battles
                        unit.condition = MAX_CONDITION;
                    }
                    //movement orders will be handled later on in this process
                    island.units.push_back(unit);
                }
                islands.push_back(island);
                rim.islands.push_back(&islands.back());
            }
            //add rims to list
            rims.push_back(rim);
        }
        //now that all the entities in the world which are referred to by other entities are created, init
        //all the things that refer to somehting else that was not already created earlier
        //starting with faction base islands
        int factionCounter = 0;
        for(auto it = factions.begin(); it != factions.end();it++) {
            (*it).baseIsland = getIslandFromName( worldData.fs[0].gs[4].rs[factionCounter].us[2].data );
            factionCounter++;
        }
        std::cout << "faction base islands done\n";
        //next in turn are linked islands and unit move orders
        std::list<ClientIsland>::iterator islandCounter = islands.begin();
        for(unsigned i = 0; i < worldData.fs[2].gs.size(); i++) {
            //again, 4 is the amount of rs fields it takes to define an island
            for(unsigned c = 0; c < worldData.fs[2].gs[i].rs.size()/ISLAND_RECURRING_BLOCK_SIZE; c++) {
                //unit move orders
                std::list<ClientUnit>::iterator unitCounter = (*islandCounter).units.begin();
                //3 for the amount of us fields it takes to define units
                for( unsigned k = 0; k < worldData.fs[2].gs[i].rs[c*ISLAND_RECURRING_BLOCK_SIZE+2].us.size()/3; k++ ) {
                    std::string moveOrder = worldData.fs[2].gs[i].rs[c*ISLAND_RECURRING_BLOCK_SIZE+2].us[k*UNIT_RECURRING_BLOCK_SIZE+2].data;
                    if ( moveOrder == "" ) { (*unitCounter).targetIsland = nullptr; }
                    else {
                        (*unitCounter).targetIsland = getIslandFromName( moveOrder );
                    }
                    unitCounter++;
                }
                //linked islands
                for(unsigned k = 0; k < worldData.fs[2].gs[i].rs[c*ISLAND_RECURRING_BLOCK_SIZE+3].us.size(); k++) {
                    std::cout << "Island: " << worldData.fs[2].gs[i].rs[c*ISLAND_RECURRING_BLOCK_SIZE].data << " --- " << worldData.fs[2].gs[i].rs[c*ISLAND_RECURRING_BLOCK_SIZE+3].us[k].data << "\n";
                    ClientIsland* island = getIslandFromName( worldData.fs[2].gs[i].rs[c*ISLAND_RECURRING_BLOCK_SIZE+3].us[k].data );
                    if(island == nullptr) {
                        std::cerr << "Error in ClientWorld::refreshFromString(): island was not recogniced from it's name\n";
                    }
                    (*islandCounter).linkedIslands.push_back( island );
                }
                islandCounter++;
            }
        }
        //FS3 SEGMENT
        std::cout << "FS3\n";
        //war declarations
        for(auto it = worldData.fs[3].gs[0].rs.begin(); it != worldData.fs[3].gs[0].rs.end(); it++) {
            std::pair<ClientFaction*, ClientFaction*> declaration;
            declaration.first = getFactionFromName( (*it).us[0].data );
            declaration.second = getFactionFromName( (*it).us[1].data );
            warDeclarations.push_back( declaration );
        }
    }
    catch(const std::exception& e) {
        std::cerr << "Error: Mismatch in communication between server and client. Worldstring returned was either ill-formed, or ill-prosessed on client side!\n" << e.what() << "\n";
        worldString = "";
    }
    std::cout << "refreshFromString() finished!\n";
}

void ClientWorld::debugPrint() {
    unsigned rimCounter = 0;
    //print faction information
    std::cout << "World has following factions:\n";
    for(auto it = factions.begin(); it != factions.end(); it++) {
        std::cout << "faction '" << (*it).name << "' (baseisland '" << (*it).baseIsland->name << "')\n";
    }
    //print rim information
    for(auto it = rims.begin(); it != rims.end(); it++) {
        std::cout << "\nrim " << rimCounter << " has following islands:\n";
        for(auto ct = (*it).islands.begin(); ct != (*it).islands.end(); ct++) {
            std::cout << "'" << (*ct)->name << "'    ";
        }
        rimCounter++;
    }
    std::cout << "\n";
    //print island information
    for(auto it = islands.begin(); it != islands.end(); it++) {
        std::cout << "island '" << (*it).name;
        if( (*it).owner != nullptr ) { std::cout << "' is owned by " << (*it).owner->name << "\n"; }
        else { std::cout << "' is owned by no one\n"; }
        std::cout << " and is linked to:\n";
        for(auto ct = (*it).linkedIslands.begin(); ct != (*it).linkedIslands.end();ct++ ) {
            std::cout << "'" << (*it).name << "'   ";
        }
        //print unit information
        std::cout << "\nand has following units on it:\n";
        for(auto ct = (*it).units.begin(); ct != (*it).units.end(); ct++) {
            std::cout << "'" << (*ct).name << "' (" << (*ct).condition << "/" << MAX_CONDITION << ") belongs to faction "
            << (*ct).faction->name << " and is of unit type ";
            if( (*ct).type == UnitType::BATTLESHIP ) { std::cout << "battleship."; }
            else if( (*ct).type == UnitType::REGIMENT ) { std::cout << "regiment."; }
            else if( (*ct).type == UnitType::SPY ) { std::cout << "spy."; }
            else { std::cout << "bug."; }
            if( (*ct).targetIsland != nullptr ) { std::cout << " It has been ordered towards '" << (*ct).targetIsland->name << "'."; }
            std::cout << "\n";
        }
    }
    std::cout << "peace treaties:\n";
    for(auto it = peaceTreaties.begin(); it != peaceTreaties.end(); it++) {
        std::cout << "'" << (*it).first->name << "' is at peace with '" << (*it).second->name << "'\n";
    }
    std::cout << "peace treaty requests:\n";
    for(auto it = treatyRequests.begin(); it != treatyRequests.end(); it++) {
        std::cout << "'" << (*it).first->name << "' wants to have peace with '" << (*it).second->name << "'\n";
    }
    std::cout << "war declarations:\n";
    for(auto it = warDeclarations.begin(); it != warDeclarations.end(); it++) {
        std::cout << "'" << (*it).first->name << "' has declared war on '" << (*it).second->name << "'\n";
    }
}

bool ClientSideThreadEncapsulation::sendCommandToHost(std::string command, int attemptNumber) {
    //establish a tcp connection and send the string. Then listen for feedback string
    //and add it to the commandResponds list
    //copy all the data the thread uses to different variables so that the blocking parts of the function won't stall the main thread
    std::cout << "Sending Command '"<< command <<"' To Host in ip " << getTargetIp() << "\n";
    if(!subThreadIsActive()) {
        return false;
    }
    std::string ip = getTargetIp();
    //almost straight from sfml documentation
    sf::TcpSocket socket;
    sf::Socket::Status status = socket.connect(ip, PORT_NUMBER);
    if (status != sf::Socket::Done)
    {
        //this is most likely a placeholder for a more official way to say this(inside GUI as some notification)
        std::cerr << "Program networking: game was unable to properly establish connection to ip "<< ip <<"\n";
        return false;
    }
    unsigned bytesSent = 0;
    unsigned bytesRecieved = 0;
    socket.send(command.c_str(), command.size(), bytesSent);
    //recieve reply from host
    char databuffer[DATA_STRING_BUFFER_SIZE];
    //to ensure that the string will be null-terminated eventually, not causing the game to crash as easily with poorly formed strings
    databuffer[DATA_STRING_BUFFER_SIZE-1] = 0;
    socket.receive(databuffer, DATA_STRING_BUFFER_SIZE-1, bytesRecieved);
    //disconnect, so the client side socket won't be waiting for a timeout in case of a server not doing the disconnect for some reason
    socket.disconnect();
    //saving the command response to the appropriate list
    commandRespondMtx.lock();
    CommandResponse response;
    char* returnStringBegin = &databuffer[0];
    response.response = returnStringBegin;
    response.id = attemptNumber;
    response.command = command;
    std::cout << "response recieved from host: " << response.response << "\n";
    commandResponses.push_back( response );
    commandRespondMtx.unlock();
    return true;
}

bool ClientSideThreadEncapsulation::fetchNextCommandResponse( CommandResponse& response ) {
    commandRespondMtx.lock();
    auto it = commandResponses.begin();
    if( it != commandResponses.end() ) {
        response = (*it);
        commandResponses.erase(it);
        commandRespondMtx.unlock();
        return true;
    }
    commandRespondMtx.unlock();
    return false;
}

bool ClientSideThreadEncapsulation::handlePendingCommand() {
    pendingCommandMtx.lock();
    auto it = pendingCommands.begin();
    if( it == pendingCommands.end() ) {
        //the list is empty, there are no commands to handle
        pendingCommandMtx.unlock();
        return false;
    }
    std::string command = (*it).first;
    int id = (*it).second;
    pendingCommandMtx.unlock();
    int attemptNumber = 0;
    while(attemptNumber < ATTEMPT_NUMBER_THRESHOLD) {
        if( sendCommandToHost(command, id) ) {
            break;
        }
        else {
            attemptNumber++;
            //std::cout << "attempt number " << attemptNumber << "\n";
        }
    }
    pendingCommandMtx.lock();
    pendingCommands.erase(it);
    pendingCommandMtx.unlock();
    return true;
}

void Client::handleNonFactionCommandResponses(CommandResponse response) {
    // non-faction commands are identified with the second letter in the command sting
    switch(response.command[1]) {
        //ping command
    case 'p':
        {
            hostIpField->deactivate();
            factionNameField->deactivate();
            if(response.response == MSG::LOBBY) {
                if(factionCode != "") {
                    //do nothing: a game is still waiting to start but this client has registered in as a faction already
                }
                else {
                    if(hostIpField != nullptr) {
                        if(!writeStringToFile(LAST_HOST_IP_FILENAME, hostIpField->buttonText.getString().toAnsiString())) {
                            std::cerr << "Program was unable to write hosts ip down.\n";
                        }
                    }
                    else {
                        std::cerr << "Error in Client::handleClientNonFactionCommandResponses(): hostIpField did not exist when trying to write hosts IP into a file.\n";
                    }
                    if(factionNameField != nullptr) {
                        factionNameField->activate();
                    }
                    else {
                        std::cerr << "Error in Client::handleClientNonFactionCommandResponses(): factionNameField textfield was not ready to use for handling a response to a #p command!\n";
                    }
                }
            }
            //the response is a turn number
            else {
                //turn hasn't changed
                if( world.getTurnNumber() == stoi(response.response) ) { break; }
                if(factionCode != "") {
                    addPendingCommand( factionCode + ":gamestate:", 0);
                    pingHost = false;
                }
                else {
                    //here the client should ask for a faction code
                    factionCode = getStringFromFile("lastfactioncode.txt");
                    if(factionCode != "") {
                        addPendingCommand( factionCode + ":gamestate:", 0 );
                        pingHost = false;
                    }
                }
            }
            break;
        }
    case 'j':
        {
            if(response.response.size() != FACTION_CODE_LENGTH) {
                if(response.response == MSG::GAME_ALREADY_IN_PROGRESS) {
                    std::cout << "IMPLEMENT GAME ALREADY IN PROGRESS FUNCTIONALITY HERE!\n";
                    break;
                }
                std::cerr << "Error in Client::handleClientNonFactionCommandResponses(): potential bug in server responses; command response was not faction code in a #j command, it was instead " << response.response <<"!\n";
            }
            else {
                factionCode = response.response;
                writeStringToFile(LAST_FACTION_CODE_FILENAME, factionCode);
                addPendingCommand("#v"+factionCode, 0);
            }
            break;
        }
    case 'v':
        {
            if(response.response != "") {
                std::cerr << "Error in Client::handleClientNonFactionCommandResponses(): " << response.response << " returned in a #v command!\n";
            }

            pingHost = true;
            break;
        }
    default:
        {
            std::cerr << "Error in Client::handleClientNonFactionCommandResponses(): unknown non-faction command inserted\n";
            break;
        }
    }
}
void Client::handleFactionCommandResponses(CommandResponse response) {
    std::string factionCode, command;
    std::vector<std::string> arguments;
    if( parseFactionCommand(response.command, arguments, command, factionCode) != "") {
        //the command is something weird
        std::cerr << "Error in Client::handleCommandResponses(): faction command inserted was not properly formed!\n";
        return;
    }
    if(command == "") {
        //IMPLEMENT ALL THE COMMAND REACTIONS HERE
    }
    else if(command == "gamestate") {
        updateClientWorld(response.response);
    }
}


void Client::handleCommandResponses() {
    CommandResponse currentResponse;
    //fetchNextCommandResponse returns false when there are no CommandResponses left
    while(fetchNextCommandResponse(currentResponse)) {
        //if the command is a non-faction command
        if(currentResponse.command.size() < 1) { std::cerr << "Error in handleCommandResponses(): command sent was empty!\n"; }
        if( currentResponse.command[0] == '#' ) {
            if( currentResponse.command.size() == 1 ) {
                std::cerr << "Error in Client::handleCommandResponses(): non-faction command insterted was poorly formed\n";
                return;
            }
            //handle non-faction command feedbacks
            handleNonFactionCommandResponses(currentResponse);
            return;
        }
        //if the command is a faction command
        handleFactionCommandResponses(currentResponse);
    }
}

bool Client::attemptMoveSelectedUnitsToIsland(ClientIsland* targetIsland) {
    bool unitButtonWasSelected = false;
    for(auto it = unitButtons.begin(); it != unitButtons.end(); it++) {
        if((*it)->isSelected()) {
            addPendingCommand( factionCode + ":move:" + (*it)->unit->name + FACTION_COMMAND_SEPARATOR_CHARACTER + targetIsland->name + FACTION_COMMAND_SEPARATOR_CHARACTER, 0);
            unitButtonWasSelected = true;
        }
    }
    return unitButtonWasSelected;
}

bool Client::showUnitButtonsOfIsland(ClientIsland* clientIsland) {
    std::cout << "Client::showUnitButtonsOfIsland()\n";
    freeUnitButtons();
    sf::RectangleShape shape;
    shape.setFillColor(sf::Color::White);
    shape.setOutlineColor(sf::Color::Black);
    shape.setOutlineThickness(1.0f);
    shape.setSize(sf::Vector2f(UNIT_BUTTON_WIDTH, UNIT_BUTTON_HEIGHT));
    sf::Text unitText;
    unitText.setCharacterSize(UNIT_TEXT_FONT_SIZE);
    unitText.setColor(sf::Color::Black);
    unitText.setOrigin(5.0f, UNIT_BUTTON_HEIGHT/2);
    unitText.setFont(*gui.getFont());
    sf::Vector2f initPosition = sf::Vector2f(UNIT_BUTTON_TOP_LEFT_COORDINATE_X, UNIT_BUTTON_TOP_LEFT_COORDINATE_Y);
    sf::Vector2f position = sf::Vector2f(0, UNIT_BUTTON_HEIGHT);
    float counter = 0;
    for(auto it = clientIsland->units.begin(); it != clientIsland->units.end(); it++) {
        shape.setPosition(position*counter+initPosition);
        unitText.setPosition(position*counter+initPosition);
        unitText.setString((*it).name + std::to_string((*it).condition));
        shape.setFillColor( sf::Color(it->faction->color) );
        UnitButton* button = UnitButton::createUnitButton(&gui, shape, unitText, &*it);
        unitButtons.push_back(button);
        if((*it).faction == nullptr) { std::cerr << "Error in Client::showUnitButtonsOfIsland(): unit didn't have a faction designated!\n"; }
        if((*it).faction != world.getPlayer() )
        {
            button->setSelectable(false);
        }
        counter+=1.0f;
    }
    std::cout << "Client::showUnitButtonsOfIsland() end\n";
    return true;
}

void IslandButton::onSelection() {
    std::cout << "IslandButton::onSelection()\n";
    if(hud == nullptr) {
        std::cerr << "Error in virtual function IslandButton::onSelection(): hud was defined null!\n";
        return;
    }
     client->attemptMoveSelectedUnitsToIsland(island);
     client->showUnitButtonsOfIsland(island);
     std::cout << "IslandButton::onSelection() end\n";
}

bool Client::freeUnitButtons() {
    //std::cout << "Client::freeUnitButtons()\n";
    bool wasProblem = false;
    for(auto it = unitButtons.begin(); it != unitButtons.end(); it++) {
        //std::cout << "freeing interactable\n";
        if(!gui.freeInteractable((purriGUI::Interactable*)*it)) {
            wasProblem = true;
            std::cerr << "error in Client::freeUnitButtons(): one interactable on the list didn't exist in the gui\n";
        }
    }
    unitButtons.clear();
    std::cout << "Client::freeUnitButtons() returning " << wasProblem << "\n";
    return wasProblem;
}

ClientSideThreadEncapsulation::ClientSideThreadEncapsulation() {
    nextCommandId = 0;
    subThreadActive = false;
    subThreadKilled = false;
    targetIp.clear();
}

void ClientSideThreadEncapsulation::setSubThreadActiveState(bool state) {
    configMtx.lock();
    subThreadActive = state;
    configMtx.unlock();
}

void ClientSideThreadEncapsulation::killSubThread() {
    configMtx.lock();
    subThreadKilled = true;
    configMtx.unlock();
}

bool ClientSideThreadEncapsulation::subThreadIsKilled() {
    bool returnBool;
    configMtx.lock();
    returnBool = subThreadKilled;
    configMtx.unlock();
    return returnBool;
}

std::string ClientSideThreadEncapsulation::getTargetIp() {
    configMtx.lock();
    std::string returnString = targetIp;
    configMtx.unlock();
    return returnString;
}

void ClientSideThreadEncapsulation::setTargetIp(std::string ip) {
    configMtx.lock();
    targetIp = ip;
    configMtx.unlock();
}

bool ClientSideThreadEncapsulation::subThreadIsActive() {
    bool returnBool;
    configMtx.lock();
    returnBool = subThreadActive;
    configMtx.unlock();
    return returnBool;
}

void ClientSideThreadEncapsulation::addPendingCommand(const std::string& commandString, int id) {
    pendingCommandMtx.lock();
    std::pair<std::string, int> command;
    command.first = commandString;
    command.second = id;
    pendingCommands.push_back(command);
    std::cout << "command '" << command.first << "' added to pending commands\n";
    pendingCommandMtx.unlock();
}

ClientFaction* ClientWorld::getFactionFromName(std::string name) {
    for(auto it = factions.begin(); it != factions.end(); it++) {
        if(name == (*it).name) {
            return &(*it);
        }
    }
    return nullptr;
}

ClientIsland* ClientWorld::getIslandFromName(std::string name) {
    for(auto it = islands.begin(); it != islands.end(); it++) {
        if( (*it).name == name ) {
            return &(*it);
        }
    }
    return nullptr;
}

IslandButton::IslandButton(sf::CircleShape shape, Client* _client, purriGUI::GUI* _hud, ClientIsland* _island, purriGUI::SignalListener* slot): Interactable(shape, _hud, slot) {
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

IslandButton* IslandButton::createIslandButton(purriGUI::GUI& hud, Client* _client, sf::CircleShape shape, ClientIsland* _island, purriGUI::SignalListener* slot) {
    IslandButton* islandButton = new IslandButton(shape, _client, &hud, _island, slot);
    hud.addInteractableToList((Interactable*)islandButton);
    return islandButton;
}

UnitButton::UnitButton(sf::RectangleShape shape, purriGUI::GUI* _hud, sf::Text _buttonText, ClientUnit* _unit, purriGUI::SignalListener* slot): Button(shape, _hud, _buttonText, slot) {
    //std::cout << "creating new UnitButton\n";
    unit = _unit;
    setClickingExclusiveSelectsState(false);
    setDeselectingTriggers(true);
    setSelectingTriggers(true);
    setXorSelectMode(true);
}

UnitButton* UnitButton::createUnitButton(purriGUI::GUI* gui, sf::RectangleShape shape, sf::Text _buttonText, ClientUnit* _unit, purriGUI::SignalListener* slot) {
    UnitButton* unitButton = new UnitButton(shape, gui, _buttonText, _unit, slot);
    gui->addInteractableToList((Interactable*)unitButton);
    return unitButton;
}

CommandSenderListener::CommandSenderListener( sf::Text* _commandText, Client* _client ) {
    commandText = _commandText;
    client = _client;
}

CommandSenderListener* CommandSenderListener::createCommandSenderListener(purriGUI::GUI& hud, sf::Text* _commandText, Client* _client) {
    CommandSenderListener* listener = new CommandSenderListener(_commandText, _client);
    hud.addListenerToList((SignalListener*)listener);
    return listener;
}
