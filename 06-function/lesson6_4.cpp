#include <iostream>
using namespace std;

int square(int x) { return x * x; }
double square(double x) { return x * x; }

int main()
{
    cout << square(3) << "\n";   // int版
    cout << square(2.5) << "\n"; // double版
}
