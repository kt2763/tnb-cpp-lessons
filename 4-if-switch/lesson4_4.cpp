#include <iostream>
using namespace std;

int main()
{
    int menu;
    cout << "1: 攻撃, 2: 防御, 3: 回復 → ";
    cin >> menu;

    switch (menu)
    {
    case 1:
        cout << "攻撃！\n";
        break;
    case 2:
        cout << "防御！\n";
        break;
    case 3:
        cout << "回復！\n";
        break;
    default:
        cout << "無効な入力\n";
        break;
    }
}
