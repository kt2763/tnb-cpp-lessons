#include <iostream>
#include <vector>
#include <string>

struct EnemyData
{
    std::string      name;
    std::vector<int> damage_log;

    EnemyData(std::string n, int log_size)
        : name(std::move(n))
        , damage_log(log_size, 10)
    {
        std::cout << "[コンストラクタ] " << name << std::endl;
    }

    // コピーコンストラクタ
    EnemyData(const EnemyData& other)
        : name(other.name)
        , damage_log(other.damage_log)
    {
        std::cout << "[コピー] " << name << std::endl;
    }

    // ムーブコンストラクタ：リソースを「盗む」
    // noexceptつけないとコピーコンストラクタへフォールバックすることがある
    EnemyData(EnemyData&& other) noexcept
        : name(std::move(other.name))
        , damage_log(std::move(other.damage_log))
    {
        std::cout << "[ムーブ] " << name << std::endl;
        // other.name と other.damage_log は空になった
    }

    // ムーブ代入演算子
    EnemyData& operator=(EnemyData&& other) noexcept
    {
        if (this != &other)
        {
            name = std::move(other.name);
            damage_log = std::move(other.damage_log);
        }
        std::cout << "[ムーブ代入] " << name << std::endl;
        return *this;
    }
};

// ✅ rvalue参照で受け取ればムーブできる
void process_move(EnemyData&& data)
{
    EnemyData local = std::move(data); // ここでムーブコンストラクタが呼ばれる
    std::cout << "処理中: " << local.name << std::endl;
}

int main()
{
    EnemyData enemy("ドラゴン", 10000);

    std::cout << "\n--- ムーブ渡し ---" << std::endl;
    process_move(std::move(enemy)); // コピーなし！

    std::cout << "enemy の name は今: \"" << enemy.name << "\"" << std::endl;

    // --- vector への push でのムーブ ---
    std::cout << "\n--- vectorへのムーブ ---" << std::endl;
    std::vector<EnemyData> list;
    list.reserve(2);
    list.push_back(EnemyData("ゴブリン", 100)); // 一時オブジェクト→ムーブ
    EnemyData orc("オーク", 200);
    list.push_back(std::move(orc));              // 明示的ムーブ

    return 0;
}
