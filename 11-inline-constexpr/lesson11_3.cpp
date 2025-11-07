#include <iostream>
using namespace std;

constexpr int square(int x)
{
    return x * x;
}

int arr[square(3)]; // OK: square(3) はコンパイル時に評価される

int main()
{
    cout << sizeof(arr) / sizeof(int) << "\n"; // intは4バイト
    return 0;
}
