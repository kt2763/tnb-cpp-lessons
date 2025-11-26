#include <iostream>
using namespace std;

int main()
{
    int score;
    cout << "点数を入力: ";
    cin >> score;

    if (score >= 80)
    {
        cout << "合格！\n";
    }
    else if (score >= 50)
    {
        cout << "追試です\n";
    }
    else
    {
        cout << "不合格...\n";
    }
}
