
#include "stringdataparser.hpp"


std::string StrDP::StringDataStructure::parseString(std::string& stringData) {
    //changed from std::string to sf::string in order to support different fonts (at least a bit)better
    fs.clear();
    const std::string INVALID = "STRING WAS OF INVALID FORM";
    sf::String workString(stringData);
    //parse the string into a StringDataStructure
    //while FS symbols are
    int parseMode = StrDP::FSCODE;
    std::string* currentString = nullptr;
    if( *(workString.begin()) != StrDP::FSCODE ) {
        return INVALID;
    }
    for( sf::String::Iterator it = workString.begin(); it != workString.end(); it++ ) {
        //FS MODE FORBIDDEN CHARACTERS
        if(parseMode == StrDP::FSCODE) {
            if( (*it) == StrDP::RSCODE || (*it) == StrDP::USCODE ) {
                return INVALID;
            }
        }
        //GS MODE FORBIDDEN CHARACTERS
        else if( parseMode == StrDP::GSCODE) {
            if( (*it) == StrDP::USCODE ) {
                return INVALID;
            }
        }
        //FS CHARACTER
        if( (*it) == StrDP::FSCODE ) {
            fs.push_back(StrDP::FS());
            currentString = &fs.back().data;
            parseMode = StrDP::FSCODE;
        }
        //GS CHARACTER
        else if( (*it) == StrDP::GSCODE ) {
            fs.back().gs.push_back(StrDP::GS());
            currentString = &(fs.back().gs.back().data);
            parseMode = StrDP::GSCODE;
        }
        //RS CHARACTER
        else if( (*it) == StrDP::RSCODE ) {
            fs.back().gs.back().rs.push_back(StrDP::RS());
            currentString = &(fs.back().gs.back().rs.back().data);
            parseMode = StrDP::RSCODE;
        }
        //US CHARACTER
        else if( (*it) == StrDP::USCODE ) {
            fs.back().gs.back().rs.back().us.push_back(StrDP::US());
            currentString = &(fs.back().gs.back().rs.back().us.back().data);
            parseMode = StrDP::USCODE;
        }
        //if the character was one of the separator characters, will not save it into a string
        else {
            //add new character(s) to the std::string selected as currentString
            sf::String u8(*it);
            currentString->append( u8.toAnsiString() );
        }
    }
    return "";
}

void StrDP::StringDataStructure::toString(std::string& returnString) {
    returnString.clear();

    for(auto it = fs.begin(); it != fs.end(); it++) {
        returnString.push_back(StrDP::FSCODE);
        returnString.append((*it).data);
        for(auto ct = (*it).gs.begin(); ct != (*it).gs.end(); ct++) {
            returnString.push_back(StrDP::GSCODE);
            returnString.append((*ct).data);
            for( auto kt = (*ct).rs.begin(); kt != (*ct).rs.end(); kt++ ) {
                returnString.push_back(StrDP::RSCODE);
                returnString.append((*kt).data);
                for( auto lt = (*kt).us.begin(); lt != (*kt).us.end(); lt++ ) {
                    returnString.push_back(StrDP::USCODE);
                    returnString.append((*lt).data);
                }
            }
        }
    }
}

void StrDP::StringDataStructure::printStringData() {
    for(unsigned i = 0; i < fs.size(); i++) {
        std::cout << "FS " << i << " data: '";
        std::cout << fs[i].data << "'";
        for(unsigned c = 0; c < fs[i].gs.size(); c++) {
            std::cout << "    GS " << c << " data: '";
            std::cout << fs[i].gs[c].data << "'";
            for(unsigned k = 0; k < fs[i].gs[c].rs.size(); k++) {
                std::cout << "    RS " << k << " data: '";
                std::cout << fs[i].gs[c].rs[k].data << "'";
                for(unsigned l = 0; l < fs[i].gs[c].rs[k].us.size(); l++) {
                    std::cout << "    US " << l << " data: '";
                    std::cout << fs[i].gs[c].rs[k].us[l].data << "'";
                }
                std::cout << "\n";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }
}

