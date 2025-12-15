#include <iostream>

// 敵キャラクターを表すクラス
class Enemy
{
public:
    int hp;
    int attackPower;

    Enemy(int h, int ap) : hp(h), attackPower(ap)
    {
        std::cout << "敵が出現! HP:" << hp << std::endl;
    }

    ~Enemy()
    {
        std::cout << "敵を倒した!" << std::endl;
    }
};

int main()
{
    // ヒープメモリに敵キャラを1体生成
    Enemy* enemy = new Enemy(50, 10);

    std::cout << "敵の攻撃力: " << enemy->attackPower << std::endl;

    // 敵を倒したのでメモリから削除
    delete enemy;

    // 安全のためnullptrを代入
    enemy = nullptr;

    return 0;
}
