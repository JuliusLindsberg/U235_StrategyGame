#ifndef _STRINGDATAPARSER_HPP
#define _STRINGDATAPARSER_HPP

#include <string>
#include <vector>
#include <iostream>
#include <SFML/System.hpp>

namespace StrDP {

    const unsigned FSCODE = 28;
    const unsigned GSCODE = 29;
    const unsigned RSCODE = 30;
    const unsigned USCODE = 31;

    //FS contains GS contains RS contains US
    //Names originate from:
    //FS = FILE SEPARATOR
    //GS = GROUP SEPARATOR
    //RS = RECORD SEPARATOR
    //US = UNIT SEPARATOR
    //They can be referenced as First, Second, Third and Fourth too, if that is easier to remember
    struct US {
        std::string data;
        US() {}
        US(std::string dataString) { data = dataString; }
    };

    struct RS {
        std::string data;
        std::vector<US> us;
        RS() {}
        RS(std::string dataString) { data = dataString; }
    };

    struct GS {
        std::string data;
        std::vector<RS> rs;
        GS() {}
        GS(std::string dataString) { data = dataString; }
    };

    struct FS {
        std::string data;
        std::vector<GS> gs;
        FS() {}
        FS(std::string dataString) { data = dataString; }
    };

    typedef FS First;
    typedef GS Second;
    typedef RS Third;
    typedef US Fourth;

    class StringDataStructure {
    public:
        std::vector<FS> fs;
        StringDataStructure() {}
        StringDataStructure(std::string& unparcedStringData) {
            parseString(unparcedStringData);
        }
        std::string parseString(std::string& stringData);
        void toString(std::string& returnString);
        void printStringData();
    };
};

































#endif
