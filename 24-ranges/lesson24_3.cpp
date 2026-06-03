#include <iostream>
#include <vector>
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
        { "ゾンビ",    40,  8 },
    };

    // --- filter：HP60以下の弱い敵だけ抽出 ---
    std::cout << "=== 弱い敵（HP≦60）===" << std::endl;
    for (const auto& e : enemies | std::views::filter(
        [](const Enemy& e) { return e.hp <= 60; }))
    {
        std::cout << e.name << " HP:" << e.hp << std::endl;
    }

    // --- transform：名前だけを取り出す ---
    std::cout << "\n=== 敵の名前一覧 ===" << std::endl;
    for (const auto& name : enemies | std::views::transform(
        [](const Enemy& e) { return e.name; }))
    {
        std::cout << name << std::endl;
    }

    // --- パイプラインをチェーン：HPが100以上の敵を攻撃力降順で先頭3体 ---
    // ☆要確認：views::sort はC++23以降。C++20では ranges::sort と別途コレクションに入れる必要あり
    // ここでは filter + take の組み合わせを示す
    std::cout << "\n=== 強い敵（HP≧100）の上位3体 ===" << std::endl;
    for (const auto& e : enemies
        | std::views::filter([](const Enemy& e) { return e.hp >= 100; })
        | std::views::take(3))
    {
        std::cout << e.name << " HP:" << e.hp << std::endl;
    }

    return 0;
}
