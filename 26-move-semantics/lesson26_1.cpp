#include <iostream>
#include <vector>
#include <string>

struct EnemyData
{
    std::string name;
    std::vector<int> damage_log; // 大量のダメージ履歴（重いデータのイメージ）

    EnemyData(std::string n, int log_size)
        : name(std::move(n))
        , damage_log(log_size, 10)
    {
        std::cout << "[コンストラクタ] " << name << std::endl;
    }

    // コピーコンストラクタ（コピーが起きたことを確認するためのログ）
    EnemyData(const EnemyData& other)
        : name(other.name)
        , damage_log(other.damage_log)
    {
        std::cout << "[コピー] " << name << " (" << damage_log.size() << "件のデータ)" << std::endl;
    }
};

// ❌ 値渡し：呼び出し時にコピーが発生する
void process_copy(EnemyData data)
{
    std::cout << "処理中: " << data.name << std::endl;
}

int main()
{
    EnemyData enemy("ドラゴン", 10000); // 10000件の履歴

    std::cout << "\n--- 値渡し（コピー発生）---" << std::endl;
    process_copy(enemy); // ← ここでコピーコンストラクタが呼ばれる

    std::cout << "\nenemyはまだ生きている: " << enemy.name << std::endl;

    return 0;
}
