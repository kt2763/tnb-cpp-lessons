#include <iostream>

int main()
{
    int player_hp = 100;
    int damage = 30;

    // --- 値キャプチャ [=]：コピーが取り込まれる ---
    auto show_hp = [=]()
        {
            std::cout << "HP: " << player_hp << " (ダメージ: " << damage << ")" << std::endl;
        };
    show_hp(); // HP: 100 (ダメージ: 30)

    player_hp = 70; // 元の変数を変えても...
    show_hp();      // HP: 100 のまま（コピーなので影響しない）

    std::cout << "--- 参照キャプチャ ---" << std::endl;

    // --- 参照キャプチャ [&]：参照として取り込む ---
    int enemy_hp = 200;

    auto take_damage = [&](int dmg)
        {
            enemy_hp -= dmg; // 元の変数を直接変更できる
            std::cout << "敵HP: " << enemy_hp << std::endl;
        };

    take_damage(50); // 敵HP: 150
    take_damage(80); // 敵HP: 70

    std::cout << "--- 混合キャプチャ ---" << std::endl;

    // --- 混合：デフォルト値キャプチャ、特定変数だけ参照 ---
    int base_attack = 20;
    int kill_count = 0;

    auto attack = [=, &kill_count](int enemy_remaining_hp)
        {
            int result = enemy_remaining_hp - base_attack;
            if (result <= 0)
            {
                ++kill_count; // kill_count だけ参照で変更できる
                std::cout << "撃破！通算: " << kill_count << "体" << std::endl;
            }
            return std::max(result, 0);
        };

    attack(15); // 撃破！通算: 1体
    attack(50); // (生き残る)
    attack(10); // 撃破！通算: 2体

    return 0;
}
