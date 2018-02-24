//#include "names.hpp"
#include "world.hpp"

unsigned Identifiable::nextID = 0;

void connectIslands(Island* a, Island* b) {
    a->addNeighbour(b);
    b->addNeighbour(a);
}

BattleSide* getBattleSideFromFaction(std::list<BattleSide>& sides, Faction* faction) {
    if(faction == NULL) {
        std::cerr << "Error in getBattleSideFromFaction(): NULL pointer given as faction argument!\n";
        return NULL;
    }
    for( auto it = sides.begin(); it != sides.end(); it++ ) {
        if( (*it).owner == faction ) {
            return &(*it);
        }
    }
    std::cerr << "Error in getBattleSideFromFaction(): No faction '" << faction->getName() << "'was among battleSides\n";
    return NULL;
}

Island* Island::searchNextNeighbour(IslandList blackList, bool adjacent, bool following, bool previous) {
    for(IslandIterator it = linkedIslands.begin(); it != linkedIslands.end(); it++) {
        //not on a black list to this search
        bool onBlackList = false;
        for(IslandIterator ct = blackList.begin(); ct != blackList.end(); ct++ ) {
            if(*ct == *it) {
                onBlackList = true;
                break;
            }
        }
        if(onBlackList) {
            continue;
        }
        if(adjacent) {
            if((*it)->getRimNumber() == rimNumber) {
                return *it;
            }
        }
        if(following) {
            if((*it)->getRimNumber() == rimNumber+1) {
                return *it;
            }
        }
        if(previous) {
            if((*it)->getRimNumber() == rimNumber-1) {
                return *it;
            }
        }
    }
    return NULL;
}

Island* Rim::getNextIsland(unsigned connectionsAmount) {
    for(std::list<Island>::iterator it = islands.begin(); it != islands.end(); it++) {
        if( (*it).getLinkedIslands().size() != connectionsAmount ) {
            continue;
        }
        return &*it;
    }
    return NULL;
}

void Rim::debugPrint() {
    std::cout << getName() << " has " << islands.size() << " islands. They are listed below with neighbour information.\n";
    for(std::list<Island>::iterator it = islands.begin(); it != islands.end(); it++) {
        IslandList neighbourList = (*it).getLinkedIslands();
        std::cout << "Island '" << (*it).getName() << "' has ";
        for(IslandIterator ct = neighbourList.begin(); ct != neighbourList.end(); ct++) {
            std::cout << "'" << (*ct)->getName() << "' ";
        }
        std::cout << " as it's neighbours and it belongs to ";
        if( (*it).getOwner() == NULL) {
            std::cout << "no one.\n";
        }
        else {
            std::cout << (*it).getOwner()->getName() << ".\n";
        }
    }
}

void Rim::setAdjacentIslands() {
    if (islands.size() < 2) {
        return;
    }
    //std::cout << "setting adjacent islands\n";
    std::list<Island>::iterator startIterator = islands.begin();
    //std::cout << "This rim has " << islands.size() << " islands!\n";
    //previousIsland is the first and currentIsland the second island in the list
    Island* previousIsland = &*startIterator;
    startIterator++;
    Island* currentIsland = &*startIterator;
    startIterator--;
    connectIslands(currentIsland, previousIsland);
    if(islands.size() == 2) {
        return;
    }
    while(true) {
        previousIsland = currentIsland;
        currentIsland = getNextIsland(0);
        if( currentIsland == NULL) {
            break;
        }
        connectIslands(previousIsland, currentIsland);
    }
    //std::cout << "last island being inserted!\n";
    connectIslands(previousIsland, &*startIterator);
    //std::cout << "adjacent islands set\n";
}

void World::setFollowingRims() {

    //this for loop covers all except the  first rim and the nuke island rim, which will be handled seperately.
    std::list<Rim>::iterator curr = rims.begin();
    curr++;
    std::list<Rim>::iterator prev = rims.begin();
    std::list<Rim>::iterator next = rims.begin();
    next++;
    next++;
    std::list<Rim>::iterator lastRim = rims.end();
    lastRim--;
    //iterate rims
    for(; curr != lastRim; curr++) {
        //std::cout << "new rim\n";
        //get ratios for islands of the previous and next rims of the current rim
        float previousToCurrentRatio = ((float)(*prev).islands.size())/((float)(*curr).islands.size());
        //float currentToNextRatio = ((float)(*curr).islands.size())/((float)(*next).islands.size());
        //std::cout << "previousToCurrentRatio: " << previousToCurrentRatio << "\n"; //<< "\ncurrentToNextRatio " << currentToNextRatio << "\n";
        ///int currIslandAmount = (float)(*curr).islands.size();
        //int nextIslandAmount = (float)(*next).islands.size();
        ///int prevIslandAmount = (float)(*prev).islands.size();
        //std::list<Island>::iterator nt = (*next).islands.begin();
        //std::list<Island>::iterator pt = (*prev).islands.begin();
        //std::list<Island>::iterator ct1 = (*curr).islands.begin();
        //std::list<Island>::iterator ct2 = (*curr).islands.begin();
        std::list<Island>::iterator pt = (*prev).islands.begin();
        //std::list<Island>::iterator nt = (*next).islands.begin();
        float prevCounter = 0.0f;
        //float nextCounter = 0.0f;
        //connect islands
        for(std::list<Island>::iterator it = (*curr).islands.begin(); it != (*curr).islands.end(); it++) {
            //float prevCounter = 0.0f;
            //float nextCounter = 0.0f;
            //float currCounter2 = 0.0f;
            //std::cout << "Iterating island\n";
            if( previousToCurrentRatio >= 1.0f) {
                int swapper = 1;
                //std::cout << "previousToCurrentRatio >= 1.0f\n";
                for(float aprevCounter = previousToCurrentRatio; aprevCounter > 0; aprevCounter-=1.0f) {

                    //std::cout << "connected '" << (*it).getName() << "' to '" << (*pt).getName() << "'\n";
                    connectIslands(&*it, &*pt);
                    //concerns a special case of 12 islands on the previous rim and 8 islands on current rim
                    if(previousToCurrentRatio == 1.5f) {
                        if( swapper > 0) {
                            swapper*=-1;
                            pt++;
                        }
                    }
                    else {
                        pt++;
                    }
                }
            }
            else {
                //std::cout << "previousToCurrentRatio < 1.0f\n";
                prevCounter+=previousToCurrentRatio;
                //std::cout << "connected '" << (*it).getName() << "' to '" << (*pt).getName() << "'\n";
                connectIslands(&*it, &*pt);
                if(prevCounter >= 0.999) {
                    pt++;
                    prevCounter = 0.0f;
                }
            }
        }
        next++;
        prev++;
    }
    //now connecting the final rim into the middle nuclear island
    std::list<Rim>::iterator nukeRim = lastRim;
    lastRim--;
    for(std::list<Island>::iterator it = (*lastRim).islands.begin(); it != (*lastRim).islands.end(); it++) {
        connectIslands(&(*it), &(*(*nukeRim).islands.begin()));
    }

    //std::cout << "--------------------------------------After following connecting rim status is:----------------------------------------\n";
    //debugPrint();
}
void World::peaceTreaty(Faction* a, Faction* b) {
    if( a->isAllyOf(b) || b->isAllyOf(a) ) {
        std::cerr << "Error in World::peaceTreaty(): Factions '" << a->getName() << "' and '" << b->getName() << "' were already allies\n";
        return;
    }
    a->addAlly(b);
    b->addAlly(a);
}


