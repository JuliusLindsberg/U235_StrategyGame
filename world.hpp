#ifndef WORLD_HPP
#define WORLD_HPP

#include <list>
#include <string>
#include <vector>
#include <iostream>
#include "names.hpp"
#include <stdexcept>
#include <random>
#include <cmath>

const float MAX_CONDITION = 100;
const unsigned startBattleshipAmount = 2;
const unsigned startRegimentAmount = 4;
const float battleAdvantageConstant = 1.2f;
//battleRandomDamageConstant is supposed to be between 0.0f and 1.0f!
const float battleRandomDamageConstant = 0.5f;
const float battleUnitDamageConstant = 20.0f;
const float battleAttackerDisadvantageConstant = 1.3;

const float dominanceToRegimentsFactor = 0.4f;
const float dominanceToBattleshipsFactor = 0.2f;

class Island;
class World;
class Faction;
class Garrisonable;

//base class for all the game elements in the game which have a name
class Identifiable {
    static unsigned nextID;
    unsigned id;
protected:
    std::string name;
public:
    Identifiable() {
        nextID++;
        id = Identifiable::nextID;
    }
    virtual void setDefaultName() {
        static unsigned leashNumber;
        leashNumber++;
        name = std::to_string(leashNumber);
    }
    void setName(std::string _name) {
        name = _name;
    }
    std::string getName() {
        return name;
    }
    unsigned getID() {
        return id;
    }
};

struct Movable: public Identifiable {
    //friend Faction;
private:
    Island* target;
    Island* position;
    Faction* faction;
public:
    bool recentlyMoved;
    bool hasTarget() {
        if( target ) {
            return true;
        }
        return false;
    }
    Island* getPosition() {
        return position;
    }
    bool hasPosition() {
        return position;
    }
    void setTarget(Island* _target) {
        target = _target;
    }
    void setPosition(Island* _position) {
        position = _position;
    }
    void setFaction(Faction* _faction) {
        faction = _faction;
    }
    Faction* getFaction() {
        return faction;
    }
    Island* getTarget() {
        return target;
    }
};

//can move anywhere on the map to aid someone in a battle
class Battleship: public Movable {
    friend Faction;
public:
    float condition;
};
//has the same fighting strength as a battleship has. However, regiments can only move one node at a time.
//Nuclear-attack to a factions Main-Node will cut off it's regiment support for x amount of turns.
class Regiment: public Movable {
    friend Faction;
public:
    float condition;
};
//can move anywhere on the map. Reveals information and Sabotages enemy information.
class Spy: public Movable {
    friend Faction;
public:
};

struct UnverifiedFaction {
    std::string name;
    std::string factionCode;
};

class Faction: public Identifiable {
    friend Island;
    friend World;
//private:
    std::list<Battleship> battleships;
    std::list<Regiment> regiments;
    std::list<Spy> spies;
    std::list<Faction*> allies;

    float newRegimentProgress;
    float newBattleshipProgress;

    std::string factionCode;
    Island* baseIsland;
    int regimentCounter;
    int battleshipCounter;
    int spyCounter;
    std::string getRegimentName() {
        regimentCounter++;
        return name + "#r" + std::to_string(regimentCounter);
    };
    std::string getBattleshipName() {
        battleshipCounter++;
        return name + "#b" + std::to_string(battleshipCounter);
    }
    std::string getSpyName() {
        spyCounter++;
        return name + "#s" + std::to_string(spyCounter);
    }

    Battleship* addBattleship(Island* targetIsland);

    Regiment* addRegiment(Island* targetIsland);

    Spy* addSpy(Island* targetIsland);

    void setFactionCode(std::string code) {
        factionCode = code;
    }

    void printTroopData();

public:

    void addAlly(Faction* newAlly) {
        allies.push_back(newAlly);
    }
    void removeAlly(Faction* ally) {
        for( auto it = allies.begin(); it != allies.end(); it++ ) {
            if( (*it) == ally ) {
                allies.remove(*it);
                return;
            }
        }
        std::cerr << "Error in Faction::deleteAlly(): ally to be deleted from ally list was not found!";
    }

