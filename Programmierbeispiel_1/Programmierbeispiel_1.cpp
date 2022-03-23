#include <iostream>
#include <fstream>
#include <string>

#define HASH_TABLE_SIZE 2003

#define RED_BG "\033[41m"
#define GREEN_BG "\033[42m"
#define RESET "\033[m"

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

//Struct containing kürzel & name
//Necessary so we can hash the name and arrive at the corresponding kürzel
//IS IMPROVEABLE, MAYBE WE CAN ONLY SAVE THE INDEX OF THE DATA IN OTHER ARRAY INSTEAD TO IMPROVE ACCESS SPEED
struct mapNameToKuerzel {
    std::string name;
    std::string kuerzel;
};

//Deletes all structs from heap
void clearHeap(struct share* kuerzelHashTable[HASH_TABLE_SIZE], struct mapNameToKuerzel* nameHashTable[HASH_TABLE_SIZE]) {

    //Loops arrays once, deletes elements if they are not NULL
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {

        if(kuerzelHashTable[i] != NULL)
            delete kuerzelHashTable[i];

        if(nameHashTable[i] != NULL)
            delete nameHashTable[i];
    }
}

//TEMPORARY HASH FUNCTION
int hashFunc(std::string str) {
    //Get ASCII values of string
    int sum = 0;
    for (std::string::size_type i = 0; i < str.size(); ++i) {
        sum = sum + int(str[i]);
    }
    //-1, because array has on last index '\0'
    int hashIndex = (sum * (HASH_TABLE_SIZE / 360)) % HASH_TABLE_SIZE;

    return hashIndex;
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
//Prints all information about all shares in hash table
void printAllShareData(struct share* kuerzelHashTable[HASH_TABLE_SIZE])
{
    system("cls");

    for (int i = 0; i < HASH_TABLE_SIZE; i++) {

        if (kuerzelHashTable[i] != NULL) {
            std::cout << "Index: " << i << std::endl;
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
void add(struct share* kuerzelHashTable[HASH_TABLE_SIZE], struct mapNameToKuerzel* nameHashTable[HASH_TABLE_SIZE])
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

    //Initializes and fills the share and map structs
    struct share* newShare = new struct share();
    struct mapNameToKuerzel* newMap = new struct mapNameToKuerzel();

    newShare->name = name;
    newShare->wkn = wkn;
    newShare->kuerzel = kuerzel;

    newMap->kuerzel = kuerzel;
    newMap->name = name;

    //Imports (and saves) the data to the newShare struct
    importData(kuerzel + ".csv", newShare);

    //Saves both structs inside their respective hash tables
    kuerzelHashTable[hashFunc(kuerzel)] = newShare;
    nameHashTable[hashFunc(name)] = newMap;

    //DEBUGGING ONLY, REMOVE BEFORE FINAL SUBMIT
    printAllShareData(kuerzelHashTable);
}

//Searches for either a name or a kuerzel in the corresponding hash table, returns NULL if nothing is found
struct share* search(struct share* kuerzelHashTable[HASH_TABLE_SIZE], struct mapNameToKuerzel* nameHashTable[HASH_TABLE_SIZE])
{
    //Asks for the input mode
    std::cout << "Input (n)ame or (k)uerzel?" << std::endl;
    char mode = ' ';
    do {
        std::cout << "Choose: ";
        std::cin >> mode;
    } while (mode != 'n' && mode != 'k');

    //Gets initialized differently depending on input mode
    std::string kuerzel;

    //If user wants to input a name, ask it, look for it and save the associated kuerzel if it is found
    if (mode == 'n') {
        //Ask the user for the name
        std::string name;
        do {
            std::cout << "Name: ";
            std::cin >> name;
        } while (name.length() <= 0);

        //Search for that name in the hash table
        struct mapNameToKuerzel* result = nameHashTable[hashFunc(name)];

        //If result is NULL immediately, return NULL
        if (result == NULL)
            return NULL;
        //Otherwise check if result is correct, if it is not, do cubic jumping
        else if (result->name != name) {
            //INSERT CUBIC JUMPING HERE
        }

        kuerzel = result->kuerzel;
    }
    //Otherwise simply ask the user to input the kuerzel
    else {
        //Ask the user for the kuerzel
        do {
            std::cout << "Kuerzel: ";
            std::cin >> kuerzel;
        } while (kuerzel.length() <= 0);
    }

    //Search for the kuerzel in the hash table
    struct share* result = kuerzelHashTable[hashFunc(kuerzel)];

    //If result is NULL immediately, return NULL
    if (result == NULL)
        return NULL;
    //Otherwise check if result is correct, if it is not, do cubic jumping
    else if (result->kuerzel != kuerzel) {
        //INSERT CUBIC JUMPING HERE
    }

    //Return the result
    return result;
}

//Displays the search results
void displaySearchResults(struct share* result) 
{
    system("cls");

    if (result != NULL) {
        std::cout << "Name: " << result->name << std::endl;
        std::cout << "Kuerzel: " << result->kuerzel << std::endl;
        std::cout << "WKN: " << result->wkn << std::endl;
        std::cout << "Data: " << std::endl;

        struct kursdaten entry = result->kursdaten[0];
        std::cout << entry.date << ": " << entry.open << " | " << entry.high << " | " << entry.low << " | " << entry.close << " | " << entry.adjClose << " | " << entry.volume << "\n" << std::endl;
    }
    else std::cout << "Entry not found!\n" << std::endl;

    system("pause");
    system("cls");
}

//Terribly inefficient way to delete entry (at least O(n)), so maybe improveable if we have time, good enough for now
void deleteEntry(struct share* entryToDelete, struct share* kuerzelHashTable[HASH_TABLE_SIZE], struct mapNameToKuerzel* nameHashTable[HASH_TABLE_SIZE])
{
    if (entryToDelete != NULL) {
        //Variable to stop loop after both entries have been found
        int success = 0;

        //Loop through both hash tables
        for (int i = 0; i < HASH_TABLE_SIZE && success < 2; i++) {
            //If entry is found
            if (entryToDelete == kuerzelHashTable[i]) {
                //First, delete current entry
                delete kuerzelHashTable[i];

                //Second, create a new dummy entry that should never trigger if one searches for a kuerzel
                struct share* dummyStruct = new struct share();
                dummyStruct->kuerzel = "EntryDeleted";

                //Third, add that dummy entry to the spot of the old entry
                kuerzelHashTable[i] = dummyStruct;

                //Lastly, increment success
                success++;
            }

            //Same logic as above, just for other struct type
            if (nameHashTable[i] != NULL && entryToDelete->kuerzel == nameHashTable[i]->kuerzel) {
                delete nameHashTable[i];

                struct mapNameToKuerzel* dummyStruct = new struct mapNameToKuerzel();
                dummyStruct->name = "EntryDeleted";

                nameHashTable[i] = dummyStruct;

                success++;
            }
        }

        //If both entries were deleted, print message
        if (success >= 2)
            std::cout << "Delete successful\n" << std::endl;
        else
            std::cout << "Oh oh, something went wrong! Please exit the program without saving.\n" << std::endl;
    }
    else std::cout << "Entry not found!\n" << std::endl;

    system("pause");
}

int main()
{
    //Initializes both hash tables - One is hashed with the kürzel, the other one with the name
    struct share* kuerzelHashTable[HASH_TABLE_SIZE]{ NULL };
    struct mapNameToKuerzel* nameHashTable[HASH_TABLE_SIZE]{ NULL };

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
                deleteEntry(search(kuerzelHashTable, nameHashTable), kuerzelHashTable, nameHashTable);
                break;
            case 3:
                displaySearchResults(search(kuerzelHashTable, nameHashTable));
                break;
            case 4:
                break;
            case 5:
                break;
            case 6:
                break;
            case 7:
                loop = false;
                break;
        }
    }

    clearHeap(kuerzelHashTable, nameHashTable);
    return 0;
}