void World::setWar(Faction* a, Faction* b) {
    if( !a->isAllyOf(b) || !b->isAllyOf(a) ) {
        //actually, it is better for the function to just return instead of an error message
        //std::cerr << "Error in World::declareWar(): factions '" << a->getName() << "' and '" << b->getName() << "' were at war to begin with.\n";
        return;
    }
    a->removeAlly(b);
    b->removeAlly(a);
}

bool World::isDeclaringWar(Faction* subject, Faction* object) {
    for(auto it = warDeclarations.begin(); it != warDeclarations.end(); it++) {
        if( (*it).first == subject && (*it).second == object ) {
            return true;
        }
    }
    return false;
}

void World::handleDiplomacy() {
    auto ct = treatyRequests.begin();
    ct++;
    //function will compare every treatyRequest with the other once, and if the two requests
    //are a pair in a way that they are two factions requesting each other for treaty, a peace treaty will be made
    for( auto it = treatyRequests.begin(); it != treatyRequests.end(); it++ ) {
        for(; ct != treatyRequests.end(); ct++) {
            if( ( (*it).first == (*ct).second && (*ct).first == (*it).second ) || ( (*ct).first == (*it).second && (*it).first == (*ct).second ) ) {
                peaceTreaty( (*it).first, (*it).second );
                it = treatyRequests.erase(it);
                ct = treatyRequests.erase(ct);
            }
        }
    }

    for( auto it = warDeclarations.begin(); it != warDeclarations.end(); it++ ) {
        setWar((*it).first, (*it).second);
        it = warDeclarations.erase(it);
    }
}

std::string World::startWorld(unsigned _rimAmount, int islandness, int _falloutTime, float _baseRegimentSupply, float _baseBattleshipSupply,
    unsigned _spyAmount, int _regimentRecoveryRate, int _shipRecoveryRate, std::string worldName) {

    if(!unstarted) {
        return MSG::WORLD_ALREADY_STARTED;
    }
    std::list<Faction*> factionsPointerList;

    //unsigned _rimAmount = 5;
    //int islandness = 3;

    int rimIslandAdditionValues[_rimAmount];

    for(unsigned i = 0; i < _rimAmount; i++) {
        rimIslandAdditionValues[i] = random(islandness);
        //std::cout << "rim " << i << " has " << rimIslandAdditionValues[i] << " as it's island multiplier value\n";
    }
    //initializing World data
    setName(worldName);
    //create pointer list for rims to use
    for(std::list<Faction>::iterator it = factions.begin(); it != factions.end(); it++) {
        //factions.push_back(Faction(factionNames[i]));
        factionsPointerList.push_back(&(*it));
    }
    playerAmount = factions.size();
    unverifiedFactions.clear();
    rimAmount = _rimAmount+1;
    falloutTime = _falloutTime;
    baseRegimentSupply = _baseRegimentSupply;
    baseBattleshipSupply = _baseBattleshipSupply;
    spyAmount = _spyAmount;
    regimentRecoveryRate = _regimentRecoveryRate;
    shipRecoveryRate = _shipRecoveryRate;
    unstarted = false;

    //creating Rims and islands for world according to function parameters

    Rim baseRim = Rim(playerAmount, factionsPointerList, 0, 1, 1, &islandNames);
    baseRim.setDefaultName();
    rims.push_back(baseRim);
    //int lastAmountOfIslands = _playerAmount;
    //int islandAmount = lastAmountOfIslands;
    //std::cout << "First rim pushed!\n";
    for(unsigned i = 1; i < _rimAmount; i++) {
        int islandAmount = rimIslandAdditionValues[i]*playerAmount;
        Rim newRim = Rim(islandAmount, factionsPointerList, 0, 0, i+1, &islandNames);
        newRim.setDefaultName();
        rims.push_back(newRim);
        //std::cout << "Iterable rim pushed!\n";
    }
    Rim nukeRim = Rim(1, factionsPointerList, 1, 0, _rimAmount+1, &islandNames);
    nukeRim.setDefaultName();
    rims.push_back(nukeRim);
    //std::cout << "Nuke rim pushed!\n";
    setAdjacentRims();
    //std::cout << "Adjacent rims Set!\n";
    setFollowingRims();
    //std::cout << "Following rims Set!\n";

    setStartingTroops();

    return "";
    //F(); ??WTH IS THIS???
}

