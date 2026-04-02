#include <iostream>

#define XSTR(x) STR(x)
#define STR(x) #x

int main() {
#ifdef VERSION
    std::cout << "VERSION is defined: " << XSTR(VERSION) << std::endl;
#else
    std::cout << "VERSION is NOT defined" << std::endl;
#endif
    return 0;
}
