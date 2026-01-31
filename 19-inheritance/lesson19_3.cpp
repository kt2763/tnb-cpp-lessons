#include <iostream>
#include <string>
using namespace std;

class Enemy
{
protected:
    string name;
    int hp;
    int attack;

public:
    Enemy(string n, int h, int a) : name(n), hp(h), attack(a) {}

    // 通常の攻撃(派生クラスで上書き可能にする)
    void attackPlayer()
    {
        cout << name << "の攻撃! " << attack << "ダメージ!" << endl;
    }
};

class Slime : public Enemy
{
public:
    Slime() : Enemy("スライム", 50, 10) {}

    // attackPlayerをオーバーライド(上書き)
    void attackPlayer()
    {
        cout << name << "が体当たり! " << attack << "ダメージ!" << endl;
    }
};

class Goblin : public Enemy
{
public:
    Goblin() : Enemy("ゴブリン", 80, 15) {}

    // attackPlayerをオーバーライド
    void attackPlayer()
    {
        cout << name << "が武器で斬りつける! " << attack << "ダメージ!" << endl;
    }
};

int main()
{
    Slime slime;
    slime.attackPlayer();  // スライム版が呼ばれる

    Goblin goblin;
    goblin.attackPlayer(); // ゴブリン版が呼ばれる
}
