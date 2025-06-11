/*
 * C++モダンコース - セクション9: アルゴリズムとイテレータ
 * レクチャー8: 数値アルゴリズム（accumulate, inner_product, partial_sum）
 * 
 * このレクチャーでは、数値計算に特化したアルゴリズムを学びます
 * ゲーム開発での統計計算、スコア集計、物理演算の基礎を実践します
 */

#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <string>
#include <iomanip>
#include <functional>

// プレイヤーのステータスデータ
struct PlayerStats
{
    std::string name;
    int level;
    int health;
    int mana;
    int attack;
    int defense;
    int experience;
    
    PlayerStats(const std::string& n, int lv, int hp, int mp, int atk, int def, int exp)
        : name(n), level(lv), health(hp), mana(mp), attack(atk), defense(def), experience(exp) {}
    
    void display() const
    {
        std::cout << std::setw(10) << name 
                  << " | Lv:" << std::setw(2) << level
                  << " | HP:" << std::setw(3) << health
                  << " | MP:" << std::setw(3) << mana
                  << " | ATK:" << std::setw(3) << attack
                  << " | DEF:" << std::setw(3) << defense
                  << " | EXP:" << std::setw(5) << experience << std::endl;
    }
    
    int total_stats() const
    {
        return health + mana + attack + defense;
    }
};

// ゲーム内アイテムの価格データ
struct GameItem
{
    std::string name;
    int price;
    int quantity;
    double weight;
    
    GameItem(const std::string& n, int p, int q, double w)
        : name(n), price(p), quantity(q), weight(w) {}
    
    int total_value() const { return price * quantity; }
    double total_weight() const { return weight * quantity; }
};

void demonstrateAccumulate()
{
    std::cout << "\n=== accumulate の実例 ===" << std::endl;
    
    // プレイヤーの経験値データ
    std::vector<int> daily_experience = {1200, 850, 2100, 675, 1800, 950, 1350, 2400, 780, 1650};
    
    std::cout << "日別獲得経験値: ";
    for (int exp : daily_experience)
    {
        std::cout << exp << " ";
    }
    std::cout << std::endl;
    
    // 1. 基本的な合計計算
    int total_exp = std::accumulate(daily_experience.begin(), daily_experience.end(), 0);
    std::cout << "総獲得経験値: " << total_exp << std::endl;
    std::cout << "平均獲得経験値: " << (total_exp / daily_experience.size()) << std::endl;
    
    // 2. カスタム演算での使用（最大値の検索）
    int max_daily_exp = std::accumulate(daily_experience.begin(), daily_experience.end(), 0,
        [](int current_max, int daily_exp) 
        {
            return std::max(current_max, daily_exp);
        });
    std::cout << "最高日別経験値: " << max_daily_exp << std::endl;
    
    // 3. 文字列の連結
    std::vector<std::string> player_names = {"勇者", "魔法使い", "僧侶", "盗賊"};
    std::string party_list = std::accumulate(player_names.begin(), player_names.end(), std::string("パーティ: "),
        [](const std::string& result, const std::string& name) 
        {
            return result + name + " ";
        });
    std::cout << party_list << std::endl;
    
    // 4. 複雑なオブジェクトでの合計
    std::vector<PlayerStats> party = 
    {
        {"勇者", 25, 300, 100, 80, 70, 15000},
        {"魔法使い", 22, 180, 250, 120, 40, 12000},
        {"僧侶", 20, 220, 200, 50, 60, 10000},
        {"盗賊", 24, 200, 80, 90, 50, 13000}
    };
    
    std::cout << "\nパーティメンバー:" << std::endl;
    for (const auto& member : party)
    {
        member.display();
    }
    
    int total_party_level = std::accumulate(party.begin(), party.end(), 0,
        [](int sum, const PlayerStats& member) 
        {
            return sum + member.level;
        });
    std::cout << "パーティ総レベル: " << total_party_level << std::endl;
    
    int total_party_power = std::accumulate(party.begin(), party.end(), 0,
        [](int sum, const PlayerStats& member) 
        {
            return sum + member.total_stats();
        });
    std::cout << "パーティ総合力: " << total_party_power << std::endl;
}