void World::setStartingTroops() {
    std::list<Rim>::iterator rim1 = rims.begin();
    std::list<Faction>::iterator fa = factions.begin();
    //conquer one island for every faction in rim 1
    for(std::list<Island>::iterator it = (*rim1).islands.begin(); it != (*rim1).islands.end(); it++) {
        if(fa == factions.end()) {
            std::cerr << "in setStartingTroops(): Differing amount of factions and rim 1 islands!\n";
            return;
        }
        (*fa).baseIsland = &(*it);
        //for(unsigned i = 0; i < startRegimentAmount; i++) {
        (*it).addTroops(startBattleshipAmount, startRegimentAmount, spyAmount, &(*fa));
        fa++;
    }
}

void World::setAdjacentRims() {
    std::list<Rim>::iterator it = rims.begin();
    it++;
    for(; it != rims.end(); it++) {
        //std::cout << "iterating rim islands for adjacent placement!\n";
        (*it).setAdjacentIslands();
    }
}

void World::debugPrint() {
    std::cout << "World '" << getName() << "' has " << rims.size() << " rims. Their contents are listed below:\n";
    for(std::list<Rim>::iterator it = rims.begin(); it != rims.end(); it++) {
        (*it).debugPrint();
    }
    std::cout << "\n***FACTION TROOP INFORMATION***\n";
    for(std::list<Faction>::iterator it = factions.begin(); it != factions.end(); it++) {
        (*it).printTroopData();
    }
    std::cout << "\n***FACTION DIPLOMACY INFORMATION***\nPending war declarations:\n";
    for(auto it = warDeclarations.begin(); it != warDeclarations.end(); it++) {
        std::cout << (*it).first->getName() << " is declaring war on " << (*it).second->getName() << ".\n";
    }
    std::cout << "\nPending peace treaty requests:\n";
    for(auto it = treatyRequests.begin(); it != treatyRequests.end(); it++) {
        std::cout << (*it).first->getName() << " wants to sign peace with " << (*it).second->getName() << ".\n";
    }
    std::cout << "\nAlliances:\n";
    for(auto it = factions.begin(); it != factions.end(); it++) {
        for( auto ct = factions.begin(); ct != factions.end(); ct++ ) {
            if(/*!actually, might be good to leave this commented for debugging purposes. If faction thinks it's allied with itself
                it's good to know that something is wrong in the code !it != ct && */
               (*it).isAllyOf((&*ct))) {
                    std::cout << (*it).getName() << " is allied with " << (*ct).getName() << "\n";
            }
        }
    }
}

void Faction::printTroopData() {
    std::cout << getName() << " troops:\n";
    //print regiment data
    std::cout << "--regiments--\n";
    for(std::list<Regiment>::iterator it = regiments.begin(); it != regiments.end(); it++) {
        std::cout << (*it).getName() << "(" << (*it).condition << "/" << MAX_CONDITION << ")" << " is on island " << (*(*it).getPosition()).getName();
        if( ((*it).getPosition()) != ((*it).getTarget()) && ((*it).getTarget()) != NULL ) { std::cout << " and is heading towards " << (*(*it).getTarget()).getName(); }
        std::cout << "\n";
    }
    std::cout << "--battleships--\n";
    for(std::list<Battleship>::iterator it = battleships.begin(); it != battleships.end(); it++) {
        std::cout << (*it).getName() << "(" << (*it).condition << "/" << MAX_CONDITION << ")" << " is at island " << (*(*it).getPosition()).getName();
        if( ((*it).getPosition()) != ((*it).getTarget()) && ((*it).getTarget()) != NULL ) { std::cout << " and is heading towards " << (*(*it).getTarget()).getName(); }
        std::cout << "\n";
    }
    std::cout << "--spies--\n";
    for(std::list<Spy>::iterator it = spies.begin(); it != spies.end(); it++) {
        std::cout << (*it).getName() << " is on island " << (*(*it).getPosition()).getName();
        if( ((*it).getPosition()) != ((*it).getTarget()) && ((*it).getTarget()) != NULL ) { std::cout << " and is heading towards " << (*(*it).getTarget()).getName(); }
        std::cout << "\n";
    }
}

std::string World::addTreatyRequest(Faction* subject, Faction* object) {
    //first check for null pointers and return an appropriate error message if found
    if(subject == NULL || object == NULL) {
        return MSG::INVALID_FACTION;
    }
    //check if such treatyRequest already exists.
    for( auto it = treatyRequests.begin(); it != treatyRequests.end(); it++ ) {
        if((*it).first == subject && (*it).second == object) {
            return MSG::TREATY_ALREADY_REQUESTED_WITH_THIS_FACTION;
        }
    }
    treatyRequests.push_back(std::pair<Faction*, Faction*>(subject, object));
    return "";
}

Regiment* World::getRegimentFromName(std::string regimentName) {
    for(std::list<Faction>::iterator it = factions.begin(); it != factions.end(); it++) {
        for(std::list<Regiment>::iterator ct = (*it).regiments.begin(); ct != (*it).regiments.end(); ct++) {
            if((*ct).getName() == regimentName) {
                return &(*ct);
            }
        }
    }
    //std::cerr << "Such regiment was not found, returning NULL!\n";
    return NULL;
}

Faction* World::getFactionFromName(std::string factionName) {
    for(std::list<Faction>::iterator it = factions.begin(); it != factions.end(); it++) {
        if((*it).getName() == factionName) {
            return &(*it);
        }
    }
    //std::cerr << "Such regiment was not found, returning NULL!\n";
    return NULL;
}

Battleship* World::getBattleshipFromName(std::string battleshipName) {
    for(std::list<Faction>::iterator it = factions.begin(); it != factions.end(); it++) {
        for(std::list<Battleship>::iterator ct = (*it).battleships.begin(); ct != (*it).battleships.end(); ct++) {
            if((*ct).getName() == battleshipName) {
                return &(*ct);
            }
        }
    }
    return NULL;
}

