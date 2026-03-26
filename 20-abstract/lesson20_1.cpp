#include <iostream>
#include <string>
using namespace std;

// 抽象クラス(インスタンス化できない)
class Character
{
protected:
    string name;
    int hp;

public:
    Character(string n, int h) : name(n), hp(h) {}

    // 純粋仮想関数(= 0で定義、実装は派生クラスで行う)
    virtual void attack() = 0;

    // 通常の関数も定義できる
    void takeDamage(int damage)
    {
        hp -= damage;
        cout << name << "は" << damage << "ダメージを受けた! HP:" << hp << endl;
    }

    virtual ~Character() {}
};

// 派生クラスで純粋仮想関数を実装
class Player : public Character
{
public:
    Player(string n) : Character(n, 100) {}

    // 純粋仮想関数を実装(これがないとエラー)
    void attack() override
    {
        cout << name << "が剣で攻撃!" << endl;
    }
};

int main()
{
    // Character character("テスト", 100); // エラー!抽象クラスはインスタンス化できない

    Player player("勇者");
    player.attack();        // 実装された関数が呼ばれる
    player.takeDamage(20);  // 基底クラスの関数も使える
}
