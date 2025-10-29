#include <iostream>
using namespace std;

void Print(int x)
{
    cout << "int型: " << x << "\n";
}

void Print(double x)
{
    cout << "double型: " << x << "\n";
}

void Print(string x)
{
    cout << "string型: " << x << "\n";
}

int main()
{
    Print(42);
    Print(3.14);
    Print("Hello"s); // 文字列リテラルは ""s でstring化
}
