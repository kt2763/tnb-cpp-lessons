#include <iostream>
#include <string>
using namespace std;

// 敵の基底クラス
class Enemy
{
protected:
    string name;
    int hp;
    int attack;

public:
    Enemy(string n, int h, int a) : name(n), hp(h), attack(a) {}

    virtual void attackPlayer() = 0;

    void showInfo()
    {
        cout << name << " HP:" << hp << " 攻撃:" << attack << endl;
    }

    virtual ~Enemy() {}
};

// 具体的な敵クラス
class Slime : public Enemy
{
public:
    Slime() : Enemy("スライム", 50, 10) {}

    void attackPlayer() override
    {
        cout << name << "が体当たり!" << endl;
    }
};

class Goblin : public Enemy
{
public:
    Goblin() : Enemy("ゴブリン", 80, 15) {}

    void attackPlayer() override
    {
        cout << name << "が武器で攻撃!" << endl;
    }
};

// 敵の種類を表す列挙型
enum class EnemyType
{
    SLIME,
    GOBLIN
};

// Factoryクラス(敵の生成を専門に行う)
class EnemyFactory
{
public:
    // 敵を生成する静的メソッド
    static Enemy* createEnemy(EnemyType type)
    {
        switch (type)
        {
        case EnemyType::SLIME:
            return new Slime();
        case EnemyType::GOBLIN:
            return new Goblin();
        default:
            return nullptr;
        }
    }
};

int main()
{
    cout << "=== 敵の生成 ===" << endl;

    // Factoryを使って敵を生成
    Enemy* enemy1 = EnemyFactory::createEnemy(EnemyType::SLIME);
    Enemy* enemy2 = EnemyFactory::createEnemy(EnemyType::GOBLIN);

    enemy1->showInfo();
    enemy1->attackPlayer();

    enemy2->showInfo();
    enemy2->attackPlayer();

    // メモリ解放
    delete enemy1;
    delete enemy2;
}
