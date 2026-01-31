#include <iostream>
using namespace std;

class Enemy
{
protected:
    string name;
    int attack;

public:
    Enemy(string n, int a) : name(n), attack(a) {}

    // virtualなし
    void attackPlayer()
    {
        cout << name << "の攻撃! " << attack << "ダメージ!" << endl;
    }
};

class Slime : public Enemy
{
public:
    Slime() : Enemy("スライム", 10) {}

    void attackPlayer()
    {
        cout << name << "が体当たり! " << attack << "ダメージ!" << endl;
    }
};

int main()
{
    Slime slime;
    Enemy* enemy = &slime; // 基底クラスのポインタで派生クラスを指す

    enemy->attackPlayer(); // どちらが呼ばれる?
    // 結果:「スライムの攻撃! 10ダメージ!」(基底クラスの関数が呼ばれる)
}