Spy* World::getSpyFromName(std::string spyName) {
    for(std::list<Faction>::iterator it = factions.begin(); it != factions.end(); it++) {
        for(std::list<Spy>::iterator ct = (*it).spies.begin(); ct != (*it).spies.end(); ct++) {
            if((*ct).getName() == spyName) {
                return &(*ct);
            }
        }
    }
    return NULL;
}

std::string World::moveTroop(std::string factionCode, std::vector<std::string> arguments) {
    //argument[0] is the unit name and argument[1] is the target island name
    if(arguments.size() != 2) {
        std::cerr << "Error in World::moveTroop(): wrong amount of arguments given to command 'move'\n";
        return MSG::INVALID_AMOUNT_OF_ARGUMENTS_IN_COMMAND;
    }
    Faction* troopFaction = getFactionFromCode(factionCode);
    if(troopFaction == NULL) {
        std::cerr << "Error in World::moveTroop(): getFactionCode() returned NULL\n";
    }
    if(!troopFaction) {
        std::cerr << "Error in World::moveTroop(): factionCode argument given in function call did not match to any of the factions.\n";
        return MSG::INVALID_FACTION_CODE;
    }
    Regiment* regiment = getRegimentFromName(arguments[0]);
    //std::cout << "regiment->getName returns " << regiment->getName() << "\n";
    if(regiment) {
        //if regiment of this name was found
        if(regiment->getFaction() == NULL) {
            std::cerr << "Error in World::moveTroop(): regiments faction was NULL\n";
        }
        if(regiment->getFaction() != troopFaction) {
            //std::cout << "troopFaction: " << troopFaction->getName() << "regiment->faction: ";// << regiment->getFaction()->getName() << "\n";
            //if the regiment belonged to a different faction
            std::cerr << "Error in World::moveTroop(): unit '" << arguments[0] << "' did not belong to faction '" << troopFaction->getName() << "'\n";
            return MSG::UNIT_DID_NOT_BELONG_TO_THIS_FACTION;
        }
        Island* targetIsland = getIslandFromName(arguments[1]);
        if(!targetIsland) {
            std::cerr << "Error in World::moveTroop(): island '" << arguments[1] << "' apparently does not exist\n";
            return MSG::ISLAND_DOES_NOT_EXIST;
        }
        //if the islands are not connected, The move is illegal in the game rules. Therefore, return an appropriate error string
        if( !(regiment->getPosition())->isLinkedTo(targetIsland) ) {
            return MSG::ILLEGAL_MOVE_ISLANDS_NOT_CONNECTED;
        }
        regiment->setTarget(targetIsland);
        return "";
    }
    Battleship* battleship = getBattleshipFromName(arguments[0]);
    if(battleship) {
        //if battleship of this name was found
        if(battleship->getFaction() == NULL) {
            std::cerr << "Error in World::moveTroop(): battleships faction was NULL\n";
        }
        if(battleship->getFaction() != troopFaction) {
            //if the battleship belonged to a different faction
            std::cerr << "Error in World::moveTroop(): unit '" << arguments[0] << "' did not belong to faction '" << troopFaction->getName() << "'\n";
            return MSG::UNIT_DID_NOT_BELONG_TO_THIS_FACTION;
        }
        Island* targetIsland = getIslandFromName(arguments[1]);
        if(!targetIsland) {
            std::cerr << "Error in World::moveTroop(): island '" << arguments[1] << "' apparently does not exist\n";
            return MSG::ISLAND_DOES_NOT_EXIST;
        }
        battleship->setTarget(targetIsland);
        return "";
    }
    Spy* spy = getSpyFromName(arguments[0]);
    if(spy) {
        //if spy of this name was found
        if(spy->getFaction() == NULL) {
            std::cerr << "Error in World::moveTroop(): battleships faction was NULL\n";
        }
        if(spy->getFaction() != troopFaction) {
            //if the spy belonged to a different faction
            std::cerr << "Error in World::moveTroop(): unit '" << arguments[0] << "' did not belong to faction '" << troopFaction->getName() << "'\n";
            return MSG::UNIT_DID_NOT_BELONG_TO_THIS_FACTION;
        }
        Island* targetIsland = getIslandFromName(arguments[1]);
        if(!targetIsland) {
            std::cerr << "Error in World::moveTroop(): island '" << arguments[1] << "' apparently does not exist\n";
            return MSG::ISLAND_DOES_NOT_EXIST;
        }
        spy->setTarget(targetIsland);
        return "";
    }
    std::cerr << "Error in World::moveTroop(): no unit named '" << arguments[0] << "' was found\n";
    return MSG::UNIT_WAS_NOT_FOUND;
};

Island* World::getIslandFromName(std::string islandName) {
    for(std::list<Rim>::iterator it = rims.begin(); it != rims.end(); it++) {
        for(std::list<Island>::iterator ct = (*it).islands.begin(); ct != (*it).islands.end(); ct++) {
            if( (*ct).getName() == islandName ) {
                return &(*ct);
            }
        }
    }
    return NULL;
}

void Island::addTroops(int battleshipAmount, int regimentAmount, int spyAmount, Faction* faction) {
    Garrison* garrison = getGarrison(faction);
    //add battleships
    for(int i = 0; i < battleshipAmount; i++) {
        garrison->battleships.push_back(faction->addBattleship(this));
        garrison->battleships.back()->setFaction(faction);
    }
    //add regiments
    for(int i = 0; i < regimentAmount; i++) {
        garrison->regiments.push_back(faction->addRegiment(this));
        garrison->regiments.back()->setFaction(faction);
        //std::cout << "DEBUG PRINT HERE " << faction->regiments.back().getFaction() << "\n";
    }
    //add spies
    for(int i = 0; i < spyAmount; i++) {
        garrison->spies.push_back(faction->addSpy(this));
        garrison->spies.back()->setFaction(faction);
    }
}

