// 並列アルゴリズム（C++17）
// 標準ライブラリの並列実行ポリシーを活用

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <execution>
#include <chrono>
#include <random>
#include <string>

// パフォーマンス測定ヘルパー
template<typename Func>
void measureTime(const std::string& description, Func&& func)
{
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << description << ": " << duration.count() << "ms" << std::endl;
}

// テストデータ生成
std::vector<int> generateTestData(size_t size, int minVal = 1, int maxVal = 1000)
{
    std::vector<int> data(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(minVal, maxVal);
    
    std::generate(data.begin(), data.end(), [&]() { return dist(gen); });
    return data;
}

// ゲームデータ構造
struct GameEntity
{
    int id;
    float x, y, z;
    int health;
    float speed;
    bool isActive;
    
    GameEntity(int i = 0) 
        : id(i), x(i * 0.1f), y(i * 0.2f), z(i * 0.3f), 
          health(100 - (i % 50)), speed(1.0f + (i % 10) * 0.1f), isActive(i % 7 != 0) {}
};

struct Player
{
    int playerId;
    int score;
    int level;
    float experience;
    
    Player(int id = 0) 
        : playerId(id), score(id * 100), level(1 + id / 10), 
          experience(id * 15.5f) {}
};

// 並列ソートのデモンストレーション
void demonstrateParallelSort()
{
    std::cout << "\n=== 並列ソート ===" << std::endl;
    
    const size_t dataSize = 1000000;
    auto data1 = generateTestData(dataSize);
    auto data2 = data1;  // コピーを作成
    
    // 逐次ソート
    measureTime("逐次ソート", [&]() {
        std::sort(std::execution::seq, data1.begin(), data1.end());
    });
    
    // 並列ソート
    measureTime("並列ソート", [&]() {
        std::sort(std::execution::par, data2.begin(), data2.end());
    });
    
    // 結果が同じかチェック
    bool same = std::equal(data1.begin(), data1.end(), data2.begin());
    std::cout << "ソート結果の一致: " << (same ? "YES" : "NO") << std::endl;
}

// 並列検索とカウント
void demonstrateParallelSearch()
{
    std::cout << "\n=== 並列検索とカウント ===" << std::endl;
    
    const size_t dataSize = 10000000;
    auto data = generateTestData(dataSize, 1, 100);
    
    const int searchValue = 42;
    
    // 並列カウント
    size_t count;
    measureTime("並列カウント", [&]() {
        count = std::count(std::execution::par, data.begin(), data.end(), searchValue);
    });
    
    std::cout << "値 " << searchValue << " の出現回数: " << count << std::endl;
    
    // 並列条件カウント
    size_t evenCount;
    measureTime("並列条件カウント", [&]() {
        evenCount = std::count_if(std::execution::par, data.begin(), data.end(), 
                                 [](int x) { return x % 2 == 0; });
    });
    
    std::cout << "偶数の個数: " << evenCount << std::endl;
    
    // 並列検索
    auto it = std::find(std::execution::par, data.begin(), data.end(), searchValue);
    if (it != data.end())
    {
        std::cout << "値 " << searchValue << " を発見（位置: " 
                  << std::distance(data.begin(), it) << "）" << std::endl;
    }
}

// 並列変換（transform）
void demonstrateParallelTransform()
{
    std::cout << "\n=== 並列変換 ===" << std::endl;
    
    const size_t dataSize = 5000000;
    auto data = generateTestData(dataSize);
    std::vector<float> results(dataSize);
    
    // 並列変換：整数を平方根に変換
    measureTime("並列変換（平方根）", [&]() {
        std::transform(std::execution::par, data.begin(), data.end(), results.begin(),
                      [](int x) { return std::sqrt(static_cast<float>(x)); });
    });
    
    std::cout << "変換完了: " << data.size() << " 要素" << std::endl;
    std::cout << "例: " << data[0] << " → " << results[0] << std::endl;
    std::cout << "例: " << data[100] << " → " << results[100] << std::endl;
}

// ゲームエンティティの並列処理
void demonstrateGameEntityProcessing()
{
    std::cout << "\n=== ゲームエンティティの並列処理 ===" << std::endl;
    
    const size_t entityCount = 100000;
    std::vector<GameEntity> entities(entityCount);
    
    // エンティティを初期化
    std::iota(entities.begin(), entities.end(), 0);
    std::for_each(entities.begin(), entities.end(), [](GameEntity& e) {
        e = GameEntity(e.id);
    });
    
    // 並列でエンティティ位置を更新
    float deltaTime = 0.016f;  // 60FPS
    measureTime("エンティティ位置更新", [&]() {
        std::for_each(std::execution::par, entities.begin(), entities.end(),
                     [deltaTime](GameEntity& entity) {
                         if (entity.isActive)
                         {
                             entity.x += entity.speed * deltaTime;
                             entity.y += entity.speed * deltaTime * 0.5f;
                             entity.z += entity.speed * deltaTime * 0.3f;
                         }
                     });
    });
    
    // アクティブなエンティティ数をカウント
    size_t activeCount = std::count_if(std::execution::par, entities.begin(), entities.end(),
                                      [](const GameEntity& e) { return e.isActive; });
    
    std::cout << "アクティブなエンティティ数: " << activeCount << " / " << entityCount << std::endl;
    
    // 体力の低いエンティティを並列検索
    auto lowHealthIt = std::find_if(std::execution::par, entities.begin(), entities.end(),
                                   [](const GameEntity& e) { return e.health < 20; });
    
    if (lowHealthIt != entities.end())
    {
        std::cout << "体力の低いエンティティ発見: ID " << lowHealthIt->id 
                  << " (体力: " << lowHealthIt->health << ")" << std::endl;
    }
}

// 並列数値計算
void demonstrateParallelNumerics()
{
    std::cout << "\n=== 並列数値計算 ===" << std::endl;
    
    const size_t dataSize = 10000000;
    auto data = generateTestData(dataSize, 1, 100);
    
    // 並列合計
    long long sum;
    measureTime("並列合計", [&]() {
        sum = std::reduce(std::execution::par, data.begin(), data.end(), 0LL);
    });
    
    std::cout << "合計: " << sum << std::endl;
    
    // 並列内積（二つのベクトルの内積）
    auto data2 = generateTestData(dataSize, 1, 100);
    long long dotProduct;
    measureTime("並列内積", [&]() {
        dotProduct = std::transform_reduce(std::execution::par, 
                                          data.begin(), data.end(), 
                                          data2.begin(), 0LL);
    });
    
    std::cout << "内積: " << dotProduct << std::endl;
    
    // 並列累積和
    std::vector<long long> prefixSums(dataSize);
    measureTime("並列包含スキャン", [&]() {
        std::inclusive_scan(std::execution::par, data.begin(), data.end(), 
                           prefixSums.begin(), 0LL);
    });
    
    std::cout << "累積和計算完了" << std::endl;
    std::cout << "最初の10個の累積和: ";
    for (int i = 0; i < 10; ++i)
    {
        std::cout << prefixSums[i] << " ";
    }
    std::cout << std::endl;
}

// プレイヤーデータの並列分析
void demonstratePlayerDataAnalysis()
{
    std::cout << "\n=== プレイヤーデータ分析 ===" << std::endl;
    
    const size_t playerCount = 1000000;
    std::vector<Player> players(playerCount);
    
    // プレイヤーデータ初期化
    std::for_each(std::execution::par, players.begin(), players.end(),
                 [](Player& p) {
                     static int id = 0;
                     p = Player(++id);
                 });
    
    // 高レベルプレイヤーの数をカウント
    size_t highLevelCount = std::count_if(std::execution::par, players.begin(), players.end(),
                                         [](const Player& p) { return p.level >= 50; });
    
    std::cout << "レベル50以上のプレイヤー数: " << highLevelCount << std::endl;
    
    // 総スコアを並列計算
    long long totalScore = std::reduce(std::execution::par, players.begin(), players.end(), 0LL,
                                      [](long long acc, const Player& p) {
                                          return acc + p.score;
                                      });
    
    std::cout << "総スコア: " << totalScore << std::endl;
    
    // スコアでソート
    measureTime("プレイヤーをスコアでソート", [&]() {
        std::sort(std::execution::par, players.begin(), players.end(),
                 [](const Player& a, const Player& b) {
                     return a.score > b.score;
                 });
    });
    
    std::cout << "トップ5プレイヤー:" << std::endl;
    for (int i = 0; i < 5; ++i)
    {
        std::cout << "  " << (i + 1) << ". プレイヤー" << players[i].playerId 
                  << " スコア: " << players[i].score << std::endl;
    }
}

// カスタム並列処理
void demonstrateCustomParallelOperations()
{
    std::cout << "\n=== カスタム並列操作 ===" << std::endl;
    
    const size_t dataSize = 1000000;
    std::vector<float> temperatures = generateTestData(dataSize, -20, 50);
    
    // 複雑な変換：摂氏から華氏への変換とカテゴリ分類
    std::vector<std::string> categories(dataSize);
    
    measureTime("温度分類並列処理", [&]() {
        std::transform(std::execution::par, temperatures.begin(), temperatures.end(),
                      categories.begin(), [](float celsius) {
                          float fahrenheit = celsius * 9.0f / 5.0f + 32.0f;
                          
                          if (fahrenheit < 32.0f) return std::string("凍結");
                          else if (fahrenheit < 50.0f) return std::string("寒冷");
                          else if (fahrenheit < 70.0f) return std::string("涼しい");
                          else if (fahrenheit < 85.0f) return std::string("快適");
                          else if (fahrenheit < 100.0f) return std::string("暖かい");
                          else return std::string("暑い");
                      });
    });
    
    // カテゴリごとの数をカウント
    std::vector<std::string> categoryTypes = {"凍結", "寒冷", "涼しい", "快適", "暖かい", "暑い"};
    
    for (const auto& category : categoryTypes)
    {
        size_t count = std::count(std::execution::par, categories.begin(), categories.end(), category);
        std::cout << category << ": " << count << " 個" << std::endl;
    }
}

int main()
{
    std::cout << "=== 並列アルゴリズム（C++17）===" << std::endl;
    
    std::cout << "ハードウェア並行性: " << std::thread::hardware_concurrency() << " スレッド" << std::endl;
    
    // 1. 並列ソート
    demonstrateParallelSort();
    
    // 2. 並列検索とカウント
    demonstrateParallelSearch();
    
    // 3. 並列変換
    demonstrateParallelTransform();
    
    // 4. ゲームエンティティ処理
    demonstrateGameEntityProcessing();
    
    // 5. 並列数値計算
    demonstrateParallelNumerics();
    
    // 6. プレイヤーデータ分析
    demonstratePlayerDataAnalysis();
    
    // 7. カスタム並列操作
    demonstrateCustomParallelOperations();
    
    return 0;
}

// まとめ：
// - C++17の並列実行ポリシーで簡単に並列化
// - std::execution::par で多くのアルゴリズムを並列実行
// - 大量データの処理で大幅な性能向上
// - ゲーム開発では大量エンティティやデータ分析に活用
// - 並列リダクションや変換で複雑な計算も効率化