/*
 * C++モダンコース - セクション9: アルゴリズムとイテレータ
 * レクチャー2: アルゴリズムの基礎（find, count, for_each）
 * 
 * このレクチャーでは、STLの基本的なアルゴリズムについて学びます
 * ゲーム開発でよく使用される検索、カウント、反復処理を実践します
 */

#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <string>
#include <functional>

// ゲームキャラクターのデータ構造
struct GameCharacter
{
    std::string name;
    std::string job_class;
    int level;
    int health;
    int attack_power;
    
    GameCharacter(const std::string& n, const std::string& job, int lv, int hp, int atk)
        : name(n), job_class(job), level(lv), health(hp), attack_power(atk) {}
    
    void display() const
    {
        std::cout << name << " (" << job_class << ") - Lv:" << level 
                  << " HP:" << health << " ATK:" << attack_power << std::endl;
    }
    
    bool operator==(const GameCharacter& other) const
    {
        return name == other.name;
    }
};

// アイテムデータ構造
struct GameItem
{
    std::string name;
    std::string type;
    int quantity;
    int price;
    
    GameItem(const std::string& n, const std::string& t, int q, int p)
        : name(n), type(t), quantity(q), price(p) {}
};

void demonstrateFindAlgorithms()
{
    std::cout << "\n=== find系アルゴリズムの実例 ===" << std::endl;
    
    // パーティメンバーのリスト
    std::vector<GameCharacter> party = 
    {
        {"勇者アレックス", "勇者", 25, 300, 80},
        {"魔法使いベラ", "魔法使い", 22, 180, 120},
        {"僧侶チャーリー", "僧侶", 20, 220, 40},
        {"盗賊ダナ", "盗賊", 24, 200, 90},
        {"戦士イーサン", "戦士", 26, 350, 85}
    };
    
    // 1. find - 特定のキャラクターを検索
    std::cout << "1. find - キャラクター検索:" << std::endl;
    GameCharacter target("魔法使いベラ", "", 0, 0, 0);
    auto found_char = std::find(party.begin(), party.end(), target);
    
    if (found_char != party.end())
    {
        std::cout << "見つかりました: ";
        found_char->display();
    }
    
    // 2. find_if - 条件に基づく検索
    std::cout << "\n2. find_if - 条件検索（レベル25以上）:" << std::endl;
    auto high_level = std::find_if(party.begin(), party.end(),
        [](const GameCharacter& c) { return c.level >= 25; });
    
    if (high_level != party.end())
    {
        std::cout << "高レベルキャラクター: ";
        high_level->display();
    }
    
    // 3. find_if_not - 条件に合わないものを検索
    std::cout << "\n3. find_if_not - 条件外検索（体力300未満）:" << std::endl;
    auto low_health = std::find_if_not(party.begin(), party.end(),
        [](const GameCharacter& c) { return c.health >= 300; });
    
    if (low_health != party.end())
    {
        std::cout << "体力の低いキャラクター: ";
        low_health->display();
    }
    
    // 4. find_first_of - 複数条件の最初の一致
    std::cout << "\n4. find_first_of - 職業検索:" << std::endl;
    std::vector<std::string> magic_jobs = {"魔法使い", "僧侶"};
    
    auto magic_user = std::find_first_of(party.begin(), party.end(),
        magic_jobs.begin(), magic_jobs.end(),
        [](const GameCharacter& c, const std::string& job) 
        {
            return c.job_class == job;
        });
    
    if (magic_user != party.end())
    {
        std::cout << "最初の魔法系キャラクター: ";
        magic_user->display();
    }
}

void demonstrateCountAlgorithms()
{
    std::cout << "\n=== count系アルゴリズムの実例 ===" << std::endl;
    
    // ゲーム内のアイテム在庫
    std::vector<GameItem> inventory = 
    {
        {"回復ポーション", "回復", 15, 50},
        {"魔法の剣", "武器", 1, 1000},
        {"鉄の盾", "防具", 2, 300},
        {"回復ポーション", "回復", 8, 50},
        {"魔力の杖", "武器", 1, 800},
        {"毒消し", "回復", 10, 30},
        {"回復ポーション", "回復", 5, 50}
    };
    
    // 1. count - 特定アイテムの数
    std::cout << "1. count - 回復ポーションの在庫数:" << std::endl;
    int potion_stacks = std::count_if(inventory.begin(), inventory.end(),
        [](const GameItem& item) { return item.name == "回復ポーション"; });
    std::cout << "回復ポーションの在庫: " << potion_stacks << "スタック" << std::endl;
    
    // 2. count_if - 条件に基づくカウント
    std::cout << "\n2. count_if - 種類別アイテム数:" << std::endl;
    
    int weapon_count = std::count_if(inventory.begin(), inventory.end(),
        [](const GameItem& item) { return item.type == "武器"; });
    std::cout << "武器の種類数: " << weapon_count << std::endl;
    
    int healing_count = std::count_if(inventory.begin(), inventory.end(),
        [](const GameItem& item) { return item.type == "回復"; });
    std::cout << "回復アイテムの種類数: " << healing_count << std::endl;
    
    // 3. 高価なアイテムのカウント
    std::cout << "\n3. 高価なアイテム（500G以上）の数:" << std::endl;
    int expensive_items = std::count_if(inventory.begin(), inventory.end(),
        [](const GameItem& item) { return item.price >= 500; });
    std::cout << "高価なアイテム: " << expensive_items << "個" << std::endl;
}

