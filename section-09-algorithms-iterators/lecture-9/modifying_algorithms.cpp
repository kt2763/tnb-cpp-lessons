/*
 * C++モダンコース - セクション9: アルゴリズムとイテレータ
 * レクチャー9: 変更アルゴリズム（transform, copy, remove, unique）
 * 
 * このレクチャーでは、コンテナの内容を変更するアルゴリズムを学びます
 * ゲーム開発でのデータ変換、フィルタリング、重複除去を実践します
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <iomanip>
#include <list>
#include <set>

// ゲームアイテムのデータ構造
struct GameItem
{
    int item_id;
    std::string name;
    int level;
    int price;
    std::string rarity;
    bool is_equipped;
    
    GameItem(int id, const std::string& n, int lv, int p, const std::string& r, bool equipped = false)
        : item_id(id), name(n), level(lv), price(p), rarity(r), is_equipped(equipped) {}
    
    void display() const
    {
        std::cout << "[" << std::setw(3) << item_id << "] "
                  << std::setw(15) << name 
                  << " | Lv:" << std::setw(2) << level
                  << " | " << std::setw(4) << price << "G"
                  << " | " << std::setw(8) << rarity
                  << " | " << (is_equipped ? "装備中" : "未装備") << std::endl;
    }
    
    bool operator==(const GameItem& other) const
    {
        return item_id == other.item_id;
    }
    
    bool operator<(const GameItem& other) const
    {
        return item_id < other.item_id;
    }
};

// プレイヤーキャラクターのデータ
struct Player
{
    std::string name;
    int level;
    int health;
    int max_health;
    int experience;
    bool is_active;
    
    Player(const std::string& n, int lv, int hp, int max_hp, int exp, bool active = true)
        : name(n), level(lv), health(hp), max_health(max_hp), experience(exp), is_active(active) {}
    
    void display() const
    {
        std::cout << std::setw(10) << name 
                  << " | Lv:" << std::setw(2) << level
                  << " | HP:" << std::setw(3) << health << "/" << std::setw(3) << max_health
                  << " | EXP:" << std::setw(5) << experience
                  << " | " << (is_active ? "アクティブ" : "非アクティブ") << std::endl;
    }
};

void demonstrateTransform()
{
    std::cout << "\n=== transform の実例 ===" << std::endl;
    
    // プレイヤーの基本ダメージ値
    std::vector<int> base_damage = {45, 62, 38, 71, 89, 54, 67, 82, 49, 76};
    
    std::cout << "基本ダメージ: ";
    for (int damage : base_damage)
    {
        std::cout << std::setw(3) << damage << " ";
    }
    std::cout << std::endl;
    
    // 1. 単一範囲の変換（クリティカルヒット：1.5倍）
    std::vector<double> critical_damage(base_damage.size());
    std::transform(base_damage.begin(), base_damage.end(), critical_damage.begin(),
        [](int damage) 
        {
            return damage * 1.5;
        });
    
    std::cout << "クリティカル:   ";
    for (double damage : critical_damage)
    {
        std::cout << std::setw(3) << std::fixed << std::setprecision(0) << damage << " ";
    }
    std::cout << std::endl;
    
    // 2. 二つの範囲の組み合わせ（攻撃力 × 武器倍率）
    std::vector<double> weapon_multipliers = {1.2, 1.8, 0.9, 1.4, 2.0, 1.1, 1.6, 1.9, 1.0, 1.7};
    std::vector<double> final_damage(base_damage.size());
    
    std::cout << "武器倍率:     ";
    for (double mult : weapon_multipliers)
    {
        std::cout << std::setw(3) << std::fixed << std::setprecision(1) << mult << " ";
    }
    std::cout << std::endl;
    
    std::transform(base_damage.begin(), base_damage.end(), weapon_multipliers.begin(),
                   final_damage.begin(),
                   [](int damage, double multiplier) 
                   {
                       return damage * multiplier;
                   });
    
    std::cout << "最終ダメージ: ";
    for (double damage : final_damage)
    {
        std::cout << std::setw(3) << std::fixed << std::setprecision(0) << damage << " ";
    }
    std::cout << std::endl;
    
    // 3. オブジェクトの変換（レベルアップ処理）
    std::vector<Player> players = 
    {
        {"勇者", 24, 280, 300, 8900, true},
        {"魔法使い", 22, 160, 180, 8200, true},
        {"僧侶", 20, 200, 220, 7500, false},
        {"盗賊", 23, 190, 200, 8700, true}
    };
    
    std::cout << "\nレベルアップ前:" << std::endl;
    for (const auto& player : players)
    {
        player.display();
    }
    
    // レベルアップ処理
    std::transform(players.begin(), players.end(), players.begin(),
        [](const Player& p) 
        {
            Player leveled_up = p;
            if (p.is_active && p.experience >= 8000)
            {
                leveled_up.level++;
                leveled_up.max_health += 20;
                leveled_up.health = leveled_up.max_health; // 全回復
                leveled_up.experience -= 5000; // 経験値消費
            }
            return leveled_up;
        });
    
    std::cout << "\nレベルアップ後:" << std::endl;
    for (const auto& player : players)
    {
        player.display();
    }
}

void demonstrateCopyAlgorithms()
{
    std::cout << "\n=== copy系アルゴリズムの実例 ===" << std::endl;
    
    // ゲームアイテムの管理
    std::vector<GameItem> all_items = 
    {
        {101, "木の剣", 1, 50, "コモン", false},
        {102, "鉄の剣", 5, 200, "コモン", true},
        {103, "魔法の剣", 10, 800, "レア", false},
        {201, "布の帽子", 1, 30, "コモン", false},
        {202, "鉄の兜", 5, 150, "コモン", false},
        {203, "魔法の帽子", 10, 600, "レア", true},
        {301, "革の靴", 1, 40, "コモン", false},
        {302, "鉄の靴", 5, 120, "コモン", false}
    };
    
    std::cout << "全アイテム:" << std::endl;
    for (const auto& item : all_items)
    {
        item.display();
    }
    
    // 1. copy - 基本的なコピー
    std::vector<GameItem> inventory_backup;
    std::copy(all_items.begin(), all_items.end(), std::back_inserter(inventory_backup));
    std::cout << "\nバックアップ作成完了（" << inventory_backup.size() << "アイテム）" << std::endl;
    
    // 2. copy_if - 条件付きコピー（装備中のアイテム）
    std::vector<GameItem> equipped_items;
    std::copy_if(all_items.begin(), all_items.end(), std::back_inserter(equipped_items),
        [](const GameItem& item) 
        {
            return item.is_equipped;
        });
    
    std::cout << "\n装備中のアイテム:" << std::endl;
    for (const auto& item : equipped_items)
    {
        item.display();
    }
    
    // 3. copy_n - 指定個数のコピー（最初の3個）
    std::vector<GameItem> first_items;
    std::copy_n(all_items.begin(), 3, std::back_inserter(first_items));
    
    std::cout << "\n最初の3アイテム:" << std::endl;
    for (const auto& item : first_items)
    {
        item.display();
    }
    
    // 4. レア度別の分類
    std::vector<GameItem> common_items, rare_items;
    
    std::copy_if(all_items.begin(), all_items.end(), std::back_inserter(common_items),
        [](const GameItem& item) { return item.rarity == "コモン"; });
    
    std::copy_if(all_items.begin(), all_items.end(), std::back_inserter(rare_items),
        [](const GameItem& item) { return item.rarity == "レア"; });
    
    std::cout << "\nコモンアイテム数: " << common_items.size() << std::endl;
    std::cout << "レアアイテム数: " << rare_items.size() << std::endl;
}

void demonstrateRemoveAlgorithms()
{
    std::cout << "\n=== remove系アルゴリズムの実例 ===" << std::endl;
    
    // プレイヤーのスコア履歴（無効なスコアが混入）
    std::vector<int> score_history = {1200, 0, 1350, -1, 1180, 1400, 0, 1520, -1, 1600, 1450, 0};
    
    std::cout << "元のスコア履歴: ";
    for (int score : score_history)
    {
        std::cout << std::setw(4) << score << " ";
    }
    std::cout << std::endl;
    
    // 1. remove - 特定値の削除（0を削除）
    auto new_end = std::remove(score_history.begin(), score_history.end(), 0);
    std::cout << "0除去後のサイズ: " << (new_end - score_history.begin()) << std::endl;
    
    // 実際にコンテナから削除
    score_history.erase(new_end, score_history.end());
    
    std::cout << "0除去後:        ";
    for (int score : score_history)
    {
        std::cout << std::setw(4) << score << " ";
    }
    std::cout << std::endl;
    
    // 2. remove_if - 条件による削除（負の値を削除）
    auto invalid_end = std::remove_if(score_history.begin(), score_history.end(),
        [](int score) 
        {
            return score < 0;
        });
    score_history.erase(invalid_end, score_history.end());
    
    std::cout << "無効値除去後:   ";
    for (int score : score_history)
    {
        std::cout << std::setw(4) << score << " ";
    }
    std::cout << std::endl;
    
    // 3. ゲームアイテムからの不要アイテム削除
    std::vector<GameItem> player_inventory = 
    {
        {101, "壊れた剣", 1, 1, "ジャンク", false},
        {102, "鉄の剣", 5, 200, "コモン", true},
        {103, "壊れた盾", 1, 1, "ジャンク", false},
        {104, "魔法の剣", 10, 800, "レア", false},
        {105, "古い靴", 1, 1, "ジャンク", false},
        {106, "魔法の帽子", 10, 600, "レア", true}
    };
    
    std::cout << "\n整理前のインベントリ:" << std::endl;
    for (const auto& item : player_inventory)
    {
        item.display();
    }
    
    // ジャンクアイテムを削除
    auto junk_end = std::remove_if(player_inventory.begin(), player_inventory.end(),
        [](const GameItem& item) 
        {
            return item.rarity == "ジャンク";
        });
    player_inventory.erase(junk_end, player_inventory.end());
    
    std::cout << "\nジャンク整理後:" << std::endl;
    for (const auto& item : player_inventory)
    {
        item.display();
    }
}

void demonstrateUniqueAlgorithm()
{
    std::cout << "\n=== unique の実例 ===" << std::endl;
    
    // 重複するアイテムIDのリスト
    std::vector<int> item_ids = {101, 102, 102, 103, 103, 103, 104, 105, 105, 106, 106, 107};
    
    std::cout << "重複ありのアイテムID: ";
    for (int id : item_ids)
    {
        std::cout << id << " ";
    }
    std::cout << std::endl;
    
    // 1. 連続する重複を削除（ソート済みが前提）
    auto unique_end = std::unique(item_ids.begin(), item_ids.end());
    item_ids.erase(unique_end, item_ids.end());
    
    std::cout << "重複除去後:           ";
    for (int id : item_ids)
    {
        std::cout << id << " ";
    }
    std::cout << std::endl;
    
    // 2. カスタム比較での重複除去
    std::vector<GameItem> duplicate_items = 
    {
        {201, "回復ポーション", 1, 50, "コモン", false},
        {201, "回復ポーション", 1, 50, "コモン", false},
        {202, "魔力ポーション", 1, 80, "コモン", false},
        {201, "回復ポーション", 1, 50, "コモン", false},
        {203, "解毒剤", 1, 30, "コモン", false},
        {202, "魔力ポーション", 1, 80, "コモン", false}
    };
    
    std::cout << "\n重複アイテムリスト:" << std::endl;
    for (const auto& item : duplicate_items)
    {
        item.display();
    }
    
    // アイテムIDでソート
    std::sort(duplicate_items.begin(), duplicate_items.end(),
        [](const GameItem& a, const GameItem& b) 
        {
            return a.item_id < b.item_id;
        });
    
    // 重複除去（アイテムIDが同じものを除去）
    auto item_unique_end = std::unique(duplicate_items.begin(), duplicate_items.end(),
        [](const GameItem& a, const GameItem& b) 
        {
            return a.item_id == b.item_id;
        });
    duplicate_items.erase(item_unique_end, duplicate_items.end());
    
    std::cout << "\n重複除去後:" << std::endl;
    for (const auto& item : duplicate_items)
    {
        item.display();
    }
}

void demonstrateReplaceAlgorithms()
{
    std::cout << "\n=== replace系アルゴリズムの実例 ===" << std::endl;
    
    // ダメージ値の調整（バランス調整）
    std::vector<int> weapon_damage = {45, 30, 75, 90, 30, 60, 30, 85, 70, 30};
    
    std::cout << "調整前の武器ダメージ: ";
    for (int damage : weapon_damage)
    {
        std::cout << std::setw(3) << damage << " ";
    }
    std::cout << std::endl;
    
    // 1. replace - 特定値の置換（30を35に強化）
    std::replace(weapon_damage.begin(), weapon_damage.end(), 30, 35);
    
    std::cout << "弱武器強化後:         ";
    for (int damage : weapon_damage)
    {
        std::cout << std::setw(3) << damage << " ";
    }
    std::cout << std::endl;
    
    // 2. replace_if - 条件による置換（強すぎる武器を調整）
    std::replace_if(weapon_damage.begin(), weapon_damage.end(),
        [](int damage) { return damage > 80; },
        80); // 上限を80に設定
    
    std::cout << "強武器調整後:         ";
    for (int damage : weapon_damage)
    {
        std::cout << std::setw(3) << damage << " ";
    }
    std::cout << std::endl;
    
    // 3. replace_copy - コピーしながら置換
    std::vector<int> balanced_damage;
    std::replace_copy_if(weapon_damage.begin(), weapon_damage.end(),
                        std::back_inserter(balanced_damage),
                        [](int damage) { return damage < 40; },
                        40); // 最低ダメージを40に設定
    
    std::cout << "最終バランス調整:     ";
    for (int damage : balanced_damage)
    {
        std::cout << std::setw(3) << damage << " ";
    }
    std::cout << std::endl;
}

void demonstrateReverseAndRotate()
{
    std::cout << "\n=== reverse と rotate の実例 ===" << std::endl;
    
    // ターン制バトルの行動順
    std::vector<std::string> turn_order = {"プレイヤー", "敵A", "敵B", "敵C", "NPC"};
    
    std::cout << "初期行動順: ";
    for (const auto& actor : turn_order)
    {
        std::cout << actor << " ";
    }
    std::cout << std::endl;
    
    // 1. reverse - 順序の逆転
    auto reversed_order = turn_order;
    std::reverse(reversed_order.begin(), reversed_order.end());
    
    std::cout << "逆順行動:   ";
    for (const auto& actor : reversed_order)
    {
        std::cout << actor << " ";
    }
    std::cout << std::endl;
    
    // 2. rotate - 行動順のローテーション
    auto rotated_order = turn_order;
    // 2番目の要素を先頭に持ってくる
    std::rotate(rotated_order.begin(), rotated_order.begin() + 2, rotated_order.end());
    
    std::cout << "ローテート: ";
    for (const auto& actor : rotated_order)
    {
        std::cout << actor << " ";
    }
    std::cout << std::endl;
    
    // 3. パーティの並び替え（タンクを前に）
    std::vector<std::string> party = {"魔法使い", "盗賊", "戦士", "僧侶", "弓使い"};
    
    std::cout << "\n元のパーティ配置: ";
    for (const auto& member : party)
    {
        std::cout << member << " ";
    }
    std::cout << std::endl;
    
    // 戦士（タンク）を最前列に移動
    auto tank_pos = std::find(party.begin(), party.end(), "戦士");
    if (tank_pos != party.end())
    {
        std::rotate(party.begin(), tank_pos, tank_pos + 1);
    }
    
    std::cout << "戦術配置後:       ";
    for (const auto& member : party)
    {
        std::cout << member << " ";
    }
    std::cout << std::endl;
}

void demonstrateGameInventoryManagement()
{
    std::cout << "\n=== ゲームインベントリ管理システム ===" << std::endl;
    
    // 混沌としたインベントリ
    std::vector<GameItem> messy_inventory = 
    {
        {101, "回復ポーション", 1, 50, "コモン", false},
        {105, "壊れた剣", 1, 1, "ジャンク", false},
        {101, "回復ポーション", 1, 50, "コモン", false},
        {203, "魔法の杖", 8, 600, "レア", true},
        {107, "古い靴", 1, 1, "ジャンク", false},
        {101, "回復ポーション", 1, 50, "コモン", false},
        {204, "ドラゴンの鱗", 15, 1000, "エピック", false},
        {106, "錆びた鎧", 1, 1, "ジャンク", false}
    };
    
    std::cout << "整理前のインベントリ:" << std::endl;
    for (const auto& item : messy_inventory)
    {
        item.display();
    }
    
    // 1. ジャンクアイテムの削除
    auto junk_removed = std::remove_if(messy_inventory.begin(), messy_inventory.end(),
        [](const GameItem& item) 
        {
            return item.rarity == "ジャンク";
        });
    messy_inventory.erase(junk_removed, messy_inventory.end());
    
    std::cout << "\nジャンク削除後:" << std::endl;
    for (const auto& item : messy_inventory)
    {
        item.display();
    }
    
    // 2. アイテムIDでソート
    std::sort(messy_inventory.begin(), messy_inventory.end(),
        [](const GameItem& a, const GameItem& b) 
        {
            return a.item_id < b.item_id;
        });
    
    // 3. 重複アイテムの統合
    auto unique_items_end = std::unique(messy_inventory.begin(), messy_inventory.end(),
        [](const GameItem& a, const GameItem& b) 
        {
            return a.item_id == b.item_id;
        });
    messy_inventory.erase(unique_items_end, messy_inventory.end());
    
    std::cout << "\n重複除去・ソート後:" << std::endl;
    for (const auto& item : messy_inventory)
    {
        item.display();
    }
    
    // 4. レア度による価格調整
    std::transform(messy_inventory.begin(), messy_inventory.end(), messy_inventory.begin(),
        [](const GameItem& item) 
        {
            GameItem adjusted = item;
            if (item.rarity == "エピック")
            {
                adjusted.price = static_cast<int>(adjusted.price * 1.2); // 20%値上げ
            }
            else if (item.rarity == "レア")
            {
                adjusted.price = static_cast<int>(adjusted.price * 1.1); // 10%値上げ
            }
            return adjusted;
        });
    
    std::cout << "\n価格調整後の最終インベントリ:" << std::endl;
    for (const auto& item : messy_inventory)
    {
        item.display();
    }
    
    // 5. 統計情報
    int total_value = std::accumulate(messy_inventory.begin(), messy_inventory.end(), 0,
        [](int sum, const GameItem& item) 
        {
            return sum + item.price;
        });
    
    int equipped_count = std::count_if(messy_inventory.begin(), messy_inventory.end(),
        [](const GameItem& item) 
        {
            return item.is_equipped;
        });
    
    std::cout << "\nインベントリ統計:" << std::endl;
    std::cout << "アイテム数: " << messy_inventory.size() << std::endl;
    std::cout << "総価値: " << total_value << "G" << std::endl;
    std::cout << "装備中アイテム: " << equipped_count << "個" << std::endl;
}

int main()
{
    std::cout << "C++アルゴリズムとイテレータ - レクチャー9: 変更アルゴリズム" << std::endl;
    std::cout << "=========================================================" << std::endl;
    
    // 各変更アルゴリズムの実演
    demonstrateTransform();
    demonstrateCopyAlgorithms();
    demonstrateRemoveAlgorithms();
    demonstrateUniqueAlgorithm();
    demonstrateReplaceAlgorithms();
    demonstrateReverseAndRotate();
    demonstrateGameInventoryManagement();
    
    std::cout << "\n=== まとめ ===" << std::endl;
    std::cout << "1. transform: 要素の変換（単一/二重範囲）" << std::endl;
    std::cout << "2. copy系: 条件付きコピー（copy_if, copy_n）" << std::endl;
    std::cout << "3. remove系: 要素の削除（remove, remove_if）" << std::endl;
    std::cout << "4. unique: 重複要素の除去" << std::endl;
    std::cout << "5. replace系: 要素の置換（replace, replace_if）" << std::endl;
    std::cout << "6. reverse/rotate: 要素順序の変更" << std::endl;
    std::cout << "注意: remove系は実際の削除ではなく、eraseと組み合わせる" << std::endl;
    std::cout << "ゲーム開発では、データ変換、フィルタリング、整理で重要" << std::endl;
    
    return 0;
}