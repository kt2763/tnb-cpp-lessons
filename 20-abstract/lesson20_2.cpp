#include <iostream>
using namespace std;

// インターフェース(全て純粋仮想関数)
class IDamageable
{
public:
    virtual void takeDamage(int damage) = 0;
    virtual bool isAlive() const = 0;
    virtual ~IDamageable() {}
};

// プレイヤークラスがインターフェースを実装
class Player : public IDamageable
{
private:
    int hp;

public:
    Player() : hp(100) {}

    void takeDamage(int damage) override
    {
        hp -= damage;
        cout << "プレイヤーが" << damage << "ダメージを受けた! HP:" << hp << endl;
    }

    bool isAlive() const override
    {
        return hp > 0;
    }
};

// 敵クラスもインターフェースを実装
class Enemy : public IDamageable
{
private:
    int hp;

public:
    Enemy() : hp(50) {}

    void takeDamage(int damage) override
    {
        hp -= damage;
        cout << "敵が" << damage << "ダメージを受けた! HP:" << hp << endl;
    }

    bool isAlive() const override
    {
        return hp > 0;
    }
};

// 建物もインターフェースを実装
class Building : public IDamageable
{
private:
    int durability; // 耐久度

public:
    Building() : durability(200) {}

    void takeDamage(int damage) override
    {
        durability -= damage;
        cout << "建物が" << damage << "ダメージを受けた! 耐久度:" << durability << endl;
    }

    bool isAlive() const override
    {
        return durability > 0;
    }
};

int main()
{
    // 全て同じインターフェースで扱える
    IDamageable* targets[3];
    targets[0] = new Player();
    targets[1] = new Enemy();
    targets[2] = new Building();

    // 全てに同じように攻撃できる
    for (int i = 0; i < 3; i++)
    {
        targets[i]->takeDamage(30);
        if (!targets[i]->isAlive())
        {
            cout << "破壊された!" << endl;
        }
    }

    // メモリ解放
    for (int i = 0; i < 3; i++)
    {
        delete targets[i];
    }
}
