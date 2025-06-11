/*
 * C++モダンコース - セクション9: アルゴリズムとイテレータ
 * レクチャー12: アルゴリズムの選択とパフォーマンス
 * 
 * このレクチャーでは、適切なアルゴリズムの選択方法とパフォーマンス最適化を学びます
 * ゲーム開発での実際のシナリオを通じて、効率的なアルゴリズム選択の指針を身に付けます
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <chrono>
#include <random>
#include <string>
#include <iomanip>
#include <list>
#include <deque>

// パフォーマンス測定ヘルパー
template<typename Func>
auto measure_performance(const std::string& description, Func&& func) -> std::chrono::microseconds
{
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << std::setw(30) << description << ": " 
              << std::setw(8) << duration.count() << " μs" << std::endl;
    
    return duration;
}

// ゲームエンティティのデータ構造
struct GameEntity
{
    int id;
    std::string name;
    float x, y;
    int level;
    int health;
    
    GameEntity(int entity_id, const std::string& entity_name, float px, float py, int lv, int hp)
        : id(entity_id), name(entity_name), x(px), y(py), level(lv), health(hp) {}
    
    bool operator<(const GameEntity& other) const { return id < other.id; }
    bool operator==(const GameEntity& other) const { return id == other.id; }
};

// ハッシュ関数（unordered_setで使用）
struct EntityHash
{
    std::size_t operator()(const GameEntity& entity) const
    {
        return std::hash<int>()(entity.id);
    }
};

void demonstrateSearchAlgorithmComparison()
{
    std::cout << "\n=== 検索アルゴリズムの比較 ===" << std::endl;
    
    // テストデータの準備
    const size_t data_size = 100000;
    std::vector<int> sorted_data(data_size);
    std::iota(sorted_data.begin(), sorted_data.end(), 1);
    
    std::vector<int> unsorted_data = sorted_data;
    std::random_shuffle(unsorted_data.begin(), unsorted_data.end());
    
    std::set<int> set_data(sorted_data.begin(), sorted_data.end());
    std::unordered_set<int> unordered_set_data(sorted_data.begin(), sorted_data.end());
    
    const int target = data_size / 2; // 中央値を検索
    const int searches = 1000; // 検索回数
    
    std::cout << "データサイズ: " << data_size << ", 検索回数: " << searches << std::endl;
    std::cout << "検索対象: " << target << std::endl << std::endl;
    
    // 1. 線形検索（unsorted vector）
    measure_performance("線形検索（vector）", [&]() 
    {
        for (int i = 0; i < searches; ++i)
        {
            auto result = std::find(unsorted_data.begin(), unsorted_data.end(), target);
            (void)result; // 警告抑制
        }
    });
    
    // 2. 二分探索（sorted vector）
    measure_performance("二分探索（sorted vector）", [&]() 
    {
        for (int i = 0; i < searches; ++i)
        {
            bool found = std::binary_search(sorted_data.begin(), sorted_data.end(), target);
            (void)found; // 警告抑制
        }
    });
    
    // 3. std::set検索
    measure_performance("set::find", [&]() 
    {
        for (int i = 0; i < searches; ++i)
        {
            auto result = set_data.find(target);
            (void)result; // 警告抑制
        }
    });
    
    // 4. std::unordered_set検索
    measure_performance("unordered_set::find", [&]() 
    {
        for (int i = 0; i < searches; ++i)
        {
            auto result = unordered_set_data.find(target);
            (void)result; // 警告抑制
        }
    });
    
    std::cout << "\n検索アルゴリズムの選択指針:" << std::endl;
    std::cout << "- 小さなデータ（<100要素）: 線形検索で十分" << std::endl;
    std::cout << "- ソート済みデータ: binary_search（O(log n)）" << std::endl;
    std::cout << "- 頻繁な検索: unordered_set（O(1)平均）" << std::endl;
    std::cout << "- メモリ効率重視: sorted vector + binary_search" << std::endl;
}

void demonstrateSortingAlgorithmComparison()
{
    std::cout << "\n=== ソートアルゴリズムの比較 ===" << std::endl;
    
    // 異なる特性のデータセットを準備
    const size_t data_size = 50000;
    
    // 1. ランダムデータ
    std::vector<int> random_data(data_size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, data_size);
    std::generate(random_data.begin(), random_data.end(), [&]() { return dist(gen); });
    
    // 2. ほぼソート済みデータ
    std::vector<int> nearly_sorted_data(data_size);
    std::iota(nearly_sorted_data.begin(), nearly_sorted_data.end(), 1);
    // 5%の要素をランダムに交換
    for (size_t i = 0; i < data_size / 20; ++i)
    {
        size_t pos1 = gen() % data_size;
        size_t pos2 = gen() % data_size;
        std::swap(nearly_sorted_data[pos1], nearly_sorted_data[pos2]);
    }
    
    // 3. 逆順データ
    std::vector<int> reverse_sorted_data(data_size);
    std::iota(reverse_sorted_data.begin(), reverse_sorted_data.end(), 1);
    std::reverse(reverse_sorted_data.begin(), reverse_sorted_data.end());
    
    std::cout << "データサイズ: " << data_size << std::endl << std::endl;
    
    // ランダムデータでのテスト
    std::cout << "ランダムデータのソート性能:" << std::endl;
    
    auto test_data = random_data;
    measure_performance("std::sort", [&]() 
    {
        test_data = random_data;
        std::sort(test_data.begin(), test_data.end());
    });
    
    measure_performance("std::stable_sort", [&]() 
    {
        test_data = random_data;
        std::stable_sort(test_data.begin(), test_data.end());
    });
    
    measure_performance("std::partial_sort (top 1000)", [&]() 
    {
        test_data = random_data;
        std::partial_sort(test_data.begin(), test_data.begin() + 1000, test_data.end());
    });
    
    // ほぼソート済みデータでのテスト
    std::cout << "\nほぼソート済みデータのソート性能:" << std::endl;
    
    measure_performance("std::sort", [&]() 
    {
        test_data = nearly_sorted_data;
        std::sort(test_data.begin(), test_data.end());
    });
    
    measure_performance("std::stable_sort", [&]() 
    {
        test_data = nearly_sorted_data;
        std::stable_sort(test_data.begin(), test_data.end());
    });
    
    std::cout << "\nソートアルゴリズムの選択指針:" << std::endl;
    std::cout << "- 一般的な用途: std::sort（最も高速）" << std::endl;
    std::cout << "- 安定性が必要: std::stable_sort" << std::endl;
    std::cout << "- 上位N個のみ: std::partial_sort" << std::endl;
    std::cout << "- ほぼソート済み: std::sort でも効率的" << std::endl;
}

void demonstrateContainerChoice()
{
    std::cout << "\n=== コンテナ選択によるパフォーマンス影響 ===" << std::endl;
    
    const size_t operation_count = 10000;
    const size_t initial_size = 1000;
    
    std::cout << "操作回数: " << operation_count << ", 初期サイズ: " << initial_size << std::endl << std::endl;
    
    // 1. 前方挿入の比較
    std::cout << "前方挿入の性能比較:" << std::endl;
    
    measure_performance("vector::insert(begin)", [&]() 
    {
        std::vector<int> vec(initial_size, 0);
        for (size_t i = 0; i < operation_count / 10; ++i) // vectorは重いので回数を減らす
        {
            vec.insert(vec.begin(), static_cast<int>(i));
        }
    });
    
    measure_performance("deque::push_front", [&]() 
    {
        std::deque<int> deq(initial_size, 0);
        for (size_t i = 0; i < operation_count; ++i)
        {
            deq.push_front(static_cast<int>(i));
        }
    });
    
    measure_performance("list::push_front", [&]() 
    {
        std::list<int> lst(initial_size, 0);
        for (size_t i = 0; i < operation_count; ++i)
        {
            lst.push_front(static_cast<int>(i));
        }
    });
    
    // 2. ランダムアクセスの比較
    std::cout << "\nランダムアクセスの性能比較:" << std::endl;
    
    std::vector<int> vec(initial_size);
    std::deque<int> deq(initial_size);
    std::iota(vec.begin(), vec.end(), 0);
    std::iota(deq.begin(), deq.end(), 0);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> index_dist(0, initial_size - 1);
    
    measure_performance("vector random access", [&]() 
    {
        int sum = 0;
        for (size_t i = 0; i < operation_count; ++i)
        {
            sum += vec[index_dist(gen)];
        }
        (void)sum; // 警告抑制
    });
    
    measure_performance("deque random access", [&]() 
    {
        int sum = 0;
        for (size_t i = 0; i < operation_count; ++i)
        {
            sum += deq[index_dist(gen)];
        }
        (void)sum; // 警告抑制
    });
    
    std::cout << "\nコンテナ選択の指針:" << std::endl;
    std::cout << "- 後方挿入中心: vector" << std::endl;
    std::cout << "- 前方・後方挿入: deque" << std::endl;
    std::cout << "- 中間挿入・削除頻繁: list" << std::endl;
    std::cout << "- ランダムアクセス重要: vector > deque >> list" << std::endl;
}

void demonstrateGameSpecificOptimizations()
{
    std::cout << "\n=== ゲーム開発での最適化テクニック ===" << std::endl;
    
    // 1. エンティティ管理システムの比較
    std::cout << "1. エンティティ管理システムの比較:" << std::endl;
    
    const size_t entity_count = 10000;
    const size_t lookup_count = 1000;
    
    // データ準備
    std::vector<GameEntity> entities;
    entities.reserve(entity_count);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> pos_dist(0.0f, 1000.0f);
    std::uniform_int_distribution<> level_dist(1, 50);
    std::uniform_int_distribution<> health_dist(100, 1000);
    
    for (size_t i = 0; i < entity_count; ++i)
    {
        entities.emplace_back(
            static_cast<int>(i),
            "Entity" + std::to_string(i),
            pos_dist(gen), pos_dist(gen),
            level_dist(gen), health_dist(gen)
        );
    }
    
    // ID検索の比較
    std::vector<int> lookup_ids;
    for (size_t i = 0; i < lookup_count; ++i)
    {
        lookup_ids.push_back(gen() % entity_count);
    }
    
    std::cout << "エンティティ数: " << entity_count << ", 検索回数: " << lookup_count << std::endl;
    
    // vector + linear search
    measure_performance("vector linear search", [&]() 
    {
        for (int id : lookup_ids)
        {
            auto it = std::find_if(entities.begin(), entities.end(),
                [id](const GameEntity& e) { return e.id == id; });
            (void)it; // 警告抑制
        }
    });
    
    // map lookup
    std::map<int, GameEntity> entity_map;
    for (const auto& entity : entities)
    {
        entity_map[entity.id] = entity;
    }
    
    measure_performance("map lookup", [&]() 
    {
        for (int id : lookup_ids)
        {
            auto it = entity_map.find(id);
            (void)it; // 警告抑制
        }
    });
    
    // unordered_map lookup
    std::unordered_map<int, GameEntity> entity_unordered_map;
    for (const auto& entity : entities)
    {
        entity_unordered_map[entity.id] = entity;
    }
    
    measure_performance("unordered_map lookup", [&]() 
    {
        for (int id : lookup_ids)
        {
            auto it = entity_unordered_map.find(id);
            (void)it; // 警告抑制
        }
    });
    
    // 2. 範囲検索の最適化
    std::cout << "\n2. 範囲検索の最適化（レベル範囲検索）:" << std::endl;
    
    int min_level = 20;
    int max_level = 30;
    
    // 線形検索
    measure_performance("linear range search", [&]() 
    {
        std::vector<GameEntity> result;
        std::copy_if(entities.begin(), entities.end(), std::back_inserter(result),
            [min_level, max_level](const GameEntity& e) 
            {
                return e.level >= min_level && e.level <= max_level;
            });
    });
    
    // ソート + binary search
    auto sorted_entities = entities;
    std::sort(sorted_entities.begin(), sorted_entities.end(),
        [](const GameEntity& a, const GameEntity& b) { return a.level < b.level; });
    
    measure_performance("sorted + binary search", [&]() 
    {
        GameEntity min_entity(0, "", 0, 0, min_level, 0);
        GameEntity max_entity(0, "", 0, 0, max_level + 1, 0);
        
        auto lower = std::lower_bound(sorted_entities.begin(), sorted_entities.end(), min_entity,
            [](const GameEntity& a, const GameEntity& b) { return a.level < b.level; });
        auto upper = std::lower_bound(sorted_entities.begin(), sorted_entities.end(), max_entity,
            [](const GameEntity& a, const GameEntity& b) { return a.level < b.level; });
        
        std::vector<GameEntity> result(lower, upper);
    });
}

void demonstrateAlgorithmComplexity()
{
    std::cout << "\n=== アルゴリズムの計算量と実測 ===" << std::endl;
    
    // 異なるサイズでの性能測定
    std::vector<size_t> sizes = {1000, 5000, 10000, 50000, 100000};
    
    std::cout << std::setw(10) << "サイズ" 
              << std::setw(15) << "線形検索(μs)" 
              << std::setw(15) << "二分探索(μs)" 
              << std::setw(15) << "ソート(μs)" << std::endl;
    std::cout << std::string(55, '-') << std::endl;
    
    for (size_t size : sizes)
    {
        // データ準備
        std::vector<int> sorted_data(size);
        std::iota(sorted_data.begin(), sorted_data.end(), 1);
        
        std::vector<int> unsorted_data = sorted_data;
        std::random_shuffle(unsorted_data.begin(), unsorted_data.end());
        
        int target = size / 2;
        
        // 線形検索
        auto linear_time = measure_performance("", [&]() 
        {
            auto result = std::find(unsorted_data.begin(), unsorted_data.end(), target);
            (void)result;
        });
        
        // 二分探索
        auto binary_time = measure_performance("", [&]() 
        {
            bool found = std::binary_search(sorted_data.begin(), sorted_data.end(), target);
            (void)found;
        });
        
        // ソート
        auto sort_data = unsorted_data;
        auto sort_time = measure_performance("", [&]() 
        {
            std::sort(sort_data.begin(), sort_data.end());
        });
        
        std::cout << std::setw(10) << size
                  << std::setw(15) << linear_time.count()
                  << std::setw(15) << binary_time.count()
                  << std::setw(15) << sort_time.count() << std::endl;
    }
    
    std::cout << "\n計算量の理論と実測:" << std::endl;
    std::cout << "- 線形検索: O(n) - サイズに比例して時間増加" << std::endl;
    std::cout << "- 二分探索: O(log n) - サイズ増加に対して緩やかな時間増加" << std::endl;
    std::cout << "- ソート: O(n log n) - 線形と二分探索の中間的な増加" << std::endl;
}

void demonstrateBestPractices()
{
    std::cout << "\n=== ベストプラクティスとアンチパターン ===" << std::endl;
    
    const size_t data_size = 100000;
    std::vector<int> data(data_size);
    std::iota(data.begin(), data.end(), 1);
    
    std::cout << "1. 効率的なコンテナ操作:" << std::endl;
    
    // 悪い例: 毎回push_backで容量拡張
    measure_performance("悪い例: reserve未使用", [&]() 
    {
        std::vector<int> vec;
        for (size_t i = 0; i < data_size / 10; ++i) // サイズを小さくして測定
        {
            vec.push_back(static_cast<int>(i));
        }
    });
    
    // 良い例: reserve で容量を予約
    measure_performance("良い例: reserve使用", [&]() 
    {
        std::vector<int> vec;
        vec.reserve(data_size / 10);
        for (size_t i = 0; i < data_size / 10; ++i)
        {
            vec.push_back(static_cast<int>(i));
        }
    });
    
    std::cout << "\n2. 不要なコピーの回避:" << std::endl;
    
    // 悪い例: 値渡し
    measure_performance("悪い例: 値渡しfor_each", [&]() 
    {
        int sum = 0;
        std::for_each(data.begin(), data.end(),
            [&sum](int value) 
            {
                sum += value * value; // 重い計算のシミュレート
            });
    });
    
    // 良い例: 参照渡し
    measure_performance("良い例: 参照渡しfor_each", [&]() 
    {
        int sum = 0;
        std::for_each(data.begin(), data.end(),
            [&sum](const int& value) 
            {
                sum += value * value;
            });
    });
    
    std::cout << "\n3. 適切なアルゴリズムの選択:" << std::endl;
    
    // 悪い例: 単純な検索でソート
    measure_performance("悪い例: 検索のためのソート", [&]() 
    {
        auto sorted_data = data;
        std::sort(sorted_data.begin(), sorted_data.end());
        bool found = std::binary_search(sorted_data.begin(), sorted_data.end(), 50000);
        (void)found;
    });
    
    // 良い例: 一回だけの検索なら線形検索
    measure_performance("良い例: 線形検索", [&]() 
    {
        auto result = std::find(data.begin(), data.end(), 50000);
        (void)result;
    });
    
    std::cout << "\nベストプラクティス:" << std::endl;
    std::cout << "1. 適切なコンテナを選択" << std::endl;
    std::cout << "2. 容量の事前確保（reserve）" << std::endl;
    std::cout << "3. 不要なコピーを避ける（参照渡し）" << std::endl;
    std::cout << "4. 使用頻度に応じたアルゴリズム選択" << std::endl;
    std::cout << "5. プロファイリングによる実測" << std::endl;
}

int main()
{
    std::cout << "C++アルゴリズムとイテレータ - レクチャー12: アルゴリズムの選択とパフォーマンス" << std::endl;
    std::cout << "=======================================================================" << std::endl;
    
    // 各最適化テクニックの実演
    demonstrateSearchAlgorithmComparison();
    demonstrateSortingAlgorithmComparison();
    demonstrateContainerChoice();
    demonstrateGameSpecificOptimizations();
    demonstrateAlgorithmComplexity();
    demonstrateBestPractices();
    
    std::cout << "\n=== 総合まとめ ===" << std::endl;
    std::cout << "アルゴリズム選択の指針:" << std::endl;
    std::cout << "1. データサイズを考慮" << std::endl;
    std::cout << "   - 小: 線形アルゴリズムでも十分" << std::endl;
    std::cout << "   - 大: 効率的なアルゴリズム必須" << std::endl;
    std::cout << "2. アクセスパターンを分析" << std::endl;
    std::cout << "   - 一回限り: 線形検索" << std::endl;
    std::cout << "   - 頻繁: ハッシュテーブル/ソート済み" << std::endl;
    std::cout << "3. メモリ使用量とのトレードオフ" << std::endl;
    std::cout << "4. 実装コストと保守性" << std::endl;
    std::cout << "5. 実際の測定による検証" << std::endl;
    std::cout << "\nゲーム開発では、フレームレート維持が最重要" << std::endl;
    std::cout << "プロファイラーを活用して実際のボトルネックを特定しよう" << std::endl;
    
    return 0;
}