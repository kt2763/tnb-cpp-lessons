/*
 * C++モダンコース - セクション9: アルゴリズムとイテレータ
 * レクチャー4: 二分探索アルゴリズム（binary_search, lower_bound, upper_bound）
 * 
 * このレクチャーでは、ソート済みデータに対する効率的な検索アルゴリズムを学びます
 * ゲーム開発でのアイテム検索、レベル管理、効率的なデータ取得の実装を学習します
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <iomanip>
#include <chrono>

// ゲームアイテムのデータ構造
struct GameItem
{
    int item_id;
    std::string name;
    int level_required;
    int value;
    std::string rarity;
    
    GameItem(int id, const std::string& n, int lv, int v, const std::string& r)
        : item_id(id), name(n), level_required(lv), value(v), rarity(r) {}
    
    void display() const
    {
        std::cout << "ID:" << std::setw(3) << item_id 
                  << " | " << std::setw(15) << name
                  << " | Lv:" << std::setw(2) << level_required
                  << " | " << std::setw(4) << value << "G"
                  << " | " << rarity << std::endl;
    }
    
    // IDでの比較演算子
    bool operator<(const GameItem& other) const
    {
        return item_id < other.item_id;
    }
    
    bool operator==(const GameItem& other) const
    {
        return item_id == other.item_id;
    }
};

// プレイヤーレベルデータ
struct PlayerLevel
{
    int level;
    int experience_required;
    std::string title;
    
    PlayerLevel(int lv, int exp, const std::string& t)
        : level(lv), experience_required(exp), title(t) {}
    
    void display() const
    {
        std::cout << "Lv:" << std::setw(2) << level
                  << " | EXP:" << std::setw(6) << experience_required
                  << " | " << title << std::endl;
    }
    
    bool operator<(const PlayerLevel& other) const
    {
        return level < other.level;
    }
};

void demonstrateBinarySearch()
{
    std::cout << "\n=== binary_search の実例 ===" << std::endl;
    
    // ソート済みのアイテムデータベース（IDでソート済み）
    std::vector<GameItem> item_database = 
    {
        {101, "木の剣", 1, 50, "コモン"},
        {105, "鉄の剣", 5, 200, "コモン"},
        {110, "魔法の剣", 10, 800, "レア"},
        {115, "ドラゴンソード", 15, 1500, "エピック"},
        {120, "聖なる剣", 20, 3000, "レジェンダリー"},
        {201, "布の帽子", 1, 30, "コモン"},
        {205, "鉄の兜", 5, 150, "コモン"},
        {210, "魔法の帽子", 10, 600, "レア"},
        {215, "ドラゴンヘルム", 15, 1200, "エピック"},
        {220, "神聖な冠", 20, 2500, "レジェンダリー"}
    };
    
    std::cout << "アイテムデータベース（IDでソート済み）:" << std::endl;
    for (const auto& item : item_database)
    {
        item.display();
    }
    
    // 1. 特定IDのアイテムが存在するかチェック
    std::cout << "\n1. binary_search - アイテム存在チェック:" << std::endl;
    
    std::vector<int> search_ids = {110, 150, 205, 999};
    
    for (int search_id : search_ids)
    {
        GameItem target(search_id, "", 0, 0, "");
        bool found = std::binary_search(item_database.begin(), item_database.end(), target);
        
        std::cout << "アイテムID " << search_id << ": " 
                  << (found ? "見つかりました" : "見つかりません") << std::endl;
    }
}

void demonstrateLowerBound()
{
    std::cout << "\n=== lower_bound の実例 ===" << std::endl;
    
    // レベル別の経験値テーブル（レベルでソート済み）
    std::vector<PlayerLevel> level_table = 
    {
        {1, 0, "初心者"},
        {5, 1000, "見習い"},
        {10, 5000, "戦士"},
        {15, 15000, "熟練者"},
        {20, 35000, "専門家"},
        {25, 65000, "達人"},
        {30, 100000, "マスター"},
        {35, 150000, "グランドマスター"},
        {40, 220000, "レジェンド"}
    };
    
    std::cout << "レベルテーブル:" << std::endl;
    for (const auto& level_data : level_table)
    {
        level_data.display();
    }
    
    // 1. 特定レベル以上の最初の要素を検索
    std::cout << "\n1. lower_bound - 特定レベル以上の検索:" << std::endl;
    
    std::vector<int> target_levels = {12, 25, 50};
    
    for (int target_level : target_levels)
    {
        PlayerLevel search_target(target_level, 0, "");
        auto it = std::lower_bound(level_table.begin(), level_table.end(), search_target);
        
        std::cout << "レベル" << target_level << "以上の最初のデータ: ";
        if (it != level_table.end())
        {
            it->display();
        }
        else
        {
            std::cout << "見つかりません" << std::endl;
        }
    }
    
    // 2. 範囲検索での活用
    std::cout << "\n2. lower_bound を使った範囲検索:" << std::endl;
    int min_level = 15;
    int max_level = 30;
    
    PlayerLevel min_search(min_level, 0, "");
    PlayerLevel max_search(max_level + 1, 0, ""); // max_levelより大きい最初の要素
    
    auto start_it = std::lower_bound(level_table.begin(), level_table.end(), min_search);
    auto end_it = std::lower_bound(level_table.begin(), level_table.end(), max_search);
    
    std::cout << "レベル" << min_level << "～" << max_level << "の範囲:" << std::endl;
    for (auto it = start_it; it != end_it; ++it)
    {
        it->display();
    }
}

void demonstrateUpperBound()
{
    std::cout << "\n=== upper_bound の実例 ===" << std::endl;
    
    // 価値でソートされたアイテムリスト
    std::vector<GameItem> items_by_value = 
    {
        {201, "布の帽子", 1, 30, "コモン"},
        {101, "木の剣", 1, 50, "コモン"},
        {205, "鉄の兜", 5, 150, "コモン"},
        {105, "鉄の剣", 5, 200, "コモン"},
        {210, "魔法の帽子", 10, 600, "レア"},
        {110, "魔法の剣", 10, 800, "レア"},
        {215, "ドラゴンヘルム", 15, 1200, "エピック"},
        {115, "ドラゴンソード", 15, 1500, "エピック"},
        {220, "神聖な冠", 20, 2500, "レジェンダリー"},
        {120, "聖なる剣", 20, 3000, "レジェンダリー"}
    };
    
    // 価値でソート
    std::sort(items_by_value.begin(), items_by_value.end(),
        [](const GameItem& a, const GameItem& b) 
        {
            return a.value < b.value;
        });
    
    std::cout << "アイテム（価値でソート済み）:" << std::endl;
    for (const auto& item : items_by_value)
    {
        item.display();
    }
    
    // 1. 特定価値より高いアイテムの検索
    std::cout << "\n1. upper_bound - 特定価値より高いアイテム:" << std::endl;
    
    std::vector<int> price_thresholds = {500, 1000, 2000};
    
    for (int threshold : price_thresholds)
    {
        auto it = std::upper_bound(items_by_value.begin(), items_by_value.end(), threshold,
            [](int value, const GameItem& item) 
            {
                return value < item.value;
            });
        
        std::cout << threshold << "Gより高価なアイテム数: " 
                  << (items_by_value.end() - it) << "個" << std::endl;
        
        if (it != items_by_value.end())
        {
            std::cout << "最初のアイテム: ";
            it->display();
        }
    }
}

void demonstrateEqualRange()
{
    std::cout << "\n=== equal_range の実例 ===" << std::endl;
    
    // レベル要求でソートされたアイテム（同じレベル要求のアイテムが複数存在）
    std::vector<GameItem> items_by_level = 
    {
        {101, "木の剣", 1, 50, "コモン"},
        {201, "布の帽子", 1, 30, "コモン"},
        {301, "革の靴", 1, 40, "コモン"},
        {105, "鉄の剣", 5, 200, "コモン"},
        {205, "鉄の兜", 5, 150, "コモン"},
        {110, "魔法の剣", 10, 800, "レア"},
        {210, "魔法の帽子", 10, 600, "レア"},
        {310, "魔法の靴", 10, 700, "レア"},
        {115, "ドラゴンソード", 15, 1500, "エピック"},
        {215, "ドラゴンヘルム", 15, 1200, "エピック"}
    };
    
    // レベル要求でソート
    std::sort(items_by_level.begin(), items_by_level.end(),
        [](const GameItem& a, const GameItem& b) 
        {
            return a.level_required < b.level_required;
        });
    
    std::cout << "アイテム（必要レベルでソート済み）:" << std::endl;
    for (const auto& item : items_by_level)
    {
        item.display();
    }
    
    // 1. 特定レベルで使用可能なアイテムを全て取得
    std::cout << "\n1. equal_range - 特定レベルのアイテム一覧:" << std::endl;
    
    std::vector<int> check_levels = {1, 10, 20};
    
    for (int level : check_levels)
    {
        auto range = std::equal_range(items_by_level.begin(), items_by_level.end(), level,
            [](const GameItem& item, int lv) { return item.level_required < lv; },
            [](int lv, const GameItem& item) { return lv < item.level_required; });
        
        std::cout << "レベル" << level << "で使用可能なアイテム:" << std::endl;
        for (auto it = range.first; it != range.second; ++it)
        {
            std::cout << "  ";
            it->display();
        }
        
        if (range.first == range.second)
        {
            std::cout << "  該当アイテムなし" << std::endl;
        }
    }
}

void demonstratePerformanceComparison()
{
    std::cout << "\n=== パフォーマンス比較：線形検索 vs 二分探索 ===" << std::endl;
    
    // 大量のデータを作成
    std::vector<int> large_dataset;
    for (int i = 1; i <= 100000; ++i)
    {
        large_dataset.push_back(i * 2); // 偶数のみ
    }
    
    int search_target = 50000;
    
    // 線形検索の時間測定
    auto start_time = std::chrono::high_resolution_clock::now();
    
    auto linear_result = std::find(large_dataset.begin(), large_dataset.end(), search_target);
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto linear_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    // 二分探索の時間測定
    start_time = std::chrono::high_resolution_clock::now();
    
    bool binary_result = std::binary_search(large_dataset.begin(), large_dataset.end(), search_target);
    
    end_time = std::chrono::high_resolution_clock::now();
    auto binary_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    std::cout << "データサイズ: " << large_dataset.size() << "要素" << std::endl;
    std::cout << "検索対象: " << search_target << std::endl;
    std::cout << "線形検索時間: " << linear_duration.count() << " マイクロ秒" << std::endl;
    std::cout << "二分探索時間: " << binary_duration.count() << " マイクロ秒" << std::endl;
    std::cout << "結果: " << (linear_result != large_dataset.end() && binary_result ? "両方とも見つかりました" : "見つかりません") << std::endl;
    
    if (linear_duration.count() > 0)
    {
        double speedup = static_cast<double>(linear_duration.count()) / binary_duration.count();
        std::cout << "二分探索は線形検索の約" << std::fixed << std::setprecision(1) << speedup << "倍高速" << std::endl;
    }
}

void demonstrateGameInventorySystem()
{
    std::cout << "\n=== ゲームインベントリシステムでの活用 ===" << std::endl;
    
    // プレイヤーの所持アイテム（IDでソート済み）
    std::vector<GameItem> player_inventory = 
    {
        {101, "木の剣", 1, 50, "コモン"},
        {105, "鉄の剣", 5, 200, "コモン"},
        {110, "魔法の剣", 10, 800, "レア"},
        {201, "布の帽子", 1, 30, "コモン"},
        {205, "鉄の兜", 5, 150, "コモン"},
        {210, "魔法の帽子", 10, 600, "レア"},
        {301, "革の靴", 1, 40, "コモン"},
        {310, "魔法の靴", 10, 700, "レア"}
    };
    
    std::cout << "プレイヤーのインベントリ:" << std::endl;
    for (const auto& item : player_inventory)
    {
        item.display();
    }
    
    // 1. アイテム所持チェック
    std::cout << "\n1. アイテム所持チェック:" << std::endl;
    std::vector<int> check_items = {110, 150, 210, 999};
    
    for (int item_id : check_items)
    {
        GameItem search_item(item_id, "", 0, 0, "");
        bool has_item = std::binary_search(player_inventory.begin(), player_inventory.end(), search_item);
        
        std::cout << "アイテムID " << item_id << ": " 
                  << (has_item ? "所持している" : "所持していない") << std::endl;
    }
    
    // 2. 価値範囲でのアイテム検索
    std::cout << "\n2. 価値範囲でのアイテム検索（100G～500G）:" << std::endl;
    
    // 価値でソートしたコピーを作成
    auto value_sorted_inventory = player_inventory;
    std::sort(value_sorted_inventory.begin(), value_sorted_inventory.end(),
        [](const GameItem& a, const GameItem& b) 
        {
            return a.value < b.value;
        });
    
    int min_value = 100;
    int max_value = 500;
    
    auto lower_it = std::lower_bound(value_sorted_inventory.begin(), value_sorted_inventory.end(), min_value,
        [](const GameItem& item, int value) { return item.value < value; });
    
    auto upper_it = std::upper_bound(value_sorted_inventory.begin(), value_sorted_inventory.end(), max_value,
        [](int value, const GameItem& item) { return value < item.value; });
    
    std::cout << "価値" << min_value << "G～" << max_value << "Gのアイテム:" << std::endl;
    for (auto it = lower_it; it != upper_it; ++it)
    {
        std::cout << "  ";
        it->display();
    }
}

int main()
{
    std::cout << "C++アルゴリズムとイテレータ - レクチャー4: 二分探索アルゴリズム" << std::endl;
    std::cout << "==========================================================" << std::endl;
    
    // 各二分探索アルゴリズムの実演
    demonstrateBinarySearch();
    demonstrateLowerBound();
    demonstrateUpperBound();
    demonstrateEqualRange();
    demonstratePerformanceComparison();
    demonstrateGameInventorySystem();
    
    std::cout << "\n=== まとめ ===" << std::endl;
    std::cout << "1. binary_search: 要素の存在確認（O(log n)）" << std::endl;
    std::cout << "2. lower_bound: 指定値以上の最初の要素" << std::endl;
    std::cout << "3. upper_bound: 指定値より大きい最初の要素" << std::endl;
    std::cout << "4. equal_range: 指定値と等しい要素の範囲" << std::endl;
    std::cout << "注意: すべてソート済みデータが前提条件" << std::endl;
    std::cout << "ゲーム開発では、アイテム検索やレベル管理で効率的なデータアクセスが可能" << std::endl;
    
    return 0;
}