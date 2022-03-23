#include <iostream>
#include <fstream>
#include <string>

#define hashTableSize 10

//Struct containing all numerical information about one day of a certain share
struct kursdaten {
    std::string date;
    float open;
    float high;
    float low;
    float close;
    long int volume;
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

//Imports share-data from file, saves it in share-struct
void importData(std::string fileName, struct share* associatedShare) {
    //Loops as long as no file could be opened
    //Done to prevent shares being added without share data
    bool fileOpen = false;
    while (!fileOpen) {
        fileOpen = true;

        std::ifstream file("csv_data/" + fileName);

        if (file.is_open())
        {
            std::string line;
            int lineCounter = 0;
            //Loop for each line in csv
            while (getline(file, line)) {
                lineCounter++;

                //If counter is 31 (so we are on day 30 + first line of headers) break
                if (lineCounter > 31)
                    break;
                //If counter is 1 (so we are on first line of headers) go to next line
                else if (lineCounter == 1)
                    continue;

                //Probably not the best way of doing it, but whatever it works
                //Finds the first instance of a "," in the line-string, and saves the string in front of it into a new variable
                //(often that string also then gets convertet into a float with stof()),
                //then erases that part of the line-string + the comma, meaning we can repeat this until we are on the last entry, which is the volume
                std::string date = line.substr(0, line.find(","));
                line.erase(0, line.find(",") + 1);

                float open = std::stof(line.substr(0, line.find(",")));
                line.erase(0, line.find(",") + 1);

                float high = std::stof(line.substr(0, line.find(",")));
                line.erase(0, line.find(",") + 1);

                float low = std::stof(line.substr(0, line.find(",")));
                line.erase(0, line.find(",") + 1);

                float close = std::stof(line.substr(0, line.find(",")));
                line.erase(0, line.find(",") + 1);

                float adjClose = std::stof(line.substr(0, line.find(",")));
                line.erase(0, line.find(",") + 1);

                long int volume = std::stoi(line);

                //Creates a new struct from the data
                struct kursdaten newDay = {
                    date,
                    open,
                    high,
                    low,
                    close,
                    volume,
                    adjClose
                };
                //Adds that struct to the kursdaten array (-1 for line headers, -1 for index not counter = linecounter - 2)
                associatedShare->kursdaten[lineCounter - 2] = newDay;
            }
            file.close();
        }
        else {
            fileOpen = false;
            std::cout << "Unable to open file! Make sure the file is inside the folder 'csv_data'!" << std::endl;
            do {
                std::cout << "Full filename: ";
                std::cin >> fileName;
            } while (fileName.length() <= 0);
        }
    }
}

//DEBUGGING FUNCTION, DELETE LATER
//Prints all general information about all shares in array
void printAllShareData(struct share* kuerzelHashTable[hashTableSize])
{
    system("cls");

    for (int i = 0; i < hashTableSize; i++) {

        if (kuerzelHashTable[i] != NULL) {
            std::cout << "Name: " << kuerzelHashTable[i]->name << std::endl;
            std::cout << "Kuerzel: " << kuerzelHashTable[i]->kuerzel << std::endl;
            std::cout << "WKN: " << kuerzelHashTable[i]->wkn << std::endl;
            std::cout << "Data: " << std::endl;

            for (int day = 0; day < 30; day++) {
                struct kursdaten entry = kuerzelHashTable[i]->kursdaten[day];
                std::cout << entry.date << ": " << entry.open << " | " << entry.high << " | " << entry.low << " | " << entry.close << " | " << entry.adjClose << " | " << entry.volume << std::endl;
            }
            std::cout << std::endl;
        }
    }

    system("pause");
    system("cls");
}

//Adds a new share to the arrays
void add(struct share* kuerzelHashTable[hashTableSize], struct mapNameToKuerzel* nameHashTable[10])
{
    system("cls");

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

    importData(kuerzel + ".csv", newShare);

    //CHANGES ONCE HASH FUNCTION IS DONE, WORKS FOR NOW
    for (int i = 0; i < hashTableSize; i++) {
        if (kuerzelHashTable[i] == NULL && nameHashTable[i] == NULL) {
            kuerzelHashTable[i] = newShare;
            nameHashTable[i] = newMap;
            break;
        }
    }

    printAllShareData(kuerzelHashTable);
}

int main()
{
    //Initializes both hash tables - One is hashed with the kürzel, the other one with the name
    struct share* kuerzelHashTable[hashTableSize]{ NULL };
    struct mapNameToKuerzel* nameHashTable[hashTableSize]{ NULL };

    //Flag to stop looping if user wants to exit
    bool loop = true;

    while (loop) {
        system("cls");

        int option = 0;
        std::cout << "(1) - ADD | (2) - DEL | (3) - SEARCH | (4) - PLOT | (5) - SAVE | (6) - LOAD | (7) - QUIT" << std::endl;

        //Ask user to select as long as option is invalid
        do {
            std::cout << "Select: ";
            std::cin >> option;
        } while (option <= 0 || option > 7);

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
            clearHeap(kuerzelHashTable, nameHashTable);
            loop = false;
            break;
        }
    }
}
