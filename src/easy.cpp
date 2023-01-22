#include <iostream>
#include <fstream>
#include <unistd.h>

int main() {
    std::string color;
    std::string pole[8];
    std::cin >> color;
    for(int i = 0; i < 8; ++ i) {
        std::cin >> pole[i];
    }
    if(color == "white") {
        std::cout << "h2e3" << std::endl;
    } else {
        
    }
    return 0;
}