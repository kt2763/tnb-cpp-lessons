/*
 * C++モダンコース - セクション9: アルゴリズムとイテレータ
 * レクチャー3: ソートアルゴリズム（sort, stable_sort, partial_sort）
 * 
 * このレクチャーでは、STLのソートアルゴリズムについて学びます
 * ゲーム開発でのランキング、スコア管理、優先度付けの実装を通じて理解します
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <random>
#include <iomanip>

// プレイヤーのスコアデータ
struct PlayerScore
{
    std::string player_name;
    int score;
    int level;
    std::string rank;
    
    PlayerScore(const std::string& name, int s, int lv, const std::string& r = "")
        : player_name(name), score(s), level(lv), rank(r) {}
    
    void display() const
    {
        std::cout << std::setw(12) << player_name 
                  << " | Score: " << std::setw(6) << score
                  << " | Level: " << std::setw(3) << level
                  << " | Rank: " << rank << std::endl;
    }
};

// ゲームアイテムのデータ
struct GameItem
{
    std::string name;
    std::string category;
    int rarity;
    int value;
    int power;
    
    GameItem(const std::string& n, const std::string& cat, int r, int v, int p)
        : name(n), category(cat), rarity(r), value(v), power(p) {}
    
    void display() const
    {
        std::cout << std::setw(15) << name 
                  << " | " << std::setw(8) << category
                  << " | Rarity: " << rarity
                  << " | Value: " << std::setw(4) << value
                  << " | Power: " << std::setw(3) << power << std::endl;
    }
};

void demonstrateBasicSort()
{
    std::cout << "\n=== 基本的なsortアルゴリズム ===" << std::endl;
    
    // プレイヤーのスコアリスト
    std::vector<PlayerScore> players = 
    {
        {"アリス", 8750, 25, "ゴールド"},
        {"ボブ", 9200, 22, "プラチナ"},
        {"チャーリー", 7800, 28, "シルバー"},
        {"ダイアナ", 9500, 30, "ダイヤモンド"},
        {"イーサン", 8200, 24, "ゴールド"},
        {"フィオナ", 9100, 26, "プラチナ"}
    };
    
    std::cout << "ソート前のスコア一覧:" << std::endl;
    for (const auto& player : players)
    {
        player.display();
    }
    
    // 1. スコアの降順ソート
    std::cout << "\n1. スコア降順ソート:" << std::endl;
    auto score_sorted = players;
    std::sort(score_sorted.begin(), score_sorted.end(),
        [](const PlayerScore& a, const PlayerScore& b) 
        {
            return a.score > b.score;
        });
    
    for (const auto& player : score_sorted)
    {
        player.display();
    }
    
    // 2. レベル昇順ソート
    std::cout << "\n2. レベル昇順ソート:" << std::endl;
    auto level_sorted = players;
    std::sort(level_sorted.begin(), level_sorted.end(),
        [](const PlayerScore& a, const PlayerScore& b) 
        {
            return a.level < b.level;
        });
    
    for (const auto& player : level_sorted)
    {
        player.display();
    }
}

void demonstrateStableSort()
{
    std::cout << "\n=== stable_sort（安定ソート）の実例 ===" << std::endl;
    
    // 同じランクのプレイヤーがいる場合
    std::vector<PlayerScore> players = 
    {
        {"アレックス", 8500, 25, "ゴールド"},
        {"ベラ", 8700, 23, "ゴールド"},
        {"チャーリー", 9200, 28, "プラチナ"},
        {"ダナ", 8600, 24, "ゴールド"},
        {"イーサン", 9100, 27, "プラチナ"},
        {"フィオナ", 8550, 26, "ゴールド"}
    };
    
    std::cout << "元の順序:" << std::endl;
    for (size_t i = 0; i < players.size(); ++i)
    {
        std::cout << "[" << i << "] ";
        players[i].display();
    }
    
    // stable_sortでランク別ソート（元の順序を保持）
    std::cout << "\nstable_sort - ランク別ソート（同ランク内は元の順序を保持）:" << std::endl;
    auto stable_sorted = players;
    std::stable_sort(stable_sorted.begin(), stable_sorted.end(),
        [](const PlayerScore& a, const PlayerScore& b) 
        {
            // ランクの優先度: ダイヤモンド > プラチナ > ゴールド > シルバー
            std::vector<std::string> rank_order = {"ダイヤモンド", "プラチナ", "ゴールド", "シルバー"};
            
            auto rank_a = std::find(rank_order.begin(), rank_order.end(), a.rank);
            auto rank_b = std::find(rank_order.begin(), rank_order.end(), b.rank);
            
            return rank_a < rank_b;
        });
    
    for (size_t i = 0; i < stable_sorted.size(); ++i)
    {
        std::cout << "[" << i << "] ";
        stable_sorted[i].display();
    }
    
    // 通常のsortとの比較
    std::cout << "\n通常のsort - 同じ条件（同ランク内の順序は不定）:" << std::endl;
    std::sort(players.begin(), players.end(),
        [](const PlayerScore& a, const PlayerScore& b) 
        {
            std::vector<std::string> rank_order = {"ダイヤモンド", "プラチナ", "ゴールド", "シルバー"};
            
            auto rank_a = std::find(rank_order.begin(), rank_order.end(), a.rank);
            auto rank_b = std::find(rank_order.begin(), rank_order.end(), b.rank);
            
            return rank_a < rank_b;
        });
    
    for (size_t i = 0; i < players.size(); ++i)
    {
        std::cout << "[" << i << "] ";
        players[i].display();
    }
}

void demonstratePartialSort()
{
    std::cout << "\n=== partial_sort（部分ソート）の実例 ===" << std::endl;
    
    // 大量のゲームアイテムデータ
    std::vector<GameItem> items = 
    {
        {"鉄の剣", "武器", 2, 150, 25},
        {"魔法の杖", "武器", 4, 800, 45},
        {"回復ポーション", "回復", 1, 50, 0},
        {"ドラゴンの鱗", "素材", 5, 1200, 0},
        {"革の鎧", "防具", 2, 200, 0},
        {"ミスリルの盾", "防具", 4, 900, 0},
        {"毒消し", "回復", 1, 30, 0},
        {"炎の指輪", "装飾", 3, 600, 20},
        {"氷の結晶", "素材", 3, 400, 0},
        {"聖なる剣", "武器", 5, 2000, 80},
        {"魔力の水晶", "素材", 4, 750, 0},
        {"エリクサー", "回復", 5, 1500, 0}
    };
    
    std::cout << "全アイテム一覧:" << std::endl;
    for (const auto& item : items)
    {
        item.display();
    }
    
    // 1. 最も価値の高い上位5アイテムのみをソート
    std::cout << "\n1. partial_sort - 価値上位5位まで:" << std::endl;
    auto value_sorted = items;
    std::partial_sort(value_sorted.begin(), value_sorted.begin() + 5, value_sorted.end(),
        [](const GameItem& a, const GameItem& b) 
        {
            return a.value > b.value;
        });
    
    std::cout << "上位5位のアイテム:" << std::endl;
    for (size_t i = 0; i < 5; ++i)
    {
        std::cout << "[" << (i+1) << "] ";
        value_sorted[i].display();
    }
    
    // 2. レア度上位3位まで
    std::cout << "\n2. partial_sort - レア度上位3位まで:" << std::endl;
    auto rarity_sorted = items;
    std::partial_sort(rarity_sorted.begin(), rarity_sorted.begin() + 3, rarity_sorted.end(),
        [](const GameItem& a, const GameItem& b) 
        {
            return a.rarity > b.rarity;
        });
    
    std::cout << "レア度上位3位のアイテム:" << std::endl;
    for (size_t i = 0; i < 3; ++i)
    {
        std::cout << "[" << (i+1) << "] ";
        rarity_sorted[i].display();
    }
}

void demonstrateAdvancedSorting()
{
    std::cout << "\n=== 高度なソート技術 ===" << std::endl;
    
    std::vector<PlayerScore> tournament_players = 
    {
        {"マスター剣士", 9800, 35, "ダイヤモンド"},
        {"炎の魔導師", 9500, 32, "プラチナ"},
        {"影の暗殺者", 9750, 30, "ダイヤモンド"},
        {"聖なる僧侶", 9200, 28, "プラチナ"},
        {"鋼の戦士", 9600, 33, "ダイヤモンド"},
        {"風の射手", 9100, 29, "プラチナ"},
        {"大地の守護者", 9850, 36, "ダイヤモンド"},
        {"氷の魔術師", 9300, 31, "プラチナ"}
    };
    
    // 1. 複数条件でのソート（ランク優先、同ランクならスコア順）
    std::cout << "1. 複数条件ソート（ランク優先、同ランクならスコア降順）:" << std::endl;
    
    std::sort(tournament_players.begin(), tournament_players.end(),
        [](const PlayerScore& a, const PlayerScore& b) 
        {
            // まずランクで比較
            if (a.rank != b.rank)
            {
                if (a.rank == "ダイヤモンド") return true;
                if (b.rank == "ダイヤモンド") return false;
                if (a.rank == "プラチナ") return true;
                if (b.rank == "プラチナ") return false;
                return a.rank < b.rank;
            }
            // 同じランクならスコアで比較
            return a.score > b.score;
        });
    
    for (const auto& player : tournament_players)
    {
        player.display();
    }
    
    // 2. nth_element - 中央値の検索
    std::cout << "\n2. nth_element - 中央値のプレイヤー:" << std::endl;
    std::vector<int> all_scores;
    for (const auto& player : tournament_players)
    {
        all_scores.push_back(player.score);
    }
    
    size_t middle = all_scores.size() / 2;
    std::nth_element(all_scores.begin(), all_scores.begin() + middle, all_scores.end());
    
    std::cout << "中央値のスコア: " << all_scores[middle] << std::endl;
    
    // 3. is_sorted - ソート済みかどうかの確認
    std::cout << "\n3. is_sorted - ソート状態の確認:" << std::endl;
    bool is_score_sorted = std::is_sorted(tournament_players.begin(), tournament_players.end(),
        [](const PlayerScore& a, const PlayerScore& b) 
        {
            return a.score > b.score;
        });
    
    std::cout << "スコア降順でソート済み: " << (is_score_sorted ? "はい" : "いいえ") << std::endl;
}

void demonstrateGameRanking()
{
    std::cout << "\n=== ゲームランキングシステムの実装 ===" << std::endl;
    
    // ランダムなスコアデータを生成
    std::vector<PlayerScore> online_players;
    std::vector<std::string> names = 
    {
        "DragonSlayer", "MagicMaster", "ShadowNinja", "HolyPriest", "IronWarrior",
        "FireMage", "IceQueen", "ThunderGod", "EarthShaker", "WindDancer",
        "DarkLord", "LightBringer", "SwordSaint", "ArcanePower", "DivineBlade"
    };
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> score_dist(5000, 10000);
    std::uniform_int_distribution<> level_dist(20, 40);
    
    for (const auto& name : names)
    {
        online_players.emplace_back(name, score_dist(gen), level_dist(gen), "オンライン");
    }
    
    // トップ10ランキングの作成
    std::cout << "=== トップ10ランキング ===" << std::endl;
    std::partial_sort(online_players.begin(), online_players.begin() + 10, online_players.end(),
        [](const PlayerScore& a, const PlayerScore& b) 
        {
            return a.score > b.score;
        });
    
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << "第" << std::setw(2) << (i+1) << "位: ";
        online_players[i].display();
    }
    
    // レベル帯別のランキング
    std::cout << "\n=== レベル30以上のトップ5 ===" << std::endl;
    std::vector<PlayerScore> high_level_players;
    std::copy_if(online_players.begin(), online_players.end(), 
                 std::back_inserter(high_level_players),
                 [](const PlayerScore& p) { return p.level >= 30; });
    
    std::sort(high_level_players.begin(), high_level_players.end(),
        [](const PlayerScore& a, const PlayerScore& b) 
        {
            return a.score > b.score;
        });
    
    size_t show_count = std::min(high_level_players.size(), size_t(5));
    for (size_t i = 0; i < show_count; ++i)
    {
        std::cout << "第" << std::setw(2) << (i+1) << "位: ";
        high_level_players[i].display();
    }
}

int main()
{
    std::cout << "C++アルゴリズムとイテレータ - レクチャー3: ソートアルゴリズム" << std::endl;
    std::cout << "=======================================================" << std::endl;
    
    // 各ソートアルゴリズムの実演
    demonstrateBasicSort();
    demonstrateStableSort();
    demonstratePartialSort();
    demonstrateAdvancedSorting();
    demonstrateGameRanking();
    
    std::cout << "\n=== まとめ ===" << std::endl;
    std::cout << "1. sort: 高速な不安定ソート" << std::endl;
    std::cout << "2. stable_sort: 安定ソート（等価要素の順序を保持）" << std::endl;
    std::cout << "3. partial_sort: 部分的なソート（上位N件のみ）" << std::endl;
    std::cout << "4. nth_element: N番目の要素を正しい位置に配置" << std::endl;
    std::cout << "5. is_sorted: ソート済みかどうかの確認" << std::endl;
    std::cout << "ゲーム開発では、ランキング表示や優先度付けに活用できます" << std::endl;
    
    return 0;
}