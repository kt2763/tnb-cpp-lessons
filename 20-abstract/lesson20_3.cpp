#include <iostream>
#include <string>
using namespace std;

// インターフェース1:ダメージを受ける
class IDamageable
{
public:
    virtual void takeDamage(int damage) = 0;
    virtual ~IDamageable() {}
};

// インターフェース2:移動できる
class IMovable
{
public:
    virtual void move(float x, float y) = 0;
    virtual ~IMovable() {}
};

// プレイヤーは2つ全てを実装
class Player : public IDamageable, public IMovable
{
private:
    string name;
    int hp;
    float x, y;

public:
    Player(string n) : name(n), hp(100), x(0), y(0) {}

    void takeDamage(int damage) override
    {
        hp -= damage;
        cout << name << "は" << damage << "ダメージ! HP:" << hp << endl;
    }

    void move(float dx, float dy) override
    {
        x += dx;
        y += dy;
        cout << name << "が移動: (" << x << ", " << y << ")" << endl;
    }

};

// 敵は移動と攻撃、ダメージを受けるを実装
class Enemy : public IDamageable, public IMovable
{};

// 建物はダメージを受けるだけ(移動や攻撃はできない)
class Building : public IDamageable
{};

