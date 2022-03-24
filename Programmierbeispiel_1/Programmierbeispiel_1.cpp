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

    int hashIndex = ((sum - 195) * HASH_TABLE_SIZE / (360 - 195)) % HASH_TABLE_SIZE;

    return hashIndex;
}

namespace probing {
    //Function returs the share corresponding to the kuerzel, or NULL if not found
    struct share* quadraticProbingForSearchingKuerzel(std::string kuerzel, struct share* stocks[HASH_TABLE_SIZE]) {
        int i = 1;
        int hashIndex = hashFunc(kuerzel);
        while (stocks[hashIndex] != NULL) {
            //Quadratic probing
            if (stocks[hashIndex]->kuerzel == kuerzel) {
                return stocks[hashIndex];
            }
            hashIndex = (hashIndex + (i * i)) % HASH_TABLE_SIZE;
            i++;
        }
        //return if index stock[hashIndex] == NULL -> not found
        return NULL;
    }

    //quadraticProbingForSearchingName gets different struct as parameter & returns the corresponding kuerzel if it is found
    std::string quadraticProbingForSearchingName(std::string name, struct mapNameToKuerzel* stocks[HASH_TABLE_SIZE]) {
        int i = 1;
        int hashIndex = hashFunc(name);
        while (stocks[hashIndex] != NULL) {
            //Quadratic probing
            if (stocks[hashIndex]->name == name) {
                return stocks[hashIndex]->kuerzel;
            }
            hashIndex = (hashIndex + (i * i)) % HASH_TABLE_SIZE;
            i++;
        }
        //return if index stock[hashIndex] == NULL -> no value
        return "";
    }

    //Returns the first empty index
    int quadraticProbingForAddingKuerzel(std::string kuerzel, struct share* stocks[HASH_TABLE_SIZE]) {
        int i = 1;
        int hashIndex = hashFunc(kuerzel);

        while (stocks[hashIndex] != NULL) {
            //Quadratic probing
            hashIndex = (hashIndex + i * i) % HASH_TABLE_SIZE;
            i++;
        }

        return hashIndex;
    }

    //Exact same as function above just for other hash table
    int quadraticProbingForAddingName(std::string name, struct mapNameToKuerzel* stocks[HASH_TABLE_SIZE]) {
        int i = 1;
        int hashIndex = hashFunc(name);

        while (stocks[hashIndex] != NULL) {
            //Quadratic probing
            hashIndex = (hashIndex + i * i) % HASH_TABLE_SIZE;
            i++;
        }

        return hashIndex;
    }
}

//Helper function for importData
int countLines(std::string fileName) {
    std::ifstream file("csv_data/" + fileName);

    //Count how many lines the file has
    int count = 0;
    if (file.is_open()) {
        std::string line;
        while (getline(file, line))
        {
            ++count;
        }
        file.close();

        return count;
    }
    //If the file could not be opened, return -1
    else {
        return -1;
    }
}

//Imports share-data from file, saves it in share-struct
void importData(std::string fileName, struct share* associatedShare) {
    //Loops as long as no file could be opened
    //Done to prevent shares being added without share data
    int lineCount = -1;
    while (lineCount == -1) {
        //Counts the lines in a file, if file is not found return -1 and continue looping
        lineCount = countLines(fileName);

        std::ifstream file("csv_data/" + fileName);

        if (file.is_open())
        {
            std::string line;

            //Reads and discards all lines except the last 30
            //Needs to be done because the most recent entries are at the end of the file, so we need the last 30 lines
            for (int i = 0; i < lineCount - 30; i++) {
                getline(file, line);
            }

            int index = 0;
            //Loop for each remaining line in csv
            while (getline(file, line)) {
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
                associatedShare->kursdaten[index] = newDay;
                //Increment index
                index++;
            }
            file.close();
        }
        else {
            std::cout << "Unable to open file! Make sure the file is inside the folder 'csv_data'!" << std::endl;
            do {
                std::cout << "Full filename: ";
                std::cin >> fileName;
            } while (fileName.length() <= 0);
        }
    }
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

    //Saves both structs inside their respective hash tables at the index returned by quadratic probing
    kuerzelHashTable[probing::quadraticProbingForAddingKuerzel(kuerzel, kuerzelHashTable)] = newShare;
    nameHashTable[probing::quadraticProbingForAddingName(name, nameHashTable)] = newMap;

    std::cout << "Share successfully added!\n" << std::endl;
    system("pause");
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

        //Look through hash table if we can find the kuerzel, if not, return NULL
        kuerzel = probing::quadraticProbingForSearchingName(name, nameHashTable);
        if (kuerzel == "") {
            return NULL;
        }
    }
    //Otherwise simply ask the user to input the kuerzel
    else {
        //Ask the user for the kuerzel
        do {
            std::cout << "Kuerzel: ";
            std::cin >> kuerzel;
        } while (kuerzel.length() <= 0);
    }

    //Return the found struct or NULL if not found
    return probing::quadraticProbingForSearchingKuerzel(kuerzel, kuerzelHashTable);
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

        //Most recent entry is the last one in the array
        struct kursdaten entry = result->kursdaten[29];
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

