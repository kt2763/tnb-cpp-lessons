#include <iostream>
#include <vector>
#include <algorithm>
#include <ranges>
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

    // --- HP昇順ソート（rangesではコンテナをそのまま渡せる）---
    std::ranges::sort(enemies, {}, &Enemy::hp); // プロジェクション使用
    // ※ {} は比較子（デフォルト昇順 less）、&Enemy::hp はソートキー
    // 高い順（降順）にするならstd::ranges::greater() ）

    std::cout << "=== HP昇順 ===" << std::endl;
    for (const auto& e : enemies)
    {
        std::cout << e.name << " HP:" << e.hp << std::endl;
    }

    // --- find_if：HPが100以上の最初の敵 ---
    auto it = std::ranges::find_if(enemies,
        [](const Enemy& e) { return e.hp >= 100; });

    if (it != enemies.end())
    {
        std::cout << "\n最初の強敵: " << it->name << std::endl;
    }

    // --- count_if：HPが60以下を数える ---
    int weak_count = std::ranges::count_if(enemies,
        [](const Enemy& e) { return e.hp <= 60; });

    std::cout << "弱い敵の数: " << weak_count << std::endl;

    // --- for_each：全敵の名前を表示 ---
    std::cout << "\n=== 敵一覧 ===" << std::endl;
    std::ranges::for_each(enemies,
        [](const Enemy& e)
        {
            std::cout << "- " << e.name << std::endl;
        });

    return 0;
}
