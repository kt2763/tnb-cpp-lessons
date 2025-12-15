#include <iostream>

class Enemy
{
public:
    int hp;

    Enemy(int h) : hp(h)
    {
        std::cout << "敵が出現! HP:" << hp << std::endl;
    }

    ~Enemy()
    {
        std::cout << "敵を倒した!" << std::endl;
    }
};

// ❌ 悪い例:メモリリークが発生
void spawnEnemyBad()
{
    Enemy* enemy = new Enemy(50);

    // 戦闘処理...
    std::cout << "戦闘中..." << std::endl;

    // deleteを忘れている!
    // この関数を呼ぶたびに敵がメモリに残り続ける
}

// ✅ 良い例:きちんと解放する
void spawnEnemyGood()
{
    Enemy* enemy = new Enemy(50);

    // 戦闘処理...
    std::cout << "戦闘中..." << std::endl;

    // 敵を倒したのでメモリから削除
    delete enemy;
}

int main()
{
    std::cout << "=== 悪い例:10回戦闘 ===" << std::endl;
    for (int i = 0; i < 10; i++)
    {
        spawnEnemyBad();  // 敵が10体分メモリに残る!
    }

    std::cout << "\n=== 良い例:10回戦闘 ===" << std::endl;
    for (int i = 0; i < 10; i++)
    {
        spawnEnemyGood();  // ちゃんと削除される
    }

    return 0;
}