//Helper function for plot
//Returns the digits of a given number
int getDigits(int number) {
    int digits = 0;

    do {
        number /= 10;
        digits++;
    } while (number != 0);

    return digits;
}

//Prints a simple candlestick chart for a single share
void plot(struct share* shareToPlot)
{
    system("cls");

    if (shareToPlot != NULL) {
        std::cout << "Name: " << shareToPlot->name << std::endl;
        std::cout << "Kuerzel: " << shareToPlot->kuerzel << std::endl;
        std::cout << "WKN: " << shareToPlot->wkn << std::endl;

        //First, save what are the highest and lowest values of high/low
        float highest = 0;
        float lowest = 0;
        for (int i = 0; i < 30; i++) {
            struct kursdaten day = shareToPlot->kursdaten[i];

            //On the first day, properly initialize highest and lowest
            if (i == 0) {
                lowest = day.low;
                highest = day.high;
            }

            //On all other days, save any value that was higher/lower than the current highest/lowest
            if (day.high > highest)
                highest = day.high;
            if (day.low < lowest)
                lowest = day.low;
        }

        //Next, we can calculate the top and bottom boundaries of the chart
        //This is the space we will have between the border and the highest/lowest points of our chart
        //Currently, we set that space to be 1% on each side
        float topBoundary = static_cast<float>(highest * 1.01);
        float bottomBoundary = static_cast<float>(lowest * 0.99);

        //We also need to know how many digits each of those numbers has to print them properly later
        int topBoundaryDigits = getDigits(static_cast<int>(topBoundary));
        int bottomBoundaryDigits = getDigits(static_cast<int>(bottomBoundary));

        //Now we specify the width (in characters) the chart takes up on our screen
        int chartWidth = 100;

        //With all those calculations done, we can start printing our chart!
        //First, we print a line with our top and bottom boundary numbers
        std::cout << "             " << static_cast<int>(bottomBoundary);
        for (int i = 0; i < chartWidth - bottomBoundaryDigits - topBoundaryDigits; i++) {
            std::cout << " ";
        }
        std::cout << static_cast<int>(topBoundary) << std::endl;

        //Then we print the top line of the chart area
        std::cout << "            +";
        for (int i = 0; i < chartWidth; i++) {
            std::cout << "-";
        }
        std::cout << "+" << std::endl;

        //Next, we print the actual data
        for (int i = 0; i < 30; i++) {
            struct kursdaten day = shareToPlot->kursdaten[i];

            //Change the color of the bar depending if the close was higher than the low
            bool green = day.close > day.open ? true : false;

            //We need to scale the open/close value of the day with the chart width & its boundaries to find the starting/ending characters
            int openScaled = static_cast<int>((day.open - bottomBoundary) * chartWidth / (topBoundary - bottomBoundary));
            int closeScaled = static_cast<int>((day.close - bottomBoundary) * chartWidth / (topBoundary - bottomBoundary));

            //Same with the high/low
            int highScaled = static_cast<int>((day.high - bottomBoundary) * chartWidth / (topBoundary - bottomBoundary));
            int lowScaled = static_cast<int>((day.low - bottomBoundary) * chartWidth / (topBoundary - bottomBoundary));

            //Now we can start printing by printing the date and a seperator
            std::cout << day.date << "  |";

            //Next we print characters for the whole chart width
            for (int width = 0; width < chartWidth; width++) {
                //If close is higher than open and we are between the two, make background green
                if (green && width >= openScaled && width <= closeScaled)
                    std::cout << GREEN_BG;
                //If open is higher than close and we are between the two, make background red
                else if (!green && width <= openScaled && width >= closeScaled)
                    std::cout << RED_BG;

                //If we are between the high and low, print a dash, else print a space
                if (width >= lowScaled && width <= highScaled)
                    std::cout << "-";
                else
                    std::cout << " ";

                //Reset the colors (does not do anything if we have no colors)
                std::cout << RESET;
            }
            //Print a closing seperator at the end
            std::cout << "|" << std::endl;
        }

        //Lastly, we print the closing line
        std::cout << "            +";
        for (int i = 0; i < chartWidth; i++) {
            std::cout << "-";
        }
        std::cout << "+\n" << std::endl;
    }
    else std::cout << "Share not found!\n" << std::endl;

    system("pause");
    system("cls");
}

