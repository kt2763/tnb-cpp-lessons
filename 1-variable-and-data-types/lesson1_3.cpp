#include <iostream>
#include <limits> // numeric_limits

int main()
{
    std::cout << "int  : " << sizeof(int) << "バイト, 最大値は"
              << std::numeric_limits<int>::max() << "\n";
    std::cout << "double: " << sizeof(double) << "バイト, 最大値は"
              << std::numeric_limits<double>::max() << "\n";
    return 0;
}
