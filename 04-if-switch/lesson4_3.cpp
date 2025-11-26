#include <iostream>
using namespace std;

int main()
{
    int grade;
    cout << "成績(1〜3)を入力: ";
    cin >> grade;

    switch (grade)
    {
    case 1:
        cout << "良\n";
        break;
    case 2:
        cout << "可\n";
        break;
    case 3:
        cout << "不可\n";
        break;
    default:
        cout << "範囲外\n";
    }
}
