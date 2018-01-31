#include "names.hpp"

std::string Namesystem::getName() {

    if(names.size() < 1) {
        std::cerr << "Error in Namesystem, getName(): requested a name when names.size() is less than 1(there are no names in the list to choose from).\n";
        return "ERROR";
    }

    if(!uniqueNames) {
        //std::cout << "not using unique names!\n";
        int nameIndex = random(names.size()-1);
        return names[nameIndex];
    }

    else {
        //std::cout << "using unique names!\n";
        // seuraavat koodin kuvailemattomat numeroarvot liittyvät ascii-koodistoon ja englanninkielisiin aakkosiin
        unsigned nameIndex = random(names.size()-1);
        std::string nextName;
        used[nameIndex]++;
        if(used[nameIndex] != 1) {
            //std::cout << "used[nameIndex != 1]\n";
            //tämä varmistaa, että nimet eivät koskaan ole samanlaisia, eivätkä myöskään mene yli charin numeromäärän
            //tosin nimien esteettinen vaikutus saattaa kärsiä tästä, mutta jos tämä varotoimenpide tulee tarpeeseen, on nimiä ollut jo muutenkin liian vähän
            if(used[nameIndex] > 56) {
                names[nameIndex] = "New " + names[nameIndex];
                used[nameIndex] = 0;
            }
            //symbol 65 in ascii means A and other aphabet comes right after that
            char nameAdditionChar = 63+used[nameIndex];
            nextName = names[nameIndex] + ' ' + nameAdditionChar;
        }
        else {
            //std::cout << "else\n";
            nextName = names[nameIndex];
            /*
            what was this code doing right here???
            char transformingChar;
            //nextName = names[nameIndex];
            // 2 jos nimi on käytetty aikaisemmin, nimetään uusi [x 2]:ksi
            if(used[nameIndex] < 3) {
                transformingChar = (char)used[nameIndex];
            }
            //jos on enemmän kuin 2 instanssia, käytetään nimeämiseen isoja aakkosia
            else if(used[nameIndex] > 2 && used[nameIndex] < 30) {
                transformingChar = (char)used[nameIndex]+57;
            }
            //kun isot aakkoset ovat loppuneet, käytetään loput char-8 sopivat luvut
            //jos nämä lisänimet eivät riitä, joko on käynyt ERITTÄIN huono tuuri nimilistan käytön kanssa, tai nimiä on vain liian vähän.
            //kuitenkin, koska nimijärjestelmä on luvannut samannimisien olevan mahdottomuus, laitetaan tällaisessa tilanteessa alkupereäiseen nimeen New-lisäliite,
            //ja alotetaan numero ja aakkoslisätunnusten iterointi alusta uniikkiuden varmistamiseksi.
            // Esim. New New New New Washington 15 olisi mahdollinen nimi tällä nimisysteemillä, jos Washington oli alkuperäisessä nimilistassa
            else {
                transformingChar = (char)used[nameIndex]-26;
            }
            nextName = names[nameIndex]+" " + transformingChar;
            */
        }
        //std::cout << "getName() returning: '" << nextName << "'\n";
        return nextName;
    }
}
