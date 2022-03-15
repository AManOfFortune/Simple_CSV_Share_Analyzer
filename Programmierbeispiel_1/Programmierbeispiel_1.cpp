#include <iostream>

#define hashTableSize 10

//Struct containing all numerical information about one day of a certain share
struct kursdaten {
    std::string date;
    float open;
    float high;
    float low;
    float close;
    float volume;
    float adjClose;
};

//Struct containing all information about one share
struct share {
    std::string name;
    std::string wkn;
    std::string kuerzel;

    struct kursdaten kursdaten[30];
};

//Struct containing only kürzel
//Necessary so we can hash the name and arrive at the corresponding kürzel
//IS IMPROVEABLE, MAYBE WE CAN ONLY SAVE THE INDEX OF THE DATA IN OTHER ARRAY INSTEAD TO IMPROVE ACCESS SPEED
struct mapNameToKuerzel {
    std::string kuerzel;
};

//Deletes all structs from heap
void clearHeap(struct share* kuerzelHashTable[10], struct mapNameToKuerzel* nameHashTable[10]) {

    //Loops arrays once, deletes elements if they are not NULL
    for (int i = 0; i < hashTableSize; i++) {

        if(kuerzelHashTable[i] != NULL)
            delete kuerzelHashTable[i];

        if(nameHashTable[i] != NULL)
            delete nameHashTable[i];
    }
}

//DEBUGGING FUNCTION, DELETE LATER
//Prints all general information about all shares in array
void printAllAktienData(struct share* kuerzelHashTable[hashTableSize])
{
    for (int i = 0; i < hashTableSize; i++) {

        if (kuerzelHashTable[i] != NULL) {
            std::cout << "Name: " << kuerzelHashTable[i]->name << std::endl;
            std::cout << "Kuerzel: " << kuerzelHashTable[i]->kuerzel << std::endl;
            std::cout << "WKN: " << kuerzelHashTable[i]->wkn << "\n" << std::endl;
        }
    }
}

//Adds a new share to the arrays
void add(struct share* kuerzelHashTable[hashTableSize], struct mapNameToKuerzel* nameHashTable[10])
{
    std::string name = "";
    std::string wkn = "";
    std::string kuerzel = "";

    std::cout << "Enter name: ";
    std::cin >> name;

    std::cout << "Enter WKN: ";
    std::cin >> wkn;

    std::cout << "Enter kuerzel: ";
    std::cin >> kuerzel;

    struct share* newShare = new struct share();
    struct mapNameToKuerzel* newMap = new struct mapNameToKuerzel();

    newShare->name = name;
    newShare->wkn = wkn;
    newShare->kuerzel = kuerzel;

    newMap->kuerzel = kuerzel;

    //CHANGES ONCE HASH FUNCTION IS DONE, WORKS FOR NOW
    for (int i = 0; i < hashTableSize; i++) {
        if (kuerzelHashTable[i] == NULL && nameHashTable[i] == NULL) {
            kuerzelHashTable[i] = newShare;
            nameHashTable[i] = newMap;
            break;
        }
    }

    printAllAktienData(kuerzelHashTable);
}

int main()
{
    //Initializes both hash tables - One is hashed with the kürzel, the other one with the name
    struct share* kuerzelHashTable[hashTableSize]{ NULL };
    struct mapNameToKuerzel* nameHashTable[hashTableSize]{ NULL };

    //Flag to stop looping if user wants to exit
    bool loop = true;

    while (loop) {

        int option = 0;
        std::cout << "(1) - ADD | (2) - DEL | (3) - IMPORT | (4) - SEARCH | (5) - PLOT | (6) - SAVE | (7) - LOAD | (8) - QUIT" << std::endl;

        //Ask user to select as long as option is invalid
        do {
            std::cout << "Select: ";
            std::cin >> option;
        } while (option <= 0 || option > 8);

        switch (option)
        {
        case 1:
            add(kuerzelHashTable, nameHashTable);
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        case 7:
            break;
        case 8:
            clearHeap(kuerzelHashTable, nameHashTable);
            loop = false;
            break;
        }
    }
}
