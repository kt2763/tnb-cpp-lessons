#include <iostream>
using namespace std;

int main()
{
    int age = 20;
    bool hasTicket = true;

    cout << boolalpha;
    cout << (age >= 18 && hasTicket) << "\n"; // true
    cout << (age < 18 || !hasTicket) << "\n"; // false
    cout << !(age >= 18) << "\n";             // false
}
