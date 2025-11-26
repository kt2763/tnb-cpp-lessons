#include <iostream>
using namespace std;

class Player
{
private:
    int hp; // フィールド（メンバ変数）
    int maxHp;

public:
    // コンストラクタ
    Player(int maxHpValue)
    {
        maxHp = maxHpValue;
        hp = maxHp;
    }

    // メソッド（メンバ関数）
    void Damage(int amount)
    {
        hp -= amount;
        if (hp < 0)
            hp = 0;
    }

    void Heal(int amount)
    {
        hp += amount;
        if (hp > maxHp)
            hp = maxHp;
    }

    void ShowStatus()
    {
        cout << "HP: " << hp << " / " << maxHp << endl;
    }
};
