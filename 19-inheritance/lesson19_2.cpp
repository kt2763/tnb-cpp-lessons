#include <iostream>
#include <string>
using namespace std;

// 基底クラス:敵キャラクター全般
class Enemy
{
protected:
    string name;
    int hp;
    int attack;

public:
    Enemy(string n, int h, int a) : name(n), hp(h), attack(a)
    {
        cout << "【出現】" << name << " HP:" << hp << " 攻撃:" << attack << endl;
    }

    void takeDamage(int damage)
    {
        hp -= damage;
        cout << name << "に" << damage << "ダメージ! 残りHP:" << hp << endl;
    }

    void attackPlayer()
    {
        cout << name << "の攻撃! " << attack << "ダメージ!" << endl;
    }
};

// 派生クラス1:スライム
class Slime : public Enemy
{
public:
    Slime() : Enemy("スライム", 50, 10) {}

    // スライム独自の能力
    void split()
    {
        cout << name << "が分裂した!" << endl;
    }
};

// 派生クラス2:ゴブリン
class Goblin : public Enemy
{
public:
    Goblin() : Enemy("ゴブリン", 80, 15) {}

    // ゴブリン独自の能力
    void throwStone()
    {
        cout << name << "が石を投げた! " << (attack + 5) << "ダメージ!" << endl;
    }
};


int main()
{
    Slime slime;
    slime.attackPlayer();  // 基底クラスの関数
    slime.split();         // 派生クラス独自の関数

    cout << endl;

    Goblin goblin;
    goblin.attackPlayer();
    goblin.throwStone();

}
