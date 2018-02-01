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
    //if uniqueNames os true, append some number to the end of the name, ensuring there will be no two identical names
    Namesystem(std::string nameFileName, bool _uniqueNames);

    std::string getName();

};

#endif
