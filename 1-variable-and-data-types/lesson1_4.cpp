#include <iostream>

int main()
{
    auto x = 10;        // 推論 → int
    auto y = 3.14;      // 推論 → double
    auto msg = "hello"; // 推論 → const char*

    std::cout << x << " " << y << " " << msg << "\n";
    return 0;
}
