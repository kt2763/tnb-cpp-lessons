#include <iostream>
#include <string>
using namespace std;

// 抽象クラス(インスタンス化できない)
class Enemy
{
protected:
    string name;
    int hp;

public:
    Enemy(string n, int h) : name(n), hp(h) {}

    // 純粋仮想関数(= 0で定義)
    virtual void attackPlayer() = 0;

    // 純粋仮想関数
    virtual void specialAbility() = 0;

    void takeDamage(int damage)
    {
        hp -= damage;
        cout << name << "に" << damage << "ダメージ!" << endl;
    }

    virtual ~Enemy() {}
};

class Slime : public Enemy
{
public:
    Slime() : Enemy("スライム", 50) {}

    // 純粋仮想関数を実装しないとエラー
    void attackPlayer() override
    {
        cout << name << "が体当たり!" << endl;
    }

    void specialAbility() override
    {
        cout << name << "が分裂した!" << endl;
    }
};

class Dragon : public Enemy
{
public:
    Dragon() : Enemy("ドラゴン", 300) {}

    void attackPlayer() override
    {
        cout << name << "が爪で攻撃!" << endl;
    }

    void specialAbility() override
    {
        cout << name << "が火を吐いた!" << endl;
    }
};

int main()
{
    // Enemy enemy; // エラー!抽象クラスはインスタンス化できない

    Enemy* slime = new Slime();
    Enemy* dragon = new Dragon();

    slime->attackPlayer();
    slime->specialAbility();

    dragon->attackPlayer();
    dragon->specialAbility();

    delete slime;
    delete dragon;
}
