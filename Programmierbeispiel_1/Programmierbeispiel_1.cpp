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
