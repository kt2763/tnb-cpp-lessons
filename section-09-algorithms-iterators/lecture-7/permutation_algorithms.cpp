/*
 * C++モダンコース - セクション9: アルゴリズムとイテレータ
 * レクチャー7: 順列アルゴリズム（next_permutation, prev_permutation）
 * 
 * このレクチャーでは、順列を生成・操作するアルゴリズムを学びます
 * ゲーム開発でのパズル生成、組み合わせ探索、戦略の最適化を実践します
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <iomanip>
#include <set>

// ゲームカードのデータ構造
struct GameCard
{
    int value;
    std::string suit;
    std::string name;
    
    GameCard(int v, const std::string& s, const std::string& n)
        : value(v), suit(s), name(n) {}
    
    void display() const
    {
        std::cout << "[" << suit << " " << name << "(" << value << ")]";
    }
    
    bool operator<(const GameCard& other) const
    {
        if (suit != other.suit)
            return suit < other.suit;
        return value < other.value;
    }
    
    bool operator==(const GameCard& other) const
    {
        return value == other.value && suit == other.suit;
    }
};

// プレイヤーキャラクターのデータ
struct Character
{
    std::string name;
    char symbol;
    int power;
    
    Character(const std::string& n, char s, int p)
        : name(n), symbol(s), power(p) {}
    
    void display() const
    {
        std::cout << symbol << "(" << name << ":" << power << ")";
    }
    
    bool operator<(const Character& other) const
    {
        return symbol < other.symbol;
    }
};

void demonstrateBasicPermutations()
{
    std::cout << "\n=== 基本的な順列生成 ===" << std::endl;
    
    // 簡単な文字列での順列
    std::string game_elements = "ABC";
    std::cout << "ゲーム要素: " << game_elements << std::endl;
    std::cout << "すべての組み合わせ（順列）:" << std::endl;
    
    int count = 0;
    do 
    {
        std::cout << ++count << ". " << game_elements << std::endl;
    } while (std::next_permutation(game_elements.begin(), game_elements.end()));
    
    std::cout << "総組み合わせ数: " << count << std::endl;
}

void demonstrateNextPermutation()
{
    std::cout << "\n=== next_permutation の実例 ===" << std::endl;
    
    // ダンジョンの部屋配置
    std::vector<char> room_types = {'S', 'T', 'B', 'E'}; // Start, Treasure, Boss, Exit
    std::vector<std::string> room_names = {"スタート", "宝物庫", "ボス部屋", "出口"};
    
    std::cout << "ダンジョンの部屋タイプ: ";
    for (size_t i = 0; i < room_types.size(); ++i)
    {
        std::cout << room_types[i] << "(" << room_names[i] << ") ";
    }
    std::cout << std::endl;
    
    std::cout << "\n可能なダンジョン配置（最初の10パターン）:" << std::endl;
    
    // 初期状態をソートして辞書順最小から開始
    std::sort(room_types.begin(), room_types.end());
    
    int layout_count = 0;
    do 
    {
        std::cout << ++layout_count << ". ";
        for (char room : room_types)
        {
            std::cout << room << " ";
        }
        
        // 妥当性チェック（スタートが最初、出口が最後）
        if (room_types[0] == 'S' && room_types[3] == 'E')
        {
            std::cout << " <- 有効な配置";
        }
        std::cout << std::endl;
        
        if (layout_count >= 10) break; // 最初の10個まで表示
        
    } while (std::next_permutation(room_types.begin(), room_types.end()));
}

void demonstratePrevPermutation()
{
    std::cout << "\n=== prev_permutation の実例 ===" << std::endl;
    
    // ゲームの難易度設定
    std::vector<int> difficulty_levels = {1, 2, 3, 4}; // Easy, Normal, Hard, Expert
    std::vector<std::string> difficulty_names = {"イージー", "ノーマル", "ハード", "エキスパート"};
    
    // 最大の順列から開始（降順にソート）
    std::sort(difficulty_levels.rbegin(), difficulty_levels.rend());
    
    std::cout << "難易度の順列（降順から開始）:" << std::endl;
    std::cout << "開始状態: ";
    for (size_t i = 0; i < difficulty_levels.size(); ++i)
    {
        std::cout << difficulty_names[difficulty_levels[i]-1] << " ";
    }
    std::cout << std::endl;
    
    std::cout << "\n前の順列を生成（最初の8パターン）:" << std::endl;
    
    int pattern_count = 0;
    do 
    {
        std::cout << ++pattern_count << ". ";
        for (size_t i = 0; i < difficulty_levels.size(); ++i)
        {
            std::cout << difficulty_names[difficulty_levels[i]-1] << " ";
        }
        std::cout << std::endl;
        
        if (pattern_count >= 8) break;
        
    } while (std::prev_permutation(difficulty_levels.begin(), difficulty_levels.end()));
}

void demonstrateCardGameCombinations()
{
    std::cout << "\n=== カードゲームでの組み合わせ生成 ===" << std::endl;
    
    // トランプカードの一部を使用
    std::vector<GameCard> hand = 
    {
        {1, "♠", "A"},
        {11, "♠", "J"},
        {12, "♠", "Q"},
        {13, "♠", "K"}
    };
    
    std::cout << "手札: ";
    for (const auto& card : hand)
    {
        card.display();
        std::cout << " ";
    }
    std::cout << std::endl;
    
    // すべての手札の並び順を生成
    std::cout << "\n可能な手札の並び（ポーカーの戦略分析）:" << std::endl;
    
    int combination_count = 0;
    do 
    {
        std::cout << ++combination_count << ". ";
        for (const auto& card : hand)
        {
            card.display();
            std::cout << " ";
        }
        
        // 特定の組み合わせをチェック（例：ストレート）
        bool is_straight = true;
        for (size_t i = 1; i < hand.size(); ++i)
        {
            if (hand[i].value != hand[i-1].value + 1)
            {
                is_straight = false;
                break;
            }
        }
        
        if (is_straight)
        {
            std::cout << " <- ストレート！";
        }
        
        std::cout << std::endl;
        
    } while (std::next_permutation(hand.begin(), hand.end()));
    
    std::cout << "総組み合わせ数: " << combination_count << std::endl;
}

void demonstratePartyFormation()
{
    std::cout << "\n=== パーティ編成の最適化 ===" << std::endl;
    
    // パーティメンバーの候補
    std::vector<Character> party_candidates = 
    {
        {"勇者", 'H', 80},
        {"魔法使い", 'M', 90},
        {"僧侶", 'P', 70},
        {"盗賊", 'T', 75}
    };
    
    std::cout << "パーティ候補メンバー:" << std::endl;
    for (const auto& character : party_candidates)
    {
        character.display();
        std::cout << " ";
    }
    std::cout << std::endl;
    
    // 最適なパーティ編成を探索
    std::cout << "\nパーティ編成パターンの分析:" << std::endl;
    
    int formation_count = 0;
    int best_power = 0;
    std::vector<Character> best_formation;
    
    do 
    {
        formation_count++;
        
        // 現在の編成の総合力を計算
        int total_power = 0;
        for (const auto& character : party_candidates)
        {
            total_power += character.power;
        }
        
        // バランス評価（前衛・後衛のバランス）
        bool has_tank = false;
        bool has_healer = false;
        bool has_dps = false;
        
        for (const auto& character : party_candidates)
        {
            if (character.symbol == 'H') has_tank = true;
            if (character.symbol == 'P') has_healer = true;
            if (character.symbol == 'M' || character.symbol == 'T') has_dps = true;
        }
        
        bool balanced = has_tank && has_healer && has_dps;
        
        std::cout << formation_count << ". ";
        for (const auto& character : party_candidates)
        {
            character.display();
            std::cout << " ";
        }
        std::cout << "| 総合力:" << total_power 
                  << " | バランス:" << (balanced ? "良好" : "不良");
        
        if (balanced && total_power > best_power)
        {
            best_power = total_power;
            best_formation = party_candidates;
            std::cout << " <- 新記録！";
        }
        
        std::cout << std::endl;
        
    } while (std::next_permutation(party_candidates.begin(), party_candidates.end()));
    
    std::cout << "\n最適編成: ";
    for (const auto& character : best_formation)
    {
        character.display();
        std::cout << " ";
    }
    std::cout << "| 総合力: " << best_power << std::endl;
}

void demonstratePuzzleGeneration()
{
    std::cout << "\n=== パズルゲーム生成システム ===" << std::endl;
    
    // 数字パズルの基本配置
    std::vector<int> puzzle_pieces = {1, 2, 3, 4, 5, 6, 7, 8, 0}; // 0は空白
    
    std::cout << "8パズルの初期状態生成:" << std::endl;
    
    // ランダムな有効な初期状態を生成
    std::random_shuffle(puzzle_pieces.begin(), puzzle_pieces.end());
    
    // パズルの表示関数
    auto display_puzzle = [](const std::vector<int>& puzzle) 
    {
        for (size_t i = 0; i < puzzle.size(); ++i)
        {
            if (i % 3 == 0) std::cout << std::endl;
            
            if (puzzle[i] == 0)
                std::cout << "  ";
            else
                std::cout << puzzle[i] << " ";
        }
        std::cout << std::endl;
    };
    
    std::cout << "生成されたパズル状態:";
    display_puzzle(puzzle_pieces);
    
    // 解けるパズルかどうかの簡易チェック
    int inversion_count = 0;
    for (size_t i = 0; i < puzzle_pieces.size(); ++i)
    {
        if (puzzle_pieces[i] == 0) continue;
        for (size_t j = i + 1; j < puzzle_pieces.size(); ++j)
        {
            if (puzzle_pieces[j] != 0 && puzzle_pieces[i] > puzzle_pieces[j])
            {
                inversion_count++;
            }
        }
    }
    
    bool is_solvable = (inversion_count % 2 == 0);
    std::cout << "転置数: " << inversion_count << std::endl;
    std::cout << "解けるパズル: " << (is_solvable ? "はい" : "いいえ") << std::endl;
    
    // 目標状態への変換例
    std::vector<int> target_state = {1, 2, 3, 4, 5, 6, 7, 8, 0};
    std::cout << "\n目標状態:";
    display_puzzle(target_state);
}

void demonstrateOptimizationProblem()
{
    std::cout << "\n=== 最適化問題での順列活用 ===" << std::endl;
    
    // 街を訪問する巡回セールスマン問題（TSP）のミニ版
    struct City
    {
        std::string name;
        int x, y;
        
        City(const std::string& n, int px, int py) : name(n), x(px), y(py) {}
    };
    
    std::vector<City> cities = 
    {
        {"スタート", 0, 0},
        {"街A", 3, 4},
        {"街B", 6, 2},
        {"街C", 2, 7}
    };
    
    // 距離計算関数
    auto calculate_distance = [](const City& a, const City& b) 
    {
        int dx = a.x - b.x;
        int dy = a.y - b.y;
        return std::sqrt(dx*dx + dy*dy);
    };
    
    // 街のインデックス（スタート地点を除く）
    std::vector<int> city_indices = {1, 2, 3};
    
    std::cout << "都市間距離の最適化（巡回セールスマン問題）:" << std::endl;
    std::cout << "都市リスト:" << std::endl;
    for (size_t i = 0; i < cities.size(); ++i)
    {
        std::cout << i << ". " << cities[i].name 
                  << " (" << cities[i].x << ", " << cities[i].y << ")" << std::endl;
    }
    
    double shortest_distance = std::numeric_limits<double>::max();
    std::vector<int> best_route;
    
    int route_count = 0;
    do 
    {
        route_count++;
        
        // 現在のルートの総距離を計算
        double total_distance = 0.0;
        
        // スタート -> 最初の都市
        total_distance += calculate_distance(cities[0], cities[city_indices[0]]);
        
        // 都市間の移動
        for (size_t i = 1; i < city_indices.size(); ++i)
        {
            total_distance += calculate_distance(cities[city_indices[i-1]], cities[city_indices[i]]);
        }
        
        // 最後の都市 -> スタートに戻る
        total_distance += calculate_distance(cities[city_indices.back()], cities[0]);
        
        std::cout << "ルート" << route_count << ": スタート";
        for (int idx : city_indices)
        {
            std::cout << " -> " << cities[idx].name;
        }
        std::cout << " -> スタート | 距離: " << std::fixed << std::setprecision(2) << total_distance;
        
        if (total_distance < shortest_distance)
        {
            shortest_distance = total_distance;
            best_route = city_indices;
            std::cout << " <- 最短記録！";
        }
        
        std::cout << std::endl;
        
    } while (std::next_permutation(city_indices.begin(), city_indices.end()));
    
    std::cout << "\n最適ルート: スタート";
    for (int idx : best_route)
    {
        std::cout << " -> " << cities[idx].name;
    }
    std::cout << " -> スタート" << std::endl;
    std::cout << "最短距離: " << std::fixed << std::setprecision(2) << shortest_distance << std::endl;
}

int main()
{
    std::cout << "C++アルゴリズムとイテレータ - レクチャー7: 順列アルゴリズム" << std::endl;
    std::cout << "==========================================================" << std::endl;
    
    // 各順列アルゴリズムの実演
    demonstrateBasicPermutations();
    demonstrateNextPermutation();
    demonstratePrevPermutation();
    demonstrateCardGameCombinations();
    demonstratePartyFormation();
    demonstratePuzzleGeneration();
    demonstrateOptimizationProblem();
    
    std::cout << "\n=== まとめ ===" << std::endl;
    std::cout << "1. next_permutation: 辞書順で次の順列を生成" << std::endl;
    std::cout << "2. prev_permutation: 辞書順で前の順列を生成" << std::endl;
    std::cout << "3. is_permutation: 2つの範囲が順列関係かチェック" << std::endl;
    std::cout << "注意: 初期状態をソートしてから使用することが重要" << std::endl;
    std::cout << "ゲーム開発では、組み合わせ探索、パズル生成、最適化問題で活用" << std::endl;
    std::cout << "計算量はO(n)だが、全順列生成はO(n!)になるため大きなデータでは注意" << std::endl;
    
    return 0;
}