void demonstrateInnerProduct()
{
    std::cout << "\n=== inner_product の実例 ===" << std::endl;
    
    // ダメージ計算システム
    std::vector<int> attack_values = {80, 90, 75, 85, 95}; // 攻撃力
    std::vector<double> multipliers = {1.2, 1.5, 0.8, 1.0, 1.3}; // 属性倍率
    
    std::cout << "攻撃力: ";
    for (int atk : attack_values)
    {
        std::cout << std::setw(3) << atk << " ";
    }
    std::cout << std::endl;
    
    std::cout << "属性倍率: ";
    for (double mult : multipliers)
    {
        std::cout << std::setw(5) << std::fixed << std::setprecision(1) << mult << " ";
    }
    std::cout << std::endl;
    
    // 1. 基本的な内積計算（ダメージ合計）
    double total_damage = std::inner_product(attack_values.begin(), attack_values.end(),
                                           multipliers.begin(), 0.0);
    std::cout << "総ダメージ: " << std::fixed << std::setprecision(1) << total_damage << std::endl;
    
    // 2. カスタム演算での内積
    // 最大ダメージの計算
    double max_damage = std::inner_product(attack_values.begin(), attack_values.end(),
                                         multipliers.begin(), 0.0,
                                         [](double max_val, double current) 
                                         {
                                             return std::max(max_val, current);
                                         },
                                         [](int atk, double mult) 
                                         {
                                             return atk * mult;
                                         });
    std::cout << "最大単発ダメージ: " << std::fixed << std::setprecision(1) << max_damage << std::endl;
    
    // 3. アイテムの価値計算
    std::vector<GameItem> inventory = 
    {
        {"回復ポーション", 50, 15, 0.5},
        {"魔法の剣", 1000, 1, 2.0},
        {"鉄の盾", 300, 2, 5.0},
        {"魔力の指輪", 800, 1, 0.1}
    };
    
    std::cout << "\nインベントリ価値計算:" << std::endl;
    for (const auto& item : inventory)
    {
        std::cout << std::setw(12) << item.name 
                  << " | 単価:" << std::setw(4) << item.price 
                  << " | 数量:" << std::setw(2) << item.quantity
                  << " | 重量:" << std::setw(4) << std::fixed << std::setprecision(1) << item.weight << std::endl;
    }
    
    // 価格と数量のベクトルを作成
    std::vector<int> prices, quantities;
    std::vector<double> weights;
    
    for (const auto& item : inventory)
    {
        prices.push_back(item.price);
        quantities.push_back(item.quantity);
        weights.push_back(item.weight);
    }
    
    int total_value = std::inner_product(prices.begin(), prices.end(), quantities.begin(), 0);
    std::cout << "インベントリ総価値: " << total_value << "ゴールド" << std::endl;
    
    double total_weight = std::inner_product(weights.begin(), weights.end(), quantities.begin(), 0.0);
    std::cout << "インベントリ総重量: " << std::fixed << std::setprecision(1) << total_weight << "kg" << std::endl;
}

void demonstratePartialSum()
{
    std::cout << "\n=== partial_sum の実例 ===" << std::endl;
    
    // レベルアップに必要な経験値
    std::vector<int> level_exp_required = {100, 200, 350, 500, 750, 1000, 1500, 2000, 3000, 4000};
    
    std::cout << "各レベルで必要な経験値: ";
    for (int exp : level_exp_required)
    {
        std::cout << std::setw(4) << exp << " ";
    }
    std::cout << std::endl;
    
    // 1. 累積経験値の計算
    std::vector<int> cumulative_exp(level_exp_required.size());
    std::partial_sum(level_exp_required.begin(), level_exp_required.end(), cumulative_exp.begin());
    
    std::cout << "累積必要経験値:     ";
    for (int exp : cumulative_exp)
    {
        std::cout << std::setw(4) << exp << " ";
    }
    std::cout << std::endl;
    
    // 現在の経験値でレベル判定
    int current_exp = 3500;
    auto level_it = std::lower_bound(cumulative_exp.begin(), cumulative_exp.end(), current_exp);
    int current_level = level_it - cumulative_exp.begin() + 1;
    
    std::cout << "\n現在の経験値: " << current_exp << std::endl;
    std::cout << "現在のレベル: " << current_level << std::endl;
    
    if (level_it != cumulative_exp.end())
    {
        int next_level_exp = *level_it;
        std::cout << "次のレベルまで: " << (next_level_exp - current_exp) << " EXP" << std::endl;
    }
    
    // 2. ゲーム内通貨の収支管理
    std::vector<int> daily_gold_changes = {+150, -80, +200, -120, +300, -50, +180, -90, +250, -40};
    
    std::cout << "\n日別ゴールド収支: ";
    for (int change : daily_gold_changes)
    {
        std::cout << std::setw(4) << (change >= 0 ? "+" : "") << change << " ";
    }
    std::cout << std::endl;
    
    std::vector<int> gold_balance(daily_gold_changes.size());
    int initial_gold = 1000;
    
    // 初期金額から累積計算
    std::partial_sum(daily_gold_changes.begin(), daily_gold_changes.end(), gold_balance.begin(),
        [initial_gold](int sum, int change) mutable 
        {
            static bool first_call = true;
            if (first_call)
            {
                first_call = false;
                return initial_gold + change;
            }
            return sum + change;
        });
    
    // 手動で正しく計算
    gold_balance[0] = initial_gold + daily_gold_changes[0];
    for (size_t i = 1; i < daily_gold_changes.size(); ++i)
    {
        gold_balance[i] = gold_balance[i-1] + daily_gold_changes[i];
    }
    
    std::cout << "日別ゴールド残高: ";
    for (int balance : gold_balance)
    {
        std::cout << std::setw(4) << balance << " ";
    }
    std::cout << std::endl;
    
    // 3. スキル習得の進行状況
    std::vector<int> skill_training_hours = {2, 3, 1, 4, 2, 3, 5, 2, 4, 3};
    std::cout << "\n日別訓練時間: ";
    for (int hours : skill_training_hours)
    {
        std::cout << std::setw(2) << hours << " ";
    }
    std::cout << std::endl;
    
    std::vector<int> total_training_hours(skill_training_hours.size());
    std::partial_sum(skill_training_hours.begin(), skill_training_hours.end(), total_training_hours.begin());
    
    std::cout << "累積訓練時間:   ";
    for (int total : total_training_hours)
    {
        std::cout << std::setw(2) << total << " ";
    }
    std::cout << std::endl;
    
    // スキルレベル判定（10時間ごとにレベルアップ）
    std::cout << "スキルレベル:   ";
    for (int total : total_training_hours)
    {
        std::cout << std::setw(2) << (total / 10 + 1) << " ";
    }
    std::cout << std::endl;
}

