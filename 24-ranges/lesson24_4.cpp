#include <iostream>
#include <vector>
#include <ranges>
#include <string>

struct Enemy
{
    std::string name;
    int         hp;
};

int main()
{
    std::vector<Enemy> enemies = {
        { "ゴブリン", 50 },
        { "スライム", 20 },
        { "ドラゴン", 500 },
    };

    // ビューは元のコンテナを参照するだけ
    auto weak_view = enemies | std::views::filter(
        [](const Enemy& e) { return e.hp < 100; });

    // ① 元のコンテナを変更すると反映される（参照なので）
    enemies[0].hp = 200; // ゴブリンを強化

    std::cout << "=== フィルタ後（変更後）===" << std::endl;
    for (const auto& e : weak_view)
    {
        std::cout << e.name << " HP:" << e.hp << std::endl;
        // スライムだけ表示される（ゴブリンは200になったので除外）
    }

    // ② ビューを std::vector に変換する（C++20での手動コピー）
    std::vector<Enemy> result;
    for (const auto& e : weak_view)
    {
        result.push_back(e);
    }
    std::cout << "\nresult のサイズ: " << result.size() << std::endl;

    return 0;
}
