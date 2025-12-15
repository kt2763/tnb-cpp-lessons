#include <iostream>

// RAII を使った「ステージ管理クラス」
class Stage
{
private:
    int* enemyData;      // 敵のデータ
    int enemyCount;

public:
    // ステージ開始時に自動的にデータを読み込む
    Stage(int count) : enemyCount(count)
    {
        enemyData = new int[enemyCount];
        std::cout << "ステージ開始! 敵を" << enemyCount << "体読み込みました" << std::endl;

        // 敵のデータを初期化
        for (int i = 0; i < enemyCount; i++)
        {
            enemyData[i] = 30;  // HP30の敵
        }
    }

    // ステージ終了時に自動的にデータを解放
    ~Stage()
    {
        delete[] enemyData;
        std::cout << "ステージ終了! メモリを解放しました" << std::endl;
    }

    void showEnemies()
    {
        for (int i = 0; i < enemyCount; i++)
        {
            std::cout << "敵" << i << ": HP=" << enemyData[i] << std::endl;
        }
    }
};

int main()
{
    std::cout << "=== ゲーム開始 ===" << std::endl;

    {
        // ステージ1を作る(この時点で自動的にメモリ確保)
        Stage stage1(3);

        stage1.showEnemies();

        std::cout << "ステージ1をクリア!" << std::endl;

        // このブロックを抜けると自動的にデストラクタが呼ばれる
        // つまり delete[] を自分で書かなくても自動的に解放される!
    }

    std::cout << "\n=== ステージ2へ ===" << std::endl;

    {
        // ステージ2を作る
        Stage stage2(5);  // 敵が5体

        stage2.showEnemies();

        // ここでも自動的に解放される
    }

    std::cout << "\n=== ゲームクリア! ===" << std::endl;

    return 0;
}