    std::string getFactionCode() {
        return factionCode;
    }
    Faction(std::string _name) {
        regimentCounter = 0;
        battleshipCounter = 0;
        spyCounter = 0;
        newBattleshipProgress = 0.0f;
        newRegimentProgress = 0.0f;
        setName(_name);
    }
    void turnUpdate(float areaDominance, float baseRegimentSupply, float baseBattleshipSupply);
    int peekRegimentCounter() {
        return regimentCounter;
    }
    int peekBattleshipCounter() {
        return battleshipCounter;
    }
    int peekSpyCounter() {
        return spyCounter;
    }
    bool isAllyOf(Faction* faction) {
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
};


struct Garrison {
    Faction* faction;
    std::list<Battleship*> battleships;
    std::list<Regiment*> regiments;
    std::list<Spy*> spies;
    Garrison(Faction* owner) {
        faction = owner;
    }
    Faction* getFaction() {
        return faction;
    }
    bool isEmpty() {
        if( battleships.size() + regiments.size() ) {
            return false;
        }
        return true;
    }
    bool isEmptyOfRegiments() {
        if( regiments.size() ) {
            return false;
        }
        return true;
    }
    bool isEmptyOfBattleships() {
        if( battleships.size() ) {
            return false;
        }
        return true;
    }
    float countBattleStrength() {
        float strength = 0.0f;
        for(auto it = battleships.begin(); it != battleships.end(); it++) {
            strength += (*it)->condition/MAX_CONDITION;
            std::cout << "strength += " << (*it)->condition /MAX_CONDITION << "\n";
        }
        for(auto it = regiments.begin(); it != regiments.end(); it++) {
            strength += (*it)->condition/MAX_CONDITION;
        }
        return strength;
    }
};

struct Garrisonable {
    std::list<Garrison> garrisons;
    Faction* owner;
    void clearGarrisons() {
        for(std::list<Garrison>::iterator it = garrisons.begin(); it != garrisons.end(); it++) {
            clearGarrison(&*it);
        }
    }
    void clearGarrison(Garrison* garrison) {
        garrison->battleships.clear();
        garrison->regiments.clear();
        garrison->spies.clear();
    }
    //returns null if such garrison was not found or if passed pointer was null (in this case also prints an error report)
    Garrison* getGarrison(Faction* owner) {
        if(!owner) { std::cerr << "Warning: Null pointer was passed into a Garrisonable::getGarrison function call!\n"; return NULL; }
        for(std::list<Garrison>::iterator it = garrisons.begin(); it != garrisons.end(); it++) {
            if(((*it).faction) == owner) {
                return &(*it);
            }
        }
        return NULL;
    }
    //if there are no units at the
    bool isClaimable(Faction* faction);
    void attemptClaim(Faction* faction) {
        if( isClaimable(faction) ) {
            owner = faction;
        }
    }
    Faction* getOwner() {
        return owner;
    }
    //returns true if faction and unit was fround from the list. Otherwise returns false.
    bool popUnitFromList(Movable* unit) {
        //std::cout << "popping unit!\n";
        for(std::list<Garrison>::iterator it = garrisons.begin(); it != garrisons.end(); it++) {
            for(std::list<Regiment*>::iterator ct = (*it).regiments.begin(); ct != (*it).regiments.end(); ct++) {
                //std::cout << "comparing " << (*ct)->getName() << " to " << (unit->getName()) << "\n";
                if( (*ct)->getName() == (unit->getName()) ) {
                    //correct unit was found
                    //std::cout << "matched!\n";
                    (*it).regiments.erase(ct);
                    return true;
                }
            }
            for(std::list<Battleship*>::iterator ct = (*it).battleships.begin(); ct != (*it).battleships.end(); ct++) {
                if( (*ct)->getName() == (unit->getName()) ) {
                    //correct unit was found
                    (*it).battleships.erase(ct);
                    return true;
                }
            }
            for(std::list<Spy*>::iterator ct = (*it).spies.begin(); ct != (*it).spies.end(); ct++) {
                if( (*ct)->getName() == (unit->getName()) ) {
                    //correct unit was found
                    (*it).spies.erase(ct);
                    return true;
                }
            }
        }
        std::cerr << "Error in Garrisonable::popUnitFromList(): unit '" << unit->getName() << "' was not found from the garrisons\n";
        return false;
    }
    void addUnitToList(Regiment* unit, Faction* faction) {
        Garrison* garrison = getGarrison(faction);
        if(!garrison) {
            std::cerr << "Error in Garrisonable::addUnitToList(): Faction '" << faction->getName() << "' was not found among the garrisons\n";
            return;
        }
        garrison->regiments.push_back(unit);
        return;
    }

