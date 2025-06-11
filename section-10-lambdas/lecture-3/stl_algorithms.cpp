/**
 * @file stl_algorithms.cpp
 * @brief STLアルゴリズムとラムダ式の組み合わせ
 * 
 * STLアルゴリズムとラムダ式を組み合わせることで、
 * ゲームデータのソート、フィルタリング、検索などが簡潔に実現できます。
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <string>
#include <map>
#include <set>
#include <functional>

struct Player
{
    std::string name;
    int level;
    int score;
    std::string playerClass;
    bool isOnline;
    
    void display() const
    {
        std::cout << name << " (Lv." << level << ", " << playerClass 
                  << ", スコア: " << score << ", "
                  << (isOnline ? "オンライン" : "オフライン") << ")\n";
    }
};

struct Item
{
    std::string name;
    std::string category;
    int value;
    int rarity; // 1-5 (1=コモン, 5=レジェンダリー)
    
    void display() const
    {
        std::cout << name << " (" << category << ", 価値: " << value 
                  << ", レア度: " << rarity << ")\n";
    }
};

int main()
{
    // テストデータの初期化
    std::vector<Player> players = 
    {
        {"タロウ", 15, 1500, "戦士", true},
        {"ハナコ", 22, 2800, "魔法使い", false},
        {"ジロウ", 8, 650, "盗賊", true},
        {"サクラ", 35, 4200, "僧侶", true},
        {"レン", 28, 3100, "弓手", false}
    };
    
    std::vector<Item> inventory = 
    {
        {"鉄の剣", "武器", 150, 2},
        {"ポーション", "消耗品", 50, 1},
        {"ミスリルの鎧", "防具", 800, 4},
        {"魔法の杖", "武器", 300, 3},
        {"エリクサー", "消耗品", 200, 2},
        {"ドラゴンシールド", "防具", 1500, 5},
        {"フェニックスの羽", "アクセサリー", 5000, 5}
    };
    
    // 1. std::sort - カスタムソート
    std::cout << "=== プレイヤーソート ===\n";
    
    // レベルでソート（降順）
    auto playersCopyForLevel = players;
    std::sort(playersCopyForLevel.begin(), playersCopyForLevel.end(),
        [](const Player& a, const Player& b)
        {
            return a.level > b.level; // 降順
        });
    
    std::cout << "レベル順（降順）:\n";
    for (const auto& player : playersCopyForLevel)
    {
        player.display();
    }
    
    // スコアでソート（昇順）
    auto playersCopyForScore = players;
    std::sort(playersCopyForScore.begin(), playersCopyForScore.end(),
        [](const Player& a, const Player& b)
        {
            return a.score < b.score; // 昇順
        });
    
    std::cout << "\nスコア順（昇順）:\n";
    for (const auto& player : playersCopyForScore)
    {
        player.display();
    }
    
    // 2. std::find_if - 条件検索
    std::cout << "\n=== 条件検索 ===\n";
    
    // オンラインの魔法使いを検索
    auto mageIt = std::find_if(players.begin(), players.end(),
        [](const Player& p)
        {
            return p.playerClass == "魔法使い" && p.isOnline;
        });
    
    if (mageIt != players.end())
    {
        std::cout << "オンラインの魔法使いが見つかりました: ";
        mageIt->display();
    }
    else
    {
        std::cout << "オンラインの魔法使いはいません\n";
    }
    
    // レベル30以上のプレイヤーを検索
    auto highLevelIt = std::find_if(players.begin(), players.end(),
        [](const Player& p)
        {
            return p.level >= 30;
        });
    
    if (highLevelIt != players.end())
    {
        std::cout << "高レベルプレイヤー: ";
        highLevelIt->display();
    }
    
    // 3. std::count_if - 条件カウント
    std::cout << "\n=== 条件カウント ===\n";
    
    // オンラインプレイヤー数
    auto onlineCount = std::count_if(players.begin(), players.end(),
        [](const Player& p)
        {
            return p.isOnline;
        });
    
    std::cout << "オンラインプレイヤー数: " << onlineCount << "\n";
    
    // レアアイテム数（レア度 4 以上）
    auto rareItemCount = std::count_if(inventory.begin(), inventory.end(),
        [](const Item& item)
        {
            return item.rarity >= 4;
        });
    
    std::cout << "レアアイテム数（レア度 4+）: " << rareItemCount << "\n";
    
    // 4. std::for_each - 全要素に処理適用
    std::cout << "\n=== バフ適用 ===\n";
    
    // 全プレイヤーにレベルアップボーナス
    std::for_each(players.begin(), players.end(),
        [](Player& p)
        {
            int oldLevel = p.level;
            p.level += 2;
            p.score += 100;
            std::cout << p.name << ": Lv." << oldLevel << " → Lv." << p.level 
                      << ", スコア +100\n";
        });
    
    // 5. std::transform - 変換処理
    std::cout << "\n=== アイテム価値計算 ===\n";
    
    // アイテムの調整価値を計算（レア度を考慮）
    std::vector<int> adjustedValues;
    std::transform(inventory.begin(), inventory.end(), 
                   std::back_inserter(adjustedValues),
        [](const Item& item)
        {
            // レア度に基づいた価値調整
            float multiplier = 1.0f + (item.rarity - 1) * 0.5f;
            return static_cast<int>(item.value * multiplier);
        });
    
    std::cout << "調整後のアイテム価値:\n";
    for (size_t i = 0; i < inventory.size(); ++i)
    {
        std::cout << inventory[i].name << ": " << inventory[i].value 
                  << " → " << adjustedValues[i] << "\n";
    }
    
    // 6. std::accumulate - 累積処理
    std::cout << "\n=== 統計情報 ===\n";
    
    // 全プレイヤーの合計スコア
    int totalScore = std::accumulate(players.begin(), players.end(), 0,
        [](int sum, const Player& p)
        {
            return sum + p.score;
        });
    
    std::cout << "全プレイヤーの合計スコア: " << totalScore << "\n";
    
    // インベントリの合計価値
    int totalValue = std::accumulate(inventory.begin(), inventory.end(), 0,
        [](int sum, const Item& item)
        {
            return sum + item.value;
        });
    
    std::cout << "インベントリの合計価値: " << totalValue << "\n";
    
    // 7. std::partition - 条件で分割
    std::cout << "\n=== プレイヤー分類 ===\n";
    
    auto playersForPartition = players;
    
    // オンラインプレイヤーとオフラインプレイヤーで分割
    auto partitionPoint = std::partition(playersForPartition.begin(), 
                                        playersForPartition.end(),
        [](const Player& p)
        {
            return p.isOnline;
        });
    
    std::cout << "オンラインプレイヤー:\n";
    for (auto it = playersForPartition.begin(); it != partitionPoint; ++it)
    {
        it->display();
    }
    
    std::cout << "\nオフラインプレイヤー:\n";
    for (auto it = partitionPoint; it != playersForPartition.end(); ++it)
    {
        it->display();
    }
    
    // 8. std::remove_if - 条件で削除
    std::cout << "\n=== 低価値アイテムの削除 ===\n";
    
    auto inventoryFiltered = inventory;
    
    // 価値100未満のアイテムを削除
    auto removePoint = std::remove_if(inventoryFiltered.begin(), 
                                     inventoryFiltered.end(),
        [](const Item& item)
        {
            return item.value < 100;
        });
    
    inventoryFiltered.erase(removePoint, inventoryFiltered.end());
    
    std::cout << "価値100以上のアイテム:\n";
    for (const auto& item : inventoryFiltered)
    {
        item.display();
    }
    
    // 9. std::any_of, std::all_of, std::none_of - 条件チェック
    std::cout << "\n=== 条件チェック ===\n";
    
    // レジェンダリーアイテムがあるか
    bool hasLegendary = std::any_of(inventory.begin(), inventory.end(),
        [](const Item& item)
        {
            return item.rarity == 5;
        });
    
    std::cout << "レジェンダリーアイテム所持: " 
              << (hasLegendary ? "はい" : "いいえ") << "\n";
    
    // 全プレイヤーがレベル10以上か
    bool allHighLevel = std::all_of(players.begin(), players.end(),
        [](const Player& p)
        {
            return p.level >= 10;
        });
    
    std::cout << "全プレイヤーがLv.10以上: " 
              << (allHighLevel ? "はい" : "いいえ") << "\n";
    
    // スコア5000以上のプレイヤーがいないか
    bool noTopPlayer = std::none_of(players.begin(), players.end(),
        [](const Player& p)
        {
            return p.score >= 5000;
        });
    
    std::cout << "スコア5000以上のプレイヤーなし: " 
              << (noTopPlayer ? "はい" : "いいえ") << "\n";
    
    // 10. 複雑なゲームロジックの例
    std::cout << "\n=== ギルドマッチングシステム ===\n";
    
    // プレイヤーをレベル帯でグループ分け
    std::map<std::string, std::vector<Player>> levelGroups;
    
    std::for_each(players.begin(), players.end(),
        [&levelGroups](const Player& p)
        {
            std::string group;
            if (p.level < 15)
                group = "初心者";
            else if (p.level < 25)
                group = "中級者";
            else if (p.level < 35)
                group = "上級者";
            else
                group = "エキスパート";
            
            levelGroups[group].push_back(p);
        });
    
    for (const auto& [groupName, groupPlayers] : levelGroups)
    {
        std::cout << "\n[" << groupName << "] (" << groupPlayers.size() << "人):\n";
        for (const auto& player : groupPlayers)
        {
            std::cout << "  ";
            player.display();
        }
    }
    
    return 0;
}

/**
 * まとめ：
 * - std::sort: カスタムソート条件をラムダで指定
 * - std::find_if: 条件にマッチする要素を検索
 * - std::count_if: 条件にマッチする要素数をカウント
 * - std::for_each: 全要素に処理を適用
 * - std::transform: 要素を変換して新しいコンテナに格納
 * - std::accumulate: 累積処理で統計情報を算出
 * - std::partition: 条件でコンテナを分割
 * - std::remove_if: 条件にマッチする要素を削除
 * - std::any_of/all_of/none_of: 条件チェック
 * - ゲームデータの処理に非常に有用
 */