//Saves both hash tables into two files
void saveData(struct share* kuerzelHashTable[HASH_TABLE_SIZE], struct mapNameToKuerzel* nameHashTable[HASH_TABLE_SIZE]) {
    // Open and overwrite (or create a new) text file
    std::ofstream stockFile("kuerzelHashTable.txt", std::ofstream::trunc);

    //If that was successfull, save all data into it
    if (stockFile.is_open()) {
        // Write to the file
        for (int i = 0; i < HASH_TABLE_SIZE; i++) {
            //Saving format is as follows:
            // 
            //NULL
            //NULL
            //<name>|<kuerzel>|<wkn>:<date>|<open>| ... |<volume>;<date>|<open> ...
            //NULL
            //...
            //
            //This means that for every spot in the hash table, 1 line with either NULL or data exists in the file

            //If data is not NULL, save data as described above
            if (kuerzelHashTable[i] != NULL) {
                stockFile << kuerzelHashTable[i]->name;
                stockFile << "|" << kuerzelHashTable[i]->kuerzel;
                stockFile << "|" << kuerzelHashTable[i]->wkn;
                stockFile << ":";

                for (int day = 0; day < 30; day++) {
                    struct kursdaten entry = kuerzelHashTable[i]->kursdaten[day];
                    stockFile << entry.date << "|" << entry.open << "|" << entry.high << "|" << entry.low << "|" << entry.close << "|" << entry.adjClose << "|" << entry.volume << ";";
                }
                stockFile << "\n";
            }
            //Otherwise save the string "NULL"
            else {
                stockFile << "NULL\n";
            }
        }

        //Print success message
        std::cout << "Data successfully saved into \"kuerzelHashTable.txt\"" << std::endl;
        // Close the file
        stockFile.close();
    }
    else {
        std::cout << "Unable to create file \"kuerzelHashTable.txt\"" << std::endl;
    }


    //Same thing as above, just for the other hashTable
    //For comments on how this works see above
    std::ofstream nameFile("nameHashTable.txt", std::ofstream::trunc);

    if (nameFile.is_open()) {
        // Write to the file
        for (int i = 0; i < HASH_TABLE_SIZE; i++) {
            //Saving format is as follows:
            // 
            //NULL
            //NULL
            //<name>|<kuerzel>
            //NULL
            //...

            if (nameHashTable[i] != NULL) {
                nameFile << nameHashTable[i]->name << "|" << nameHashTable[i]->kuerzel << "\n";
            }
            else {
                nameFile << "NULL\n";
            }
        }

        //Print success message
        std::cout << "Data successfully saved into \"nameHashTable.txt\"" << std::endl;
        // Close the file
        nameFile.close();
    }
    else {
        std::cout << "Unable to create file \"nameHashTable.txt\"" << std::endl;
    }

    std::cout << std::endl;
    system("pause");
}

