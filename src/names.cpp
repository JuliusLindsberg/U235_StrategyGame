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
        // seuraavat koodin kuvailemattomat numeroarvot liittyv�t ascii-koodistoon ja englanninkielisiin aakkosiin
        unsigned nameIndex = random(names.size()-1);
        std::string nextName;
        used[nameIndex]++;
        if(used[nameIndex] != 1) {
            //std::cout << "used[nameIndex != 1]\n";
            //t�m� varmistaa, ett� nimet eiv�t koskaan ole samanlaisia, eiv�tk� my�sk��n mene yli charin numerom��r�n
            //tosin nimien esteettinen vaikutus saattaa k�rsi� t�st�, mutta jos t�m� varotoimenpide tulee tarpeeseen, on nimi� ollut jo muutenkin liian v�h�n
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
            // 2 jos nimi on k�ytetty aikaisemmin, nimet��n uusi [x 2]:ksi
            if(used[nameIndex] < 3) {
                transformingChar = (char)used[nameIndex];
            }
            //jos on enemm�n kuin 2 instanssia, k�ytet��n nime�miseen isoja aakkosia
            else if(used[nameIndex] > 2 && used[nameIndex] < 30) {
                transformingChar = (char)used[nameIndex]+57;
            }
            //kun isot aakkoset ovat loppuneet, k�ytet��n loput char-8 sopivat luvut
            //jos n�m� lis�nimet eiv�t riit�, joko on k�ynyt ERITT�IN huono tuuri nimilistan k�yt�n kanssa, tai nimi� on vain liian v�h�n.
            //kuitenkin, koska nimij�rjestelm� on luvannut samannimisien olevan mahdottomuus, laitetaan t�llaisessa tilanteessa alkupere�iseen nimeen New-lis�liite,
            //ja alotetaan numero ja aakkoslis�tunnusten iterointi alusta uniikkiuden varmistamiseksi.
            // Esim. New New New New Washington 15 olisi mahdollinen nimi t�ll� nimisysteemill�, jos Washington oli alkuper�isess� nimilistassa
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
