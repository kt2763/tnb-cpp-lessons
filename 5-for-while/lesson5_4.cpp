#include <iostream>
using namespace std;

int main()
{
    for (int y = 0; y < 3; y++)
    {
        for (int x = 0; x < 3; x++)
        {
            cout << "(" << x << "," << y << ") ";
        }
        cout << "\n";
    }

    return 0;
}