Battleship* Faction::addBattleship(Island* targetIsland) {

    std::string shipName = getBattleshipName();

    battleships.push_back(Battleship());
    battleships.back().condition = MAX_CONDITION;
    battleships.back().setName(shipName);
    battleships.back().setTarget(NULL);
    battleships.back().setPosition(targetIsland);
    //battleships.back().setFaction(this);
    return &battleships.back();
}
Regiment* Faction::addRegiment(Island* targetIsland) {

    std::string regimentName = getRegimentName();
    regiments.push_back(Regiment());
    regiments.back().condition = MAX_CONDITION;
    regiments.back().setName(regimentName);
    regiments.back().setTarget(NULL);
    regiments.back().setPosition(targetIsland);
    //std::cout << "ADDREGIMENT\n";
    //battleships.back().setFaction(this);
    //std::cout << "in addRegiment regiment faction is '" << battleships.back().getFaction()->getName() << "'\n";
    return &regiments.back();
}
Spy* Faction::addSpy(Island* targetIsland) {

    std::string spyName = getSpyName();
    spies.push_back(Spy());
    spies.back().setName(spyName);
    spies.back().setTarget(NULL);
    spies.back().setPosition(targetIsland);
    //battleships.back().setFaction(this);
    return &spies.back();
}

std::string World::endTurn() {
    if(!isStarted()) {
        return MSG::GAME_NOT_STRATED_YET;
    }
    turn++;
    //move troops, do battles. That sort of thing!

    //handle peace treaties before moving troops or fighting battles
    handleDiplomacy();

    //move regiments
    for(std::list<Faction>::iterator it = factions.begin(); it != factions.end(); it++) {
        for( std::list<Regiment>::iterator ct = (*it).regiments.begin(); ct != (*it).regiments.end(); ct++ ) {
            if( (*ct).getPosition() != (*ct).getTarget() && (*ct).getTarget() != NULL ) {
                //if unit was moved
                (*ct).recentlyMoved = true;
                Island* oldPosition = (*ct).getPosition();
                (*ct).setPosition((*ct).getTarget());
                (*ct).setTarget(NULL);
                oldPosition->popUnitFromList(/*(Movable*)*/&(*ct));
                //now add the unit to the new islands unit list
                (*ct).getPosition()->addUnitToList(&(*ct), &(*it) );

            }
            else {
                (*ct).recentlyMoved = false;
            }
        }
    }
    //move battleships
    for(std::list<Faction>::iterator it = factions.begin(); it != factions.end(); it++) {
        for( std::list<Battleship>::iterator ct = (*it).battleships.begin(); ct != (*it).battleships.end(); ct++ ) {
            if( (*ct).getPosition() != (*ct).getTarget() && (*ct).getTarget() != NULL ) {
                (*ct).recentlyMoved = true;
                Island* oldPosition = (*ct).getPosition();
                (*ct).setPosition((*ct).getTarget());
                (*ct).setTarget(NULL);
                oldPosition->popUnitFromList(/*(Movable*)*/&(*ct));
                //now add the unit to the new islands unit list
                (*ct).getPosition()->addUnitToList(&(*ct), &(*it) );
            }
            else {
                (*ct).recentlyMoved = false;
            }
        }
    }
    //move spies
    for(std::list<Faction>::iterator it = factions.begin(); it != factions.end(); it++) {
        for( std::list<Spy>::iterator ct = (*it).spies.begin(); ct != (*it).spies.end(); ct++ ) {
            if( (*ct).getPosition() != (*ct).getTarget() && (*ct).getTarget() != NULL ) {
                (*ct).recentlyMoved = true;
                Island* oldPosition = (*ct).getPosition();
                (*ct).setPosition((*ct).getTarget());
                (*ct).setTarget(NULL);
                oldPosition->popUnitFromList(/*(Movable*)*/&(*ct));
                //now add the unit to the new islands unit list
                (*ct).getPosition()->addUnitToList(&(*ct), &(*it) );
            }
            else {
                (*ct).recentlyMoved = false;
            }
        }
    }
    fightBattles();
    std::cout << "Battles fought\n";
    //recruit new troops
    for(auto it = factions.begin(); it != factions.end(); it++) {
        (*it).turnUpdate(getDominance((&*it)), baseRegimentSupply, baseBattleshipSupply);
    }

    return "";
}
//This function returns how many islands are in control of this faction
float World::getDominance(Faction* faction) {
    float dominance = 0.0f;
    for(auto it = rims.begin(); it != rims.end(); it++) {
        for(auto ct = (*it).islands.begin(); ct != (*it).islands.end(); ct++) {
            if((*ct).owner == faction) {
                dominance+=1;
            }
        }
    }
    return dominance;
}

void Faction::turnUpdate(float areaDominance, float baseRegimentSupply, float baseBattleshipSupply) {

    //in the beginning of the turn factions will recruit new battleships and regiments relative to the base supply values and how much islands they own
    //more islands means more troops
    newRegimentProgress+= baseRegimentSupply + areaDominance*dominanceToRegimentsFactor;
    newBattleshipProgress+= baseBattleshipSupply + areaDominance*dominanceToBattleshipsFactor;
    //add new regiments
    for(; newRegimentProgress > 1.0f; newRegimentProgress-=1.0f ) {
        addRegiment(baseIsland);
    }
    //add new battleships
    for(; newBattleshipProgress > 1.0f; newBattleshipProgress-=1.0f ) {
        addBattleship(baseIsland);
    }

    for(auto it = regiments.begin(); it != regiments.end(); it++) {
        (*it).getPosition()->attemptClaim((*it).getFaction());
    }
}