    void addUnitToList(Battleship* unit, Faction* faction) {
        Garrison* garrison = getGarrison(faction);
        if(!garrison) {
            std::cerr << "Error in Garrisonable::addUnitToList(): Faction '" << faction->getName() << "' was not found among the garrisons\n";
            return;
        }
        garrison->battleships.push_back(unit);
        return;
    }
    void addUnitToList(Spy* unit, Faction* faction) {
        Garrison* garrison = getGarrison(faction);
        if(!garrison) {
            std::cerr << "Error in Garrisonable::addUnitToList(): Faction '" << faction->getName() << "' was not found among the garrisons\n";
            return;
        }
        garrison->spies.push_back(unit);
        return;
    }
};

struct BattleSide {
    Faction* owner;
    std::list<BattleSide*> allies;
    bool defenderSide;
    float totalStrength;
    float personalStrength;
    float oppressionStrength;

    bool checkForAlly(Faction* faction) {
        for(auto it = allies.begin(); it != allies.end(); it++) {
            if((*it)->owner == faction) {
                return true;
            }
        }
        return false;
    }
    void addAlly(BattleSide* battleSide) {
        allies.push_back(battleSide);
    }
    bool isAllyOf(Faction* faction) {
        /*
        for(auto it = allies.begin(); it != allies.end(); it++) {
            if( (*it)->isAllyOf(faction) ) {
                return true;
            }
        }
        return false;*/
        return owner->isAllyOf(faction);
    }
    void countTotalStrength(std::list<BattleSide>* sides);
    void countOppressionStrength(std::list<BattleSide>* sides);
};

typedef std::list<Island*>::iterator IslandIterator;
typedef std::list<Island*> IslandList;

class Island: public Identifiable, public Garrisonable {
    std::list<Island*> linkedIslands;
    bool nukeIsland;
    bool baseIsland;
    int rimNumber;
public:
    Island(std::list<Faction*> factionsList, bool _nukeIsland, bool _baseIsland, int _rimNumber, Namesystem* islandNames) {
    name = islandNames->getName();
    rimNumber = _rimNumber;
    owner = NULL;
        for(std::list<Faction*>::iterator it = factionsList.begin(); it != factionsList.end(); it++) {
            garrisons.push_back(Garrison(*it));

        }
        linkedIslands.clear();
    }
    std::list<Island*> getLinkedIslands() {
        return linkedIslands;
    }
    bool isLinkedTo(Island* a) {
        //NOTE: UNTESTED FUNCTION!
        for(std::list<Island*>::iterator it = linkedIslands.begin(); it != linkedIslands.end(); it++) {
            if((*it) == a) {
                return true;
            }
        }
        return false;
    }
    bool isNukeIsland() {
        return nukeIsland;
    }
    bool isBaseIsland() {
        return baseIsland;
    }
    void setLinkedIslands(std::list<Island*> _linkedIslands) {
        linkedIslands = _linkedIslands;
    }
    void addNeighbour(Island* neighbour) {
        linkedIslands.push_back(neighbour);
    }
    int getRimNumber() {
        return rimNumber;
    }
    void setRimNumber(int _rimNumber) {
        rimNumber = _rimNumber;
    }

