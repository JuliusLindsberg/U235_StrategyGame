#ifndef NAMES_HPP
#define NAMES_HPP

#include <stdlib.h>
#include <fstream>
#include <vector>
#include <iostream>

int random(int R);

class Namesystem {
    std::vector<std::string> names;
    std::vector<int> used;
    //bool usedAlphabet[26];
public:
    bool uniqueNames;
    // jos uniqueNames on tosi, lis‰t‰‰n nimen loppuun jokin numero, varmistaen samalla ett‰ kahta samanlaista nime‰ ei tule
    Namesystem(std::string nameFileName, bool _uniqueNames) {
        uniqueNames = _uniqueNames;
        std::fstream file;
        file.open(nameFileName.c_str(), std::ios_base::in);
        if(file.is_open()) {
            std::string properName;
            properName.clear();
            while(getline(file, properName)) {
                names.push_back(properName);
            }
            file.close();
        }
        else {
            std::cerr << "Error in Namesystem: opening file " << nameFileName.c_str() << " failed, are you sure the textfile is in the right place?\n Prehaps you just forgot to add .txt to the end of the filename\n";
            std::cerr << "Creating 100 random ascii 'names' instead, due to file not opening properly.\n";
            for(unsigned a=0;a<100;a++) {
                std::string asciiName;
                asciiName.clear();
                for(unsigned i=0;i<6;i++) {
                    asciiName.push_back((char)random(250));
                }
                names.push_back(asciiName);
            }
        }

        //varmistetaan, ett‰ kaikki vektorin j‰senet ovat alustettuja
        used.reserve(names.size());
        for(unsigned i=0;i<names.size();i++) {
            used.push_back(0);
        }
    }

    std::string getName();

};





#endif
