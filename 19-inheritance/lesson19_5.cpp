#include <iostream>
#include <string>
using namespace std;

class Enemy
{
protected:
    string name;
    int attack;

public:
    Enemy(string n, int a) : name(n), attack(a) {}

    // virtualをつける
    virtual void attackPlayer()
    {
        cout << name << "の攻撃! " << attack << "ダメージ!" << endl;
    }

    // 仮想デストラクタ(重要!)
    virtual ~Enemy()
    {
        cout << name << "が倒れた..." << endl;
    }
};

class Slime : public Enemy
{
public:
    Slime() : Enemy("スライム", 10) {}

    // オーバーライド
    void attackPlayer() override
    {
        cout << name << "が体当たり! " << attack << "ダメージ!" << endl;
    }
};

class Goblin : public Enemy
{
public:
    Goblin() : Enemy("ゴブリン", 15) {}

    void attackPlayer() override
    {
        cout << name << "が武器で斬りつける! " << attack << "ダメージ!" << endl;
    }
};

int main()
{
    // 基底クラスのポインタで派生クラスを扱う
    Enemy* enemy1 = new Slime();
    Enemy* enemy2 = new Goblin();

    enemy1->attackPlayer(); // スライム版が呼ばれる!
    enemy2->attackPlayer(); // ゴブリン版が呼ばれる!

    delete enemy1;
    delete enemy2;
}
