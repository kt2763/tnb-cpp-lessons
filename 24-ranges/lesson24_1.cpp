#include <iostream>
#include <vector>
#include <map>
#include <string>

struct Enemy
{
    std::string name;
    int         hp;
    int         attack;
};

int main()
{
    std::vector<Enemy> enemies = {
        { "ゴブリン",  50, 10 },
        { "オーク",   120, 25 },
        { "スライム",  20,  5 },
    };

    // ❌ 値コピー：Enemyが大きいと毎回コピーされて遅い
    for (auto e : enemies)
    {
        std::cout << e.name << std::endl;
    }

    // ✅ const参照：コピーなし、変更不可
    for (const auto& e : enemies)
    {
        std::cout << e.name << " HP:" << e.hp << std::endl;
    }

    // ✅ 参照：変更あり
    for (auto& e : enemies)
    {
        e.hp += 10; // HPを全員回復
    }

    // structured bindings（C++17）: mapのキーと値を分解
    std::map<std::string, int> score_board = {
        { "勇者",   1500 },
        { "魔法使い", 1200 },
        { "盗賊",    900 },
    };

    for (const auto& [name, score] : score_board)
    {
        std::cout << name << ": " << score << "点" << std::endl;
    }

    return 0;
}
