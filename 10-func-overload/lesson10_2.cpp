#include <iostream>
using namespace std;

void Print(int x)
{
    cout << "int\n";
}
void Print(double x)
{
    cout << "double\n";
}

int main()
{
    Print(10.0f); // float → int か double、この場合はdoubleになる
}
