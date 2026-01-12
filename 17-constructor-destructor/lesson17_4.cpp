#include <iostream>
#include <string>
using namespace std;

class Weapon
{
public:
    string name;
    int attack;

    // 初期化リストを使った書き方
    Weapon(string n, int a) : name(n), attack(a)
    {
        cout << name << "を装備しました。攻撃力: " << attack << endl;
    }
};

int main()
{
    Weapon sword("剣", 30);
}
