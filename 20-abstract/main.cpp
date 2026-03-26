// Factoryパターン
// デザインパターンの1つ
// オブジェクトの生成ロジックを別クラスに分離する
// 生成ロジックが一箇所にまとまる
// 生成方法を変更しやすい
// クライアントコードが具体的なクラスを知らなくても良い

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
    Enemy(string n, int h, int a) : name{ n }, hp{ h }, attack{ a }
    {}

    virtual void attackPlayer() = 0;
    void showInfo()
    {
        cout << name << "HP: " << hp << " 攻撃: " << attack << endl;
    }

    virtual ~Enemy()
    {}
};

class Slime : public Enemy
{
public:
    Slime() : Enemy{ "スライム", 50, 10 }
    {}

    void attackPlayer() override
    {
        cout << name << "が体当たり！" << endl;
    }
};

class Goblin : public Enemy
{
public:
    Goblin() : Enemy{ "ゴブリン", 80, 15 }
    {}

    void attackPlayer() override
    {
        cout << name << "が武器で攻撃！" << endl;
    }
};

// 列挙型
enum class EnemyType
{
    SLIME,
    GOBLIN
};

class EnemyFactory
{
public:
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

    Enemy* enemy1 = EnemyFactory::createEnemy(EnemyType::SLIME);
    Enemy* enemy2 = EnemyFactory::createEnemy(EnemyType::GOBLIN);

    enemy1->showInfo();
    enemy1->attackPlayer();

    enemy2->showInfo();
    enemy2->attackPlayer();

    delete enemy1;
    delete enemy2;
}