void BattleSide::countTotalStrength(std::list<BattleSide>* sides) {
    std::cout << "World::countTotalStrenght()\n";
    //total strength is personal strength combined with the strength of allies that are fighting your enemies also
    totalStrength = personalStrength;
    for( auto it = allies.begin(); it != allies.end(); it++ ) {
        std::cout << "new battleSide ally: " << (*it)->owner->getName() << "\n";
        std::cout << "cts::iterating new i\n";
        //float sidesTotal = 0.0f;
        float nonMutualEnemyStrength = 0.0f;
        float mutualEnemyStrength = 0.0f;
        for( auto ct = sides->begin(); ct != sides->end(); ct++ ) {
            std::cout << "new iterable side: " << (*ct).owner->getName() << "\n";
            std::cout << "cts::iterating new c\n";
            if( &(*ct) != this && &(*ct) != (*it) ) {
                if( (*ct).isAllyOf((*it)->owner) ) {
                    //battleside *it is not of use against foe *ct
                    nonMutualEnemyStrength += (*ct).personalStrength;

                }
                else {
                    mutualEnemyStrength += (*ct).personalStrength;
                }
                std::cout << "aaaa\n";
                if(nonMutualEnemyStrength == 0) {
                    totalStrength += (*it)->personalStrength*1;
                }
                else {
                    totalStrength += (*it)->personalStrength*(nonMutualEnemyStrength+mutualEnemyStrength)/nonMutualEnemyStrength;
                }
                std::cout << "bbbb\n";
            }
            else {
                std::cout << "same side spotted!\n";
            }
        }
    }
    std::cout << "totalStrength counted!\n";
}

void BattleSide::countOppressionStrength(std::list<BattleSide>* sides) {
    std::cout << "countOpressionStrength\n";
    oppressionStrength = 0.0f;
    for( auto it = sides->begin(); it != sides->end(); it++ ) {
        std::cout << "counting opression strength for " << (*it).owner->getName() << "\n";
        if( (*it).owner == owner || (*it).isAllyOf(owner) ) {
            continue;
        }
        oppressionStrength += (*it).personalStrength;
    }
    std::cout << "battleSide oppression strength: " << oppressionStrength << "\n";
    std::cout << "opression Strength counted!\n";
}

void Island::damageUnits(std::list<BattleSide>& sides) {

    std::default_random_engine generator;
    std::uniform_real_distribution<float> randomDamageDistribution(0, battleRandomDamageConstant*2);
    //damage every unit accordingly
    for(auto it = garrisons.begin(); it != garrisons.end(); it++) {
        if( (*it).regiments.size() + (*it).battleships.size() == 0 ) { continue; }

        BattleSide* currentBattleSide = getBattleSideFromFaction(sides, (*it).getFaction());
        float disadvantage = battleAttackerDisadvantageConstant;
        if( currentBattleSide->defenderSide ) {
            disadvantage = 1.0f;
        }

        for(auto ct = (*it).regiments.begin(); ct != (*it).regiments.end(); ct++) {
            if( currentBattleSide->oppressionStrength == 0.0f ) { continue; }
            float randomFactor = battleRandomDamageConstant - randomDamageDistribution(generator);
            float damage = ( 1 + randomFactor) *  powf((currentBattleSide->oppressionStrength/currentBattleSide->totalStrength), 1/battleAdvantageConstant ) * battleUnitDamageConstant;
            if((*ct)->recentlyMoved) {
                (*ct)->condition -= damage*disadvantage;
            }
            else {//UNTESTED PART OF CODE
                (*ct)->condition -= damage;
            }
        }
        for(auto ct = (*it).battleships.begin(); ct != (*it).battleships.end(); ct++) {
            if( currentBattleSide->oppressionStrength == 0.0f ) { continue; }
            float randomFactor = battleRandomDamageConstant - randomDamageDistribution(generator);
            float damage = ( 1 + randomFactor) *  powf((currentBattleSide->oppressionStrength/currentBattleSide->totalStrength), 1/battleAdvantageConstant ) * battleUnitDamageConstant;
            if((*ct)->recentlyMoved) {
                (*ct)->condition -= damage*disadvantage;
            }
            else {//UNTESTED PART OF CODE
                (*ct)->condition -= damage;
            }
        }
        //kill all units that have negative hitpoints
        for(auto ct = (*it).regiments.begin(); ct != (*it).regiments.end(); ct++) {
            if( (*ct)->condition <= 0.0f ) {
                Faction* unitFaction = (*ct)->getFaction();
                //remove this unit from the faction unit list
                //obscuring spaghetti code here :s
                for( auto kt = unitFaction->regiments.begin(); kt != unitFaction->regiments.end(); kt++ ) {
                    if( &(*kt) == (*ct) ) {
                        unitFaction->regiments.erase(kt);
                        break;
                    }
                }
                (*it).regiments.erase(ct);
            }
        }
        for(auto ct = (*it).battleships.begin(); ct != (*it).battleships.end(); ct++) {
            if( (*ct)->condition <= 0.0f ) {
                Faction* unitFaction = (*ct)->getFaction();
                //remove this unit from the faction unit list
                //obscuring spaghetti code here :s
                for( auto kt = unitFaction->battleships.begin(); kt != unitFaction->battleships.end(); kt++ ) {
                    if( &(*kt) == (*ct) ) {
                        unitFaction->battleships.erase(kt);
                        break;
                    }
                }
                (*it).battleships.erase(ct);
            }
        }
    }
}

