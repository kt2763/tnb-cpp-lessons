#include <iostream>
#include <string>
using namespace std;

class Enemy
{
public:
    string name;
    int hp;

    // 引数付きコンストラクタ
    Enemy(string n, int h)
    {
        name = n;
        hp = h;
        cout << name << "が生成されました! HP: " << hp << endl;
    }
};

int main()
{
    Enemy slime("スライム", 50);
    Enemy dragon("ドラゴン", 300);
}
