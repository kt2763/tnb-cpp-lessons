#include <iostream>
#include <string>
using namespace std;

// 基底クラス(親クラス)
class Enemy
{
public:
    string name;
    int hp;

    Enemy(string n, int h) : name(n), hp(h)
    {
        cout << name << "が出現! HP:" << hp << endl;
    }

    void takeDamage(int damage)
    {
        hp -= damage;
        cout << name << "に" << damage << "ダメージ! 残りHP:" << hp << endl;
    }
};

// 派生クラス(子クラス) - Enemyを継承
class Slime : public Enemy
{
public:
    // 基底クラスのコンストラクタを呼び出す
    Slime() : Enemy("スライム", 50)
    {
        cout << "スライムは分裂できる!" << endl;
    }
};

int main()
{
    Slime slime;
    slime.takeDamage(20); // 基底クラスの関数を使える
}