void Island::battleSides(std::list<BattleSide>& sides) {
    for(auto it = garrisons.begin(); it != garrisons.end(); it++) {
        //if faction has no military presence on the island, it is not a contestant in a battle
        if( ( (*it).regiments.size() + (*it).battleships.size() == 0)  ) {
            continue;
        }
        BattleSide a;
        a.personalStrength = (*it).countBattleStrength();
        std::cout << "battle strength: " << a.personalStrength << "\n";
        a.totalStrength = 0.0f;
        a.owner = (*it).getFaction();
        for(auto ct = (*it).regiments.begin(); ct != (*it).regiments.end(); ct++) {
            a.totalStrength += (*ct)->condition/MAX_CONDITION;
        }
        for(auto ct = (*it).battleships.begin(); ct != (*it).battleships.end(); ct++) {
            a.totalStrength += (*ct)->condition/MAX_CONDITION;
        }
        sides.push_back(a);
    }
    //now battlesides and their personal strengths have been calculated. Commencing to finding all the allies of battlesides
    //this is poorly optimised but the code won't be bottlenecking here
    for(auto it = sides.begin(); it != sides.end(); it++) {
        for(auto ct = sides.begin(); ct != sides.end(); ct++ ) {
            if( (*it).owner->isAllyOf((*ct).owner) ) {
                (*it).addAlly(&(*ct));
            }
        }
    }
    //next we count the total strengths of the battleSides. This roughly means an equation: personal strength + ally strength - strength of allies
    // that are allied to someone else this battle side is fighting with
    for(auto it = sides.begin(); it != sides.end(); it++) {
        (*it).countTotalStrength(&sides);
        (*it).countOppressionStrength(&sides);
    }
}

bool Garrisonable::isClaimable(Faction* faction) {
    for(auto it = garrisons.begin(); it != garrisons.end(); it++) {
        if( (*it).getFaction() == faction ) {
            continue;
        }
        if( !(*it).isEmptyOfRegiments() ) {
            return false;
        }
    }
    return true;
}


void World::fightBattles() {
    std::cout << "World::Fighting battles!\n";
    for( auto it = rims.begin(); it != rims.end(); it++ ) {
        for(auto ct = (*it).islands.begin(); ct != (*it).islands.end(); ct++) {
            std::list<BattleSide> sides;
            (*ct).battleSides(sides);
            (*ct).damageUnits(sides);
        }
    }

}

std::list<Island*> World::getIslandLOSFilter(Faction* faction) {
    std::list<Island*> nullList;
    return nullList;
}

bool World::islandOnLOS(std::list<Island*>& filter) {
    return true;
}