void demonstrateForEachAlgorithm()
{
    std::cout << "\n=== for_each アルゴリズムの実例 ===" << std::endl;
    
    // プレイヤーの経験値リスト
    std::vector<int> experience_points = {1200, 850, 2100, 675, 1800, 950, 1350};
    
    // 1. 基本的なfor_each使用
    std::cout << "1. 現在の経験値一覧:" << std::endl;
    std::for_each(experience_points.begin(), experience_points.end(),
        [](int exp) { std::cout << "EXP: " << exp << " ポイント" << std::endl; });
    
    // 2. for_eachでデータを変更
    std::cout << "\n2. ボーナス経験値（+100）を追加:" << std::endl;
    std::for_each(experience_points.begin(), experience_points.end(),
        [](int& exp) { exp += 100; });
    
    std::for_each(experience_points.begin(), experience_points.end(),
        [](int exp) { std::cout << "新EXP: " << exp << " ポイント" << std::endl; });
    
    // 3. 統計情報の計算
    std::cout << "\n3. 統計情報の計算:" << std::endl;
    int total_exp = 0;
    int max_exp = 0;
    int min_exp = INT_MAX;
    
    std::for_each(experience_points.begin(), experience_points.end(),
        [&](int exp) 
        {
            total_exp += exp;
            max_exp = std::max(max_exp, exp);
            min_exp = std::min(min_exp, exp);
        });
    
    std::cout << "総経験値: " << total_exp << std::endl;
    std::cout << "最高経験値: " << max_exp << std::endl;
    std::cout << "最低経験値: " << min_exp << std::endl;
    std::cout << "平均経験値: " << (total_exp / experience_points.size()) << std::endl;
}

void demonstrateGameScenarios()
{
    std::cout << "\n=== ゲーム開発での実践的な使用例 ===" << std::endl;
    
    // ゲーム内のエネミーデータ
    std::vector<GameCharacter> enemies = 
    {
        {"スライム", "モンスター", 5, 50, 15},
        {"ゴブリン", "モンスター", 8, 80, 25},
        {"オーク", "モンスター", 12, 150, 40},
        {"ドラゴン", "ボス", 25, 500, 120},
        {"スケルトン", "アンデッド", 10, 90, 30},
        {"リッチ", "ボス", 30, 400, 150}
    };
    
    // 1. ボス敵の検索と表示
    std::cout << "1. ボス敵の検索:" << std::endl;
    std::for_each(enemies.begin(), enemies.end(),
        [](const GameCharacter& enemy) 
        {
            if (enemy.job_class == "ボス")
            {
                std::cout << "ボス発見: ";
                enemy.display();
            }
        });
    
    // 2. 特定レベル以上の敵をカウント
    std::cout << "\n2. 危険な敵（レベル15以上）の数:" << std::endl;
    int dangerous_enemies = std::count_if(enemies.begin(), enemies.end(),
        [](const GameCharacter& enemy) { return enemy.level >= 15; });
    std::cout << "危険な敵: " << dangerous_enemies << "体" << std::endl;
    
    // 3. 最強の敵を検索
    std::cout << "\n3. 最強の敵を検索:" << std::endl;
    auto strongest = std::max_element(enemies.begin(), enemies.end(),
        [](const GameCharacter& a, const GameCharacter& b) 
        {
            return a.attack_power < b.attack_power;
        });
    
    if (strongest != enemies.end())
    {
        std::cout << "最強の敵: ";
        strongest->display();
    }
    
    // 4. 戦闘可能な敵（体力100以下）を検索
    std::cout << "\n4. 戦闘しやすい敵（HP100以下）:" << std::endl;
    auto weak_enemy = std::find_if(enemies.begin(), enemies.end(),
        [](const GameCharacter& enemy) { return enemy.health <= 100; });
    
    while (weak_enemy != enemies.end())
    {
        std::cout << "戦闘推奨: ";
        weak_enemy->display();
        
        // 次の条件に合う敵を検索
        weak_enemy = std::find_if(weak_enemy + 1, enemies.end(),
            [](const GameCharacter& enemy) { return enemy.health <= 100; });
    }
}

int main()
{
    std::cout << "C++アルゴリズムとイテレータ - レクチャー2: アルゴリズムの基礎" << std::endl;
    std::cout << "=========================================================" << std::endl;
    
    // 各アルゴリズムの実演
    demonstrateFindAlgorithms();
    demonstrateCountAlgorithms();
    demonstrateForEachAlgorithm();
    demonstrateGameScenarios();
    
    std::cout << "\n=== まとめ ===" << std::endl;
    std::cout << "1. find系: 条件に合う要素の検索" << std::endl;
    std::cout << "   - find: 値による検索" << std::endl;
    std::cout << "   - find_if: 条件による検索" << std::endl;
    std::cout << "   - find_if_not: 条件に合わない要素の検索" << std::endl;
    std::cout << "2. count系: 条件に合う要素の数をカウント" << std::endl;
    std::cout << "   - count: 値によるカウント" << std::endl;
    std::cout << "   - count_if: 条件によるカウント" << std::endl;
    std::cout << "3. for_each: 全要素に対する処理の実行" << std::endl;
    std::cout << "ゲーム開発では、これらのアルゴリズムでデータ検索や統計処理を効率的に行えます" << std::endl;
    
    return 0;
}