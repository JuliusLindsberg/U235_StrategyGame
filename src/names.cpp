#include "names.hpp"

std::string Namesystem::getName() {

    if(names.size() < 1) {
        std::cerr << "Error in Namesystem, getName(): requested a name when names.size() is less than 1(there are no names in the list to choose from).\n";
        return "ERROR";
    }

    if(!uniqueNames) {
        int nameIndex = random(names.size()-1);
        return names[nameIndex];
    }

    else {
        unsigned nameIndex = random(names.size()-1);
        std::string nextName;
        used[nameIndex]++;
        if(used[nameIndex] != 1) {
            //this ensures that there will no be two identical names, now will they ever go over the numeric value of 8-bit char
            //however, the aesthetical character of such names might suffer, but this precautionary step will practically only happen when there are not enough names present in list
            if(used[nameIndex] > 56) {
                names[nameIndex] = "New " + names[nameIndex];
                used[nameIndex] = 0;
            }
            //symbol 65 in ascii means A and other aphabet comes right after that
            char nameAdditionChar = 63+used[nameIndex];
            nextName = names[nameIndex] + ' ' + nameAdditionChar;
        }
        else {
            nextName = names[nameIndex];
        }
        return nextName;
    }
}

Namesystem::Namesystem(std::string nameFileName, bool _uniqueNames) {
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

    //we ensure that every element of the vector has been initialised
    used.reserve(names.size());
    for(unsigned i=0;i<names.size();i++) {
        used.push_back(0);
    }
}