void World::worldInStringFormatForFaction(std::string& stringWorld, Faction* LOSFaction) {
    if(unstarted) {
        stringWorld = MSG::GAME_NOT_STRATED_YET;
        return;
    }
    //make sure the string is empty
    stringWorld.clear();
    StrDP::StringDataStructure stringData;
    //! FIRST FS SEGMENT
    {
        StrDP::FS first;
        //This segment contains only unchanging information about the world.
        //baseRegimentSupply
        first.gs.push_back(std::to_string(baseRegimentSupply));
        //baseBattleshipSupply
        first.gs.push_back(std::to_string(baseBattleshipSupply));
        //regimentRecoveryRate
        first.gs.push_back(std::to_string(regimentRecoveryRate));
        //shipRecoveryRate
        first.gs.push_back(std::to_string(shipRecoveryRate));
        //factions
        StrDP::GS factionsData;
        for( auto it = factions.begin(); it != factions.end(); it++ ) {
            StrDP::RS factionData;
            //faction name
            factionData.us.push_back((*it).getName());
            //faction color
            factionData.us.push_back(std::to_string((*it).getColor()));
            //faction capital island name
            if((*it).getBaseIsland() == nullptr) {
                std::cerr << "Error in worldInStringFormat(): a faction had no base island!\n";
                stringWorld = MSG::SERVER_ERROR;
                return;
            }
            factionData.us.push_back((*it).getBaseIsland()->getName());
            factionsData.rs.push_back(factionData);
        }
        //first data section complete
        first.gs.push_back(factionsData);
        if( LOSFaction != nullptr ) {
            first.gs.push_back( LOSFaction->getName() );
        }
        else {
            std::cerr << "WARNING: possibly showing global vision in a game state request!\n";
            std::string noneString = "NONE";
            first.gs.push_back( noneString );
        }
        stringData.fs.push_back(first);
    }
    //! SECOND FS SEGMENT
    {
        //this segment contains all the players spesific data including preace treaties and treaty requests
        StrDP::FS second;
        //create the factions strings
        StrDP::GS treaties;
        //peace treaties already processed are saved in this blackList
        //there ought to be a better way to do this than this...
        std::vector<std::pair<Faction*, Faction*>> blackList;
        for(auto it = factions.begin(); it != factions.end(); it++) {
            for(auto ct = factions.begin(); ct != factions.end(); ct++) {
                //if faction *it is allied with faction *ct
                if( (*it).isAllyOf(&(*ct)) ) {
                    bool isBlackListed = false;
                    //if does not appear on blacklist
                    for(auto kt = blackList.begin(); kt != blackList.end(); kt++) {
                        if( ( (*kt).first == &(*it) && (*kt).second == &(*ct) ) || ( (*kt).first == &(*ct) && (*kt).second == &(*it) ) ) {
                            isBlackListed = true;
                        }
                    }
                    //if faction is ally and not on blacklist(that is, already listed in treaties)
                    if(!isBlackListed) {
                        StrDP::RS peace;
                        //use us separation for faction names
                        peace.us.push_back((*it).getName());
                        peace.us.push_back((*ct).getName());
                        //save treaty information as rs data
                        treaties.rs.push_back(peace);
                    }
                }
            }
        }
        second.gs.push_back(treaties);
        StrDP::GS requests;
        for(auto it = treatyRequests.begin(); it != treatyRequests.end(); it++) {
            StrDP::RS requestUnit;
            //save the faction names in the data as us fields
            requestUnit.us.push_back( (*it).first->getName() );
            requestUnit.us.push_back( (*it).second->getName() );
            requests.rs.push_back(requestUnit);
        }
        second.gs.push_back(requests);
        //turn data
        second.gs.push_back(std::to_string(turn));
        //second data section complete
        stringData.fs.push_back(second);
    }
    //! THIRD FS SEGMENT
    {
        //third FS covers the island data and unit data within the islands
        StrDP::FS third;
        //this data has faction spesific information, as only some islands are in the
        //LOS(Line Of Sight) of a faction
        //this filter is used in indentyfying the islands, that are not in the line
        //of sight, and those units are not displayed in the information
        std::list<Island*> LOSfilter = getIslandLOSFilter(LOSFaction);
        //handle rims as GS segments
        for(auto it = rims.begin(); it != rims.end(); it++) {
            StrDP::GS rimUnit;
            //each rim contains n amount of islands: these islands' data is stored over
            //multiple rs information, the last section contains troop information
            //the beginning of a new island data is known because there are fixed amount
            //of rs fields in an island
            for(auto ct = (*it).islands.begin(); ct != (*it).islands.end(); ct++) {

                //in the first RS is stored island name
                rimUnit.rs.push_back( (*ct).getName() );
                //following information is the island owner
                if((*ct).getOwner() == nullptr) { rimUnit.rs.push_back(StrDP::RS()); }
                else {
                    rimUnit.rs.push_back( (*ct).getOwner()->getName() );
                }
                //third section is for island unit data
                StrDP::RS unitSegment;
                //if island is not on faction LOS, the unit data section will be empty
                if(!islandOnLOS(LOSfilter)) {
                    rimUnit.rs.push_back(unitSegment);
                }
                //else if player has line of sight to the island, reveal all troops it occupies
                else {
                    for( auto kt = (*ct).garrisons.begin(); kt != (*ct).garrisons.end();kt++ ) {
                        for(auto lt = (*kt).regiments.begin(); lt != (*kt).regiments.end();lt++) {
                            //regiment name
                            unitSegment.us.push_back((*lt)->getName());
                            //regiment condition
                            unitSegment.us.push_back(std::to_string((*lt)->condition));
                            //regiment movement order (island name)
                            if( (*lt)->getTarget() == nullptr ) {
                                unitSegment.us.push_back( StrDP::US("") );
                            }
                            else {
                                unitSegment.us.push_back( (*lt)->getTarget()->getName() );
                            }
                        }
                        for(auto lt = (*kt).battleships.begin(); lt != (*kt).battleships.end();lt++) {
                            //battleship name
                            unitSegment.us.push_back((*lt)->getName());
                            //battleship condition
                            unitSegment.us.push_back(std::to_string((*lt)->condition));
                            //battleship movement order (island name)
                            if( (*lt)->getTarget() == nullptr ) {
                                unitSegment.us.push_back( StrDP::US("") );
                            }
                            else {
                                unitSegment.us.push_back( (*lt)->getTarget()->getName() );
                            }
                        }
                        for(auto lt = (*kt).spies.begin(); lt != (*kt).spies.end();lt++) {
                            //spy name
                            unitSegment.us.push_back((*lt)->getName());
                            //spy condition(spies have no condition, so this is an empty string)
                            unitSegment.us.push_back( StrDP::US() );
                            //spy movement order (island name)
                            if( (*lt)->getTarget() == nullptr ) {
                                unitSegment.us.push_back( StrDP::US("") );
                            }
                            else {
                                unitSegment.us.push_back( (*lt)->getTarget()->getName() );
                            }
                        }
                    }
                    rimUnit.rs.push_back(unitSegment);
                }
                //fourth and absolute final section is for linked islands data
                StrDP::RS linkedIslandData;
                std::list<Island*> linkedIslandsList = (*ct).getLinkedIslands();
                for(auto kt = linkedIslandsList.begin(); kt != linkedIslandsList.end();kt++) {
                    linkedIslandData.us.push_back( (*kt)->getName() );
                }
                rimUnit.rs.push_back(linkedIslandData);
            }
            third.gs.push_back(rimUnit);
        }
        //thid data section complete
        stringData.fs.push_back(third);
    }
    //! FOURTH FS SEGMENT
    {
        StrDP::FS fourth;
        //war declarations
        StrDP::GS declarations;
        for(auto it = warDeclarations.begin(); it != warDeclarations.end(); it++ ) {
            StrDP::RS declaration;
            declaration.us.push_back((*it).first->getName());
            declaration.us.push_back((*it).second->getName());
            declarations.rs.push_back( declaration );
        }
        fourth.gs.push_back(declarations);
        stringData.fs.push_back(fourth);
    }
    stringData.toString(stringWorld);
    stringData.printStringData();
}

std::string parseFactionCommand(std::string commandString, std::vector<std::string>& arguments, std::string& command, std::string& factionCode) {
    arguments.clear();
    //pick up the faction code part from the command string
    int firstPoint = commandString.find_first_of(':');
    if(firstPoint < 0) {
        return MSG::COMMAND_SYNTAX_WAS_INVALID;
    }
    factionCode = commandString.substr(0, firstPoint);
    commandString = commandString.substr(firstPoint+1);
    //pick up the command name from the command string
    int secondPoint = commandString.find_first_of(':');
    if(secondPoint < 0) {
        return MSG::COMMAND_SYNTAX_WAS_INVALID;
    }
    command = commandString.substr(0, secondPoint);
    commandString = commandString.substr(secondPoint+1);
    //pick up all the following arguments from the command string
    while(true) {
        //ARGUMENT SEPARATOR WILL BE CHANGED INTO SOME OTHER CHARACTER IN LATER DEVELOPMENT WHEN I'M NOT USING CONSOLES FOR SENDING MESSAGES
        int nextPoint = commandString.find_first_of('%');
        if(nextPoint < 0) {
            break;
        }
        arguments.push_back(commandString.substr(0, nextPoint));
        commandString = commandString.substr(nextPoint+1);
    }
    return "";
}

bool Faction::isAllyOf(Faction* faction) {
    if(faction == NULL) {
        std::cerr << "Error in Faction::isAllyOf(): a null pointer was passed into this function as argument\n";
    }
    for(auto it = allies.begin(); it != allies.end(); it++) {
        if( faction == (*it) ) {
            return true;
        }
    }
    return false;
}
