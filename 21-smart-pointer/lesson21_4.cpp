#include <iostream>
#include <memory>

class Enemy; // 前方宣言

class Player
{
public:
    std::string name;
    std::shared_ptr<Enemy> target; // ❌ shared_ptr だと循環参照になる

    Player(std::string n) : name(n)
    {
        std::cout << name << " 登場" << std::endl;
    }

    ~Player()
    {
        std::cout << name << " 退場" << std::endl;
    }
};

class Enemy
{
public:
    std::string name;
    std::weak_ptr<Player> aggro; // ✅ weak_ptr で循環を断ち切る

    Enemy(std::string n) : name(n)
    {
        std::cout << name << " 出現" << std::endl;
    }

    ~Enemy()
    {
        std::cout << name << " 消滅" << std::endl;
    }

    void attackTarget()
    {
        // lock() で一時的に shared_ptr に変換してアクセス
        if (auto player = aggro.lock())
        {
            std::cout << name << " が " << player->name << " を攻撃！" << std::endl;
        }
        else
        {
            std::cout << name << " のターゲットはもういない" << std::endl;
        }
    }
};

int main()
{
    auto player = std::make_shared<Player>("勇者");
    auto enemy = std::make_shared<Enemy>("ゴブリン");

    player->target = enemy;       // Player が Enemy を所有
    enemy->aggro = player;      // Enemy は weak_ptr で Player を参照（所有しない）

    enemy->attackTarget();        // ターゲットが生きているのでアクセスできる

    player.reset();               // player を手放す → 参照カウントが0になり解放される

    enemy->attackTarget();        // player はもういないので expired 扱いになる

    return 0;
}