    void addTroops(int battleshipAmount, int regimentAmount, int spyAmount, Faction* faction);
    Island* searchNextNeighbour(IslandList blackList, bool adjacent, bool following, bool previous);
    void damageUnits(std::list<BattleSide>& battleSides);
    void battleSides(std::list<BattleSide>& sides);
};

class Rim: public Identifiable {
    friend World;
private:
    int number;
    std::list<Island> islands;
public:
    Rim(unsigned islandsAmount, std::list<Faction*> factionsList, bool nukeRim, bool baseRim, int _rimNumber, Namesystem* islandNames) {
        number = _rimNumber;
        setDefaultName();
        for(unsigned i = 0; i < islandsAmount; i++) {
            Island newIsland = Island(factionsList, nukeRim, baseRim, _rimNumber, islandNames);
            islands.push_back(newIsland);
        }
    }
    //get next island with connectionAmount anount of connections
    Island* getNextIsland(unsigned connectionsAmount);
    void debugPrint();
    virtual void setDefaultName() {
        name = "Rim " + std::to_string(number);
    }
    void setAdjacentIslands();
};

//world-class contains the topological information of the playable area
class World: public Identifiable {
private:
    int rimAmount;
    int playerAmount;
    int spyAmount;
    int falloutTime;
    int baseRegimentSupply;
    int baseBattleshipSupply;
    int regimentRecoveryRate;
    int shipRecoveryRate;
    Namesystem islandNames;
    std::list<Faction> factions;
    std::list<Rim> rims;
    std::vector<UnverifiedFaction> unverifiedFactions;
    std::list<std::pair<Faction*, Faction*>> treatyRequests;
    std::list<std::pair<Faction*, Faction*>> warDeclarations;
    bool unstarted;

    void completeInit() {
        return;
    }
    //void connectRimIslands() {
    //    Faction* neighbourFactions[playerAmount][2];
    //}
    void setAdjacentRims();
    void setFollowingRims();
public:
    World(): islandNames("names/islands.txt", 1)
    {
        unverifiedFactions.clear();
        unstarted = true;
    }
    bool isStarted() {
        return !unstarted;
    }

    std::string startWorld(unsigned _rimAmount, int islandness, int _falloutTime, float _baseRegimentSupply, float _baseBattleshipSupply,
    unsigned _spyAmount, int _regimentRecoveryRate, int _shipRecoveryRate, std::string worldName);

    float getDominance(Faction* faction);
    void setStartingTroops();
    void debugPrint();
    bool checkFactionCodeValidity(std::string factionCode);
    bool interpretCommand(std::string command);
    void handleDiplomacy();
    void peaceTreaty(Faction* a, Faction* b);
    void setWar(Faction* a, Faction* b);
    bool isDeclaringWar(Faction* subject, Faction* object);
    std::string declareWar(std::string command, std::vector<std::string> arguments);
    std::string endTurn();
    std::string handleServerCommand(std::string commandString);
    std::string handleNonFactionCommand(std::string commandString);
    std::string verifyFaction(std::string commandString);
    std::string addFaction(std::string commandString);
    std::string executeCommand(std::string factionCode, std::string command, std::vector<std::string> arguments);
    std::string moveTroop(std::string factionCode, std::vector<std::string> arguments);
    std::string handleCommand(std::string commandString);
    std::string factionTreatyRequest(std::string factionCode, std::vector<std::string> arguments);
    std::string addTreatyRequest(Faction* subject, Faction* object);
    Faction* getFactionFromCode(std::string factionCode);
    Faction* getFactionFromName(std::string factionName);
    Regiment* getRegimentFromName(std::string regimentName);
    Battleship* getBattleshipFromName(std::string battleshipName);
    Spy* getSpyFromName(std::string spyName);
    Island* getIslandFromName(std::string islandName);
    void fightBattles();
};

#endif
