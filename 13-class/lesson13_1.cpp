#include "lesson13_1.hpp"

int main()
{
    Player p(100); // 最大HP100のプレイヤー生成
    p.ShowStatus();

    p.Damage(30);
    p.ShowStatus();

    p.Heal(20);
    p.ShowStatus();
}
