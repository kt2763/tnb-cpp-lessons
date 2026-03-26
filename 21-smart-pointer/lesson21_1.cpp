#include <iostream>
#include <memory>
#include <vector>

class Enemy
{
public:
    std::string name;
    int hp;

    Enemy(std::string n, int h) : name(n), hp(h)
    {
        std::cout << name << " が出現！ HP:" << hp << std::endl;
    }

    ~Enemy()
    {
        std::cout << name << " を倒した！" << std::endl;
    }

    void attack()
    {
        std::cout << name << " の攻撃！" << std::endl;
    }
};

int main()
{
    // --- 基本：make_unique で生成 ---
    {
        auto boss = std::make_unique<Enemy>("ドラゴン", 1000);
        boss->attack();
        // スコープを抜けると自動解放。delete は不要！
    }

    std::cout << "--- 所有権の移動 ---" << std::endl;

    // --- std::move で所有権を移す ---
    {
        auto enemy1 = std::make_unique<Enemy>("ゴブリン", 50);

        // move すると enemy1 は nullptr になり、enemy2 がオーナーになる
        auto enemy2 = std::move(enemy1);

        if (enemy1 == nullptr)
        {
            std::cout << "enemy1 はもう何も持っていない" << std::endl;
        }

        enemy2->attack(); // enemy2 が所有しているので使える
    }

    std::cout << "--- vector に格納 ---" << std::endl;

    // --- コンテナに入れる場合も move ---
    {
        std::vector<std::unique_ptr<Enemy>> enemies;
        enemies.push_back(std::make_unique<Enemy>("スライム", 30));
        enemies.push_back(std::make_unique<Enemy>("オーク", 80));

        for (auto& e : enemies)
        {
            e->attack();
        }
        // vector が破棄されると全敵が自動解放
    }

    return 0;
}