//Fills both hash tables with data from two files
void loadData(struct share* kuerzelHashTable[HASH_TABLE_SIZE], struct mapNameToKuerzel* nameHashTable[HASH_TABLE_SIZE]) {
    //Open file
    std::ifstream stockFile("kuerzelHashTable.txt");
    //If it is opened, read data
    if (stockFile.is_open())
    {
        //Before we start loading (and potentially overwriting the already existing) shares, clear the heap once
        clearHeap(kuerzelHashTable, nameHashTable);

        std::string line;

        //Index in hashTable corresponds directly to each line in the file, so we can simply increment an index and always have the correct position
        int i = 0;
        while (getline(stockFile, line)) {
            //If the line reads "NULL", the corresponding index in the hashTable is NULL
            if (line == "NULL")
                kuerzelHashTable[i] = NULL;
            else {
                //For information on what this does and how this works see importData() function above
                std::string name = line.substr(0, line.find("|"));
                line.erase(0, line.find("|") + 1);

                std::string kuerzel = line.substr(0, line.find("|"));
                line.erase(0, line.find("|") + 1);

                std::string wkn = line.substr(0, line.find(":"));
                line.erase(0, line.find(":") + 1);

                struct share* newShare = new struct share();

                newShare->name = name;
                newShare->kuerzel = kuerzel;
                newShare->wkn = wkn;

                for (int day = 0; day < 30; day++) {
                    std::string date = line.substr(0, line.find("|"));
                    line.erase(0, line.find("|") + 1);

                    float open = std::stof(line.substr(0, line.find("|")));
                    line.erase(0, line.find("|") + 1);

                    float high = std::stof(line.substr(0, line.find("|")));
                    line.erase(0, line.find("|") + 1);

                    float low = std::stof(line.substr(0, line.find("|")));
                    line.erase(0, line.find("|") + 1);

                    float close = std::stof(line.substr(0, line.find("|")));
                    line.erase(0, line.find("|") + 1);

                    float adjClose = std::stof(line.substr(0, line.find("|")));
                    line.erase(0, line.find("|") + 1);

                    long int volume = std::stoi(line.substr(0, line.find(";")));
                    line.erase(0, line.find(";") + 1);

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

                    newShare->kursdaten[day] = newDay;
                }

                kuerzelHashTable[i] = newShare;
            }

            //Increment index
            i++;
        }

        //Print success message
        std::cout << "File \"kuerzelHashTable.txt\" successfully loaded!" << std::endl;
        stockFile.close();
    }
    else {
        std::cout << "Unable to open file \"kuerzelHashTable.txt\"!" << std::endl;
    }

    //Exact same thing as above, just for other hashTable
    //For comments on how this works see above
    std::ifstream nameFile("nameHashTable.txt");

    if (nameFile.is_open())
    {
        std::string line;

        int i = 0;
        while (getline(nameFile, line)) {

            if (line == "NULL")
                nameHashTable[i] = NULL;
            else {
                std::string name = line.substr(0, line.find("|"));
                line.erase(0, line.find("|") + 1);

                std::string kuerzel = line.substr(0, line.find("|"));

                struct mapNameToKuerzel* newStruct = new struct mapNameToKuerzel();

                newStruct->name = name;
                newStruct->kuerzel = kuerzel;
           
                nameHashTable[i] = newStruct;
            }

            i++;
        }

        std::cout << "File \"nameHashTable.txt\" successfully loaded!" << std::endl;
        nameFile.close();
    }
    else {
        std::cout << "Unable to open file \"nameHashTable.txt\"!" << std::endl;
    }

    std::cout << std::endl;
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
                plot(search(kuerzelHashTable, nameHashTable));
                break;
            case 5:
                saveData(kuerzelHashTable, nameHashTable);
                break;
            case 6:
                loadData(kuerzelHashTable, nameHashTable);
                break;
            case 7:
                loop = false;
                break;
        }
    }

    clearHeap(kuerzelHashTable, nameHashTable);
    return 0;
}