void demonstrateAdjacentDifference()
{
    std::cout << "\n=== adjacent_difference の実例 ===" << std::endl;
    
    // プレイヤーのスコア推移
    std::vector<int> score_history = {1000, 1150, 1080, 1200, 1350, 1280, 1450, 1520, 1480, 1600};
    
    std::cout << "スコア履歴: ";
    for (int score : score_history)
    {
        std::cout << std::setw(4) << score << " ";
    }
    std::cout << std::endl;
    
    // 1. スコアの変化量を計算
    std::vector<int> score_changes(score_history.size());
    std::adjacent_difference(score_history.begin(), score_history.end(), score_changes.begin());
    
    std::cout << "スコア変化: ";
    for (size_t i = 0; i < score_changes.size(); ++i)
    {
        if (i == 0)
        {
            std::cout << std::setw(4) << "---" << " ";
        }
        else
        {
            std::cout << std::setw(4) << (score_changes[i] >= 0 ? "+" : "") << score_changes[i] << " ";
        }
    }
    std::cout << std::endl;
    
    // 2. 成長率の分析
    int positive_changes = 0;
    int negative_changes = 0;
    int max_gain = 0;
    int max_loss = 0;
    
    for (size_t i = 1; i < score_changes.size(); ++i)
    {
        if (score_changes[i] > 0)
        {
            positive_changes++;
            max_gain = std::max(max_gain, score_changes[i]);
        }
        else if (score_changes[i] < 0)
        {
            negative_changes++;
            max_loss = std::min(max_loss, score_changes[i]);
        }
    }
    
    std::cout << "\nスコア分析:" << std::endl;
    std::cout << "上昇回数: " << positive_changes << "回" << std::endl;
    std::cout << "下降回数: " << negative_changes << "回" << std::endl;
    std::cout << "最大上昇: " << max_gain << "ポイント" << std::endl;
    std::cout << "最大下降: " << max_loss << "ポイント" << std::endl;
    
    // 3. 物理エンジンでの速度計算
    std::vector<double> position_data = {0.0, 1.5, 4.2, 7.8, 12.5, 18.0, 24.2, 31.1, 38.7, 47.0};
    
    std::cout << "\n位置データ: ";
    for (double pos : position_data)
    {
        std::cout << std::setw(5) << std::fixed << std::setprecision(1) << pos << " ";
    }
    std::cout << std::endl;
    
    std::vector<double> velocity_data(position_data.size());
    std::adjacent_difference(position_data.begin(), position_data.end(), velocity_data.begin());
    
    std::cout << "速度データ: ";
    for (size_t i = 0; i < velocity_data.size(); ++i)
    {
        if (i == 0)
        {
            std::cout << std::setw(5) << "---" << " ";
        }
        else
        {
            std::cout << std::setw(5) << std::fixed << std::setprecision(1) << velocity_data[i] << " ";
        }
    }
    std::cout << std::endl;
}

