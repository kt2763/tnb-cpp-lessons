#include <iostream>
#include <memory>  // スマートポインタを使うために必要

class Boss
{
public:
    std::string name;
    int hp;

    Boss(std::string n, int h) : name(n), hp(h)
    {
        std::cout << "ボス「" << name << "」が出現! HP:" << hp << std::endl;
    }

    ~Boss()
    {
        std::cout << "ボス「" << name << "」を倒した!" << std::endl;
    }

    void attack()
    {
        std::cout << name << "の攻撃!" << std::endl;
    }
};

int main()
{
    std::cout << "=== ボス戦開始 ===" << std::endl;

    {
        // unique_ptrを使った安全なメモリ管理
        // ボスは1体しかいないのでunique_ptrが最適
        std::unique_ptr<Boss> boss = std::make_unique<Boss>("ドラゴン", 1000);

        boss->attack();

        std::cout << "ボスのHP: " << boss->hp << std::endl;

        // deleteを書く必要なし!
        // このブロックを抜けると自動的に解放される
    }

    std::cout << "\n=== 敵の配列も管理できる ===" << std::endl;

    {
        // 敵の配列を管理
        int enemyCount = 5;
        std::unique_ptr<int[]> enemyHP = std::make_unique<int[]>(enemyCount);

        // 各敵のHPを設定
        for (int i = 0; i < enemyCount; i++)
        {
            enemyHP[i] = 30;
            std::cout << "敵" << i << ": HP=" << enemyHP[i] << std::endl;
        }

        // これも自動的に解放される
    }

    std::cout << "\n=== ステージクリア ===" << std::endl;

    return 0;
}
// ここで自動的にすべて解放される - 安全!
