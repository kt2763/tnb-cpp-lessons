#include <iostream>
using namespace std;

void Greet(string name = "Guest")
{
    cout << "Hello, " << name << "!\n";
}

int main()
{
    Greet("Tanabe"); // Hello, Tanabe!
    Greet();         // Hello, Guest!
}
