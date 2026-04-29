#include <iostream>
#include <functional>
#include <string>

struct Player
{
    std::string name;
    int         hp;
    int         attack;

    // ダメージ計算式を外部から注入できる
    std::function<int(int, int)> damage_formula;

    void attack_enemy(int enemy_defense) const
    {
        int dmg = damage_formula(attack, enemy_defense);
        std::cout << name << " のダメージ: " << dmg << std::endl;
    }
};

int main()
{
    // 通常ダメージ式
    auto normal_formula = [](int atk, int def) -> int
        {
            return std::max(atk - def, 1);
        };

    // クリティカルヒット式（2倍ダメージ、防御無視）
    auto critical_formula = [](int atk, int /*def*/) -> int
        {
            return atk * 2;
        };

    Player hero = { "勇者", 100, 40, normal_formula };
    hero.attack_enemy(15); // ダメージ: 25

    std::cout << "--- クリティカル発動！ ---" << std::endl;
    hero.damage_formula = critical_formula;
    hero.attack_enemy(15); // ダメージ: 80

    return 0;
}
