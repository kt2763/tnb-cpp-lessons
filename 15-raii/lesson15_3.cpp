#include <iostream>

// シンプルな敵の構造体
struct Enemy
{
    int hp;
    int x, y;  // 座標
};

int main()
{
    int enemyCount;
    std::cout << "このステージの敵の数を入力: ";
    std::cin >> enemyCount;

    // 実行時に決まった数だけ敵を生成
    Enemy* enemies = new Enemy[enemyCount];

    // 各敵を初期化
    for (int i = 0; i < enemyCount; i++)
    {
        enemies[i].hp = 30;
        enemies[i].x = i * 100;  // 横に並べる
        enemies[i].y = 0;

        std::cout << "敵" << i << ": HP=" << enemies[i].hp
            << " 座標(" << enemies[i].x << "," << enemies[i].y << ")"
            << std::endl;
    }

    // ステージクリア!敵を全部削除
    delete[] enemies;  // 配列なのでdelete[]を使う!
    enemies = nullptr;

    std::cout << "ステージクリア!" << std::endl;

    return 0;
}
