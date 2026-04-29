#include <iostream>
#include <vector>
#include <algorithm>
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
        { "ドラゴン", 500, 80 },
        { "ウルフ",    70, 15 },
    };

    // --- std::sort：HP昇順でソート ---
    std::sort(enemies.begin(), enemies.end(),
        [](const Enemy& a, const Enemy& b)
        {
            return a.hp < b.hp;
        });

    std::cout << "=== HP昇順 ===" << std::endl;
    for (const auto& e : enemies)
    {
        std::cout << e.name << " HP:" << e.hp << std::endl;
    }

    // --- std::find_if：HPが100以上の最初の敵を探す ---
    // itは「見つかった要素の位置を指すポインタっぽいもの」
    auto it = std::find_if(enemies.begin(), enemies.end(),
        [](const Enemy& e) { return e.hp >= 100; });

    // itに条件に合う要素が見つからない場合、enemies.end()を返す
    if (it != enemies.end())
    {
        std::cout << "\n最初の強敵: " << it->name << std::endl;
    }

    // --- std::count_if：HPが60以下の弱い敵を数える ---
    int weak_count = std::count_if(enemies.begin(), enemies.end(),
        [](const Enemy& e) { return e.hp <= 60; });

    std::cout << "弱い敵の数: " << weak_count << std::endl;

    // --- std::for_each：全敵にバフ（攻撃力+5）---
    std::for_each(enemies.begin(), enemies.end(),
        [](Enemy& e) { e.attack += 5; });

    std::cout << "\n=== バフ後の攻撃力 ===" << std::endl;
    for (const auto& e : enemies)
    {
        std::cout << e.name << " ATK:" << e.attack << std::endl;
    }

    return 0;
}
