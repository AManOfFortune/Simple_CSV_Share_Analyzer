#include <iostream>

struct kursdaten {
    std::string date;
    float open;
    float high;
    float low;
    float close;
    float volume;
    float adjClose;
};

struct aktie {
    std::string name;
    std::string wkn;
    std::string kuerzel;

    struct kursdaten kursdaten[30];
};

int main()
{
    int option = 0;
    std::cout << "(1) - ADD | (2) - DEL | (3) - IMPORT | (4) - SEARCH | (5) - PLOT | (6) - SAVE | (7) - LOAD | (8) - QUIT" << std::endl;

    do {
        std::cout << "Choose: ";
        std::cin >> option;
    } while (option <= 0 || option > 8);

    switch (option)
    {
    case 1:
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
        break;
    }
}
