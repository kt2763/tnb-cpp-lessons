#include <iostream>

using namespace std;

int main()
{
    int n;
    cin >> n;
    if (cin.fail())
    {
        cout << "入力エラー！\n";
        cin.clear();
        cin.ignore(1000, '\n');
        // numeric_limits<streamsize>::max()でもOK
    }
}