void demonstrateGameStatistics()
{
    std::cout << "\n=== ゲーム統計システムの実装 ===" << std::endl;
    
    // 複数のプレイヤーの戦績データ
    struct BattleRecord
    {
        std::string enemy_name;
        int damage_dealt;
        int damage_taken;
        int exp_gained;
        int gold_gained;
        
        BattleRecord(const std::string& name, int dealt, int taken, int exp, int gold)
            : enemy_name(name), damage_dealt(dealt), damage_taken(taken), exp_gained(exp), gold_gained(gold) {}
    };
    
    std::vector<BattleRecord> battle_history = 
    {
        {"スライム", 45, 12, 80, 25},
        {"ゴブリン", 62, 28, 120, 40},
        {"オーク", 89, 45, 200, 75},
        {"ドラゴン", 156, 89, 500, 200},
        {"スケルトン", 71, 35, 150, 60},
        {"トロル", 134, 67, 350, 120}
    };
    
    std::cout << "戦闘履歴:" << std::endl;
    std::cout << std::setw(12) << "敵名" << " | " << std::setw(6) << "与ダメ" << " | " 
              << std::setw(6) << "被ダメ" << " | " << std::setw(6) << "獲得EXP" << " | " 
              << std::setw(6) << "獲得金" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    
    for (const auto& record : battle_history)
    {
        std::cout << std::setw(12) << record.enemy_name << " | "
                  << std::setw(6) << record.damage_dealt << " | "
                  << std::setw(6) << record.damage_taken << " | "
                  << std::setw(6) << record.exp_gained << " | "
                  << std::setw(6) << record.gold_gained << std::endl;
    }
    
    // 統計計算
    int total_damage_dealt = std::accumulate(battle_history.begin(), battle_history.end(), 0,
        [](int sum, const BattleRecord& record) 
        {
            return sum + record.damage_dealt;
        });
    
    int total_damage_taken = std::accumulate(battle_history.begin(), battle_history.end(), 0,
        [](int sum, const BattleRecord& record) 
        {
            return sum + record.damage_taken;
        });
    
    int total_exp = std::accumulate(battle_history.begin(), battle_history.end(), 0,
        [](int sum, const BattleRecord& record) 
        {
            return sum + record.exp_gained;
        });
    
    int total_gold = std::accumulate(battle_history.begin(), battle_history.end(), 0,
        [](int sum, const BattleRecord& record) 
        {
            return sum + record.gold_gained;
        });
    
    std::cout << "\n戦闘統計:" << std::endl;
    std::cout << "総与ダメージ: " << total_damage_dealt << std::endl;
    std::cout << "総被ダメージ: " << total_damage_taken << std::endl;
    std::cout << "ダメージ効率: " << std::fixed << std::setprecision(2) 
              << (static_cast<double>(total_damage_dealt) / total_damage_taken) << std::endl;
    std::cout << "総獲得経験値: " << total_exp << std::endl;
    std::cout << "総獲得ゴールド: " << total_gold << std::endl;
    std::cout << "平均経験値/戦闘: " << (total_exp / battle_history.size()) << std::endl;
    std::cout << "平均ゴールド/戦闘: " << (total_gold / battle_history.size()) << std::endl;
    
    // 累積経験値の推移
    std::vector<int> exp_progression(battle_history.size());
    std::partial_sum(battle_history.begin(), battle_history.end(), exp_progression.begin(),
        [](int sum, const BattleRecord& record) 
        {
            return sum + record.exp_gained;
        });
    
    std::cout << "\n経験値の累積推移:" << std::endl;
    for (size_t i = 0; i < battle_history.size(); ++i)
    {
        std::cout << "戦闘" << (i+1) << "後: " << std::setw(4) << exp_progression[i] << " EXP" << std::endl;
    }
}

int main()
{
    std::cout << "C++アルゴリズムとイテレータ - レクチャー8: 数値アルゴリズム" << std::endl;
    std::cout << "=========================================================" << std::endl;
    
    // 各数値アルゴリズムの実演
    demonstrateAccumulate();
    demonstrateInnerProduct();
    demonstratePartialSum();
    demonstrateAdjacentDifference();
    demonstrateGameStatistics();
    
    std::cout << "\n=== まとめ ===" << std::endl;
    std::cout << "1. accumulate: 要素の累積（合計、最大値、文字列連結など）" << std::endl;
    std::cout << "2. inner_product: 内積計算（重み付き合計）" << std::endl;
    std::cout << "3. partial_sum: 部分和（累積合計の配列生成）" << std::endl;
    std::cout << "4. adjacent_difference: 隣接要素の差分計算" << std::endl;
    std::cout << "5. iota: 連続した値での初期化（C++11）" << std::endl;
    std::cout << "ゲーム開発では、統計処理、スコア計算、物理演算で重要な役割" << std::endl;
    std::cout << "<numeric>ヘッダに定義されており、効率的な数値処理が可能" << std::endl;
    
    return 0;
}