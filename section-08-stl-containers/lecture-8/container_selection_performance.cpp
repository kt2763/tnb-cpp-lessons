/*
 * Lecture 8: Container Selection and Performance - コンテナ選択とパフォーマンス比較
 * 
 * 適切なコンテナの選択はゲームパフォーマンスに大きく影響します。
 * 各コンテナの特性を理解し、用途に応じた最適な選択を行うことが
 * 重要です。
 */

#include <iostream>
#include <vector>
#include <list>
#include <deque>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <array>
#include <chrono>
#include <random>
#include <algorithm>
#include <iomanip>

// パフォーマンステスト用のユーティリティ
class PerformanceTimer
{
private:
    std::chrono::high_resolution_clock::time_point start_time;
    
public:
    void start()
    {
        start_time = std::chrono::high_resolution_clock::now();
    }
    
    double getElapsedMs()
    {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start_time);
        return duration.count() / 1000.0;
    }
};

// ゲームオブジェクトの例
struct GameObject
{
    int id;
    float x, y, z;
    std::string type;
    
    GameObject(int i = 0, float px = 0.0f, float py = 0.0f, float pz = 0.0f, const std::string& t = "")
        : id(i), x(px), y(py), z(pz), type(t)
    {
    }
    
    bool operator<(const GameObject& other) const
    {
        return id < other.id;
    }
};

// ハッシュ関数
struct GameObjectHash
{
    size_t operator()(const GameObject& obj) const
    {
        return std::hash<int>()(obj.id);
    }
};

struct GameObjectEqual
{
    bool operator()(const GameObject& a, const GameObject& b) const
    {
        return a.id == b.id;
    }
};

int main()
{
    std::cout << \"=== STLコンテナ パフォーマンス比較とガイドライン ===\\n\\n\";\n    \n    const int smallSize = 1000;\n    const int mediumSize = 10000;\n    const int largeSize = 100000;\n    \n    PerformanceTimer timer;\n    std::random_device rd;\n    std::mt19937 gen(rd());\n    std::uniform_int_distribution<> dis(1, 1000000);\n    \n    // 1. 挿入パフォーマンス比較\n    std::cout << \"=== 挿入パフォーマンス比較 ===\\n\";\n    std::cout << std::setw(20) << \"コンテナ\" << std::setw(12) << \"要素数\" \n              << std::setw(15) << \"時間(ms)\" << std::setw(20) << \"特徴\\n\";\n    std::cout << std::string(67, '-') << \"\\n\";\n    \n    // vector - 後方挿入\n    {\n        std::vector<int> container;\n        container.reserve(mediumSize);\n        \n        timer.start();\n        for (int i = 0; i < mediumSize; ++i)\n        {\n            container.push_back(dis(gen));\n        }\n        auto elapsed = timer.getElapsedMs();\n        \n        std::cout << std::setw(20) << \"vector (push_back)\" \n                  << std::setw(12) << mediumSize \n                  << std::setw(15) << std::fixed << std::setprecision(2) << elapsed\n                  << std::setw(20) << \"高速（後方挿入）\\n\";\n    }\n    \n    // vector - 前方挿入（非効率）\n    {\n        std::vector<int> container;\n        \n        timer.start();\n        for (int i = 0; i < smallSize; ++i)  // 小さいサイズでテスト\n        {\n            container.insert(container.begin(), dis(gen));\n        }\n        auto elapsed = timer.getElapsedMs();\n        \n        std::cout << std::setw(20) << \"vector (push_front)\" \n                  << std::setw(12) << smallSize\n                  << std::setw(15) << elapsed\n                  << std::setw(20) << \"非常に遅い\\n\";\n    }\n    \n    // deque - 両端挿入\n    {\n        std::deque<int> container;\n        \n        timer.start();\n        for (int i = 0; i < mediumSize; ++i)\n        {\n            if (i % 2 == 0)\n            {\n                container.push_back(dis(gen));\n            }\n            else\n            {\n                container.push_front(dis(gen));\n            }\n        }\n        auto elapsed = timer.getElapsedMs();\n        \n        std::cout << std::setw(20) << \"deque (両端)\" \n                  << std::setw(12) << mediumSize\n                  << std::setw(15) << elapsed\n                  << std::setw(20) << \"両端で高速\\n\";\n    }\n    \n    // list - 挿入\n    {\n        std::list<int> container;\n        \n        timer.start();\n        for (int i = 0; i < mediumSize; ++i)\n        {\n            container.push_back(dis(gen));\n        }\n        auto elapsed = timer.getElapsedMs();\n        \n        std::cout << std::setw(20) << \"list\" \n                  << std::setw(12) << mediumSize\n                  << std::setw(15) << elapsed\n                  << std::setw(20) << \"どこでも高速\\n\";\n    }\n    \n    // set - 順序付き挿入\n    {\n        std::set<int> container;\n        \n        timer.start();\n        for (int i = 0; i < mediumSize; ++i)\n        {\n            container.insert(dis(gen));\n        }\n        auto elapsed = timer.getElapsedMs();\n        \n        std::cout << std::setw(20) << \"set\" \n                  << std::setw(12) << mediumSize\n                  << std::setw(15) << elapsed\n                  << std::setw(20) << \"O(log n), 自動ソート\\n\";\n    }\n    \n    // unordered_set - ハッシュ挿入\n    {\n        std::unordered_set<int> container;\n        container.reserve(mediumSize);\n        \n        timer.start();\n        for (int i = 0; i < mediumSize; ++i)\n        {\n            container.insert(dis(gen));\n        }\n        auto elapsed = timer.getElapsedMs();\n        \n        std::cout << std::setw(20) << \"unordered_set\" \n                  << std::setw(12) << mediumSize\n                  << std::setw(15) << elapsed\n                  << std::setw(20) << \"O(1)平均\\n\";\n    }\n    \n    // 2. 検索パフォーマンス比較\n    std::cout << \"\\n=== 検索パフォーマンス比較 ===\\n\";\n    std::cout << std::setw(20) << \"コンテナ\" << std::setw(12) << \"要素数\" \n              << std::setw(15) << \"時間(ms)\" << std::setw(20) << \"計算量\\n\";\n    std::cout << std::string(67, '-') << \"\\n\";\n    \n    // テストデータ準備\n    std::vector<int> testData;\n    std::set<int> sortedSet;\n    std::unordered_set<int> hashSet;\n    \n    for (int i = 0; i < mediumSize; ++i)\n    {\n        int value = dis(gen);\n        testData.push_back(value);\n        sortedSet.insert(value);\n        hashSet.insert(value);\n    }\n    \n    std::vector<int> searchTargets;\n    for (int i = 0; i < 1000; ++i)\n    {\n        searchTargets.push_back(testData[i % testData.size()]);\n    }\n    \n    // vector - 線形検索\n    {\n        timer.start();\n        int found = 0;\n        for (int target : searchTargets)\n        {\n            if (std::find(testData.begin(), testData.end(), target) != testData.end())\n            {\n                ++found;\n            }\n        }\n        auto elapsed = timer.getElapsedMs();\n        \n        std::cout << std::setw(20) << \"vector (線形検索)\" \n                  << std::setw(12) << mediumSize\n                  << std::setw(15) << elapsed\n                  << std::setw(20) << \"O(n)\\n\";\n    }\n    \n    // vector - 二分探索（ソート済み）\n    {\n        std::sort(testData.begin(), testData.end());\n        \n        timer.start();\n        int found = 0;\n        for (int target : searchTargets)\n        {\n            if (std::binary_search(testData.begin(), testData.end(), target))\n            {\n                ++found;\n            }\n        }\n        auto elapsed = timer.getElapsedMs();\n        \n        std::cout << std::setw(20) << \"vector (二分探索)\" \n                  << std::setw(12) << mediumSize\n                  << std::setw(15) << elapsed\n                  << std::setw(20) << \"O(log n)\\n\";\n    }\n    \n    // set - 木構造検索\n    {\n        timer.start();\n        int found = 0;\n        for (int target : searchTargets)\n        {\n            if (sortedSet.find(target) != sortedSet.end())\n            {\n                ++found;\n            }\n        }\n        auto elapsed = timer.getElapsedMs();\n        \n        std::cout << std::setw(20) << \"set\" \n                  << std::setw(12) << mediumSize\n                  << std::setw(15) << elapsed\n                  << std::setw(20) << \"O(log n)\\n\";\n    }\n    \n    // unordered_set - ハッシュ検索\n    {\n        timer.start();\n        int found = 0;\n        for (int target : searchTargets)\n        {\n            if (hashSet.find(target) != hashSet.end())\n            {\n                ++found;\n            }\n        }\n        auto elapsed = timer.getElapsedMs();\n        \n        std::cout << std::setw(20) << \"unordered_set\" \n                  << std::setw(12) << mediumSize\n                  << std::setw(15) << elapsed\n                  << std::setw(20) << \"O(1)平均\\n\";\n    }\n    \n    // 3. メモリ使用量の考慮\n    std::cout << \"\\n=== メモリ使用量比較 ===\\n\";\n    \n    const int testSize = 10000;\n    \n    {\n        std::vector<int> container(testSize, 42);\n        size_t vectorSize = container.size() * sizeof(int) + sizeof(container);\n        std::cout << \"vector: \" << vectorSize << \" bytes (連続メモリ)\\n\";\n    }\n    \n    {\n        std::list<int> container(testSize, 42);\n        size_t listSize = container.size() * (sizeof(int) + 2 * sizeof(void*)) + sizeof(container);\n        std::cout << \"list: 約\" << listSize << \" bytes (ポインタオーバーヘッド)\\n\";\n    }\n    \n    {\n        std::set<int> container;\n        for (int i = 0; i < testSize; ++i)\n        {\n            container.insert(i);\n        }\n        size_t setSize = container.size() * (sizeof(int) + 3 * sizeof(void*) + sizeof(char)) + sizeof(container);\n        std::cout << \"set: 約\" << setSize << \" bytes (木構造オーバーヘッド)\\n\";\n    }\n    \n    {\n        std::unordered_set<int> container;\n        for (int i = 0; i < testSize; ++i)\n        {\n            container.insert(i);\n        }\n        size_t hashSize = container.bucket_count() * sizeof(void*) + \n                         container.size() * (sizeof(int) + sizeof(void*)) + sizeof(container);\n        std::cout << \"unordered_set: 約\" << hashSize << \" bytes (ハッシュテーブル)\\n\";\n    }\n    \n    // 4. ゲーム開発での使用ガイドライン\n    std::cout << \"\\n=== ゲーム開発での使用ガイドライン ===\\n\";\n    std::cout << \"\\n【シーン別推奨コンテナ】\\n\";\n    std::cout << \"ゲームオブジェクト管理:\\n\";\n    std::cout << \"  - 大量のオブジェクト: vector (メモリ効率、キャッシュ効率)\\n\";\n    std::cout << \"  - 頻繁な追加/削除: list (O(1)挿入/削除)\\n\";\n    std::cout << \"  - ID検索重視: unordered_map<ID, GameObject*>\\n\";\n    \n    std::cout << \"\\nプレイヤー管理:\\n\";\n    std::cout << \"  - アクティブプレイヤー: unordered_set<PlayerID>\\n\";\n    std::cout << \"  - ランキング: multiset<Player> (自動ソート)\\n\";\n    std::cout << \"  - プレイヤーデータ: unordered_map<PlayerID, PlayerData>\\n\";\n    \n    std::cout << \"\\nリソース管理:\\n\";\n    std::cout << \"  - アセットキャッシュ: unordered_map<string, Resource*>\\n\";\n    std::cout << \"  - LRUキャッシュ: list + unordered_map の組み合わせ\\n\";\n    \n    std::cout << \"\\nゲームロジック:\\n\";\n    std::cout << \"  - イベントキュー: queue (FIFO)\\n\";\n    std::cout << \"  - 優先度付きタスク: priority_queue\\n\";\n    std::cout << \"  - Undo/Redo: stack (LIFO)\\n\";\n    \n    // 5. 実際のゲームシナリオでのテスト\n    std::cout << \"\\n=== 実際のゲームシナリオ ===\\n\";\n    \n    // シナリオ1: リアルタイム弾丸管理\n    std::cout << \"\\nシナリオ1: リアルタイム弾丸管理\\n\";\n    {\n        const int bulletCount = 10000;\n        \n        // vector使用（推奨）\n        std::vector<GameObject> bullets;\n        bullets.reserve(bulletCount);\n        \n        timer.start();\n        // 弾丸追加\n        for (int i = 0; i < bulletCount; ++i)\n        {\n            bullets.emplace_back(i, i * 0.1f, i * 0.1f, 0.0f, \"bullet\");\n        }\n        \n        // 画面外の弾丸を削除\n        bullets.erase(\n            std::remove_if(bullets.begin(), bullets.end(),\n                [](const GameObject& bullet) { return bullet.x > 100.0f; }),\n            bullets.end()\n        );\n        \n        auto elapsed = timer.getElapsedMs();\n        std::cout << \"vector: \" << elapsed << \"ms (\" << bullets.size() << \"弾丸残存)\\n\";\n    }\n    \n    // シナリオ2: プレイヤー検索システム\n    std::cout << \"\\nシナリオ2: プレイヤー検索システム\\n\";\n    {\n        const int playerCount = 50000;\n        \n        std::unordered_map<int, GameObject> playerDB;\n        std::map<int, GameObject> sortedPlayerDB;\n        \n        // データベース構築\n        for (int i = 0; i < playerCount; ++i)\n        {\n            GameObject player(i, 0, 0, 0, \"player\");\n            playerDB[i] = player;\n            sortedPlayerDB[i] = player;\n        }\n        \n        std::vector<int> searchIDs;\n        for (int i = 0; i < 10000; ++i)\n        {\n            searchIDs.push_back(dis(gen) % playerCount);\n        }\n        \n        // unordered_map検索\n        timer.start();\n        int found1 = 0;\n        for (int id : searchIDs)\n        {\n            if (playerDB.find(id) != playerDB.end())\n            {\n                ++found1;\n            }\n        }\n        auto elapsed1 = timer.getElapsedMs();\n        \n        // map検索\n        timer.start();\n        int found2 = 0;\n        for (int id : searchIDs)\n        {\n            if (sortedPlayerDB.find(id) != sortedPlayerDB.end())\n            {\n                ++found2;\n            }\n        }\n        auto elapsed2 = timer.getElapsedMs();\n        \n        std::cout << \"unordered_map: \" << elapsed1 << \"ms\\n\";\n        std::cout << \"map: \" << elapsed2 << \"ms\\n\";\n        std::cout << \"速度向上: \" << std::fixed << std::setprecision(1) \n                  << (elapsed2 / elapsed1) << \"倍\\n\";\n    }\n    \n    // 6. 最適化のヒント\n    std::cout << \"\\n=== 最適化のヒント ===\\n\";\n    std::cout << \"1. reserve()でメモリ再配置を防ぐ\\n\";\n    std::cout << \"2. emplace系関数で不要なコピーを避ける\\n\";\n    std::cout << \"3. 削除時はerase-removeイディオムを使用\\n\";\n    std::cout << \"4. 範囲for文でイテレータオーバーヘッドを削減\\n\";\n    std::cout << \"5. 適切なコンテナ選択で計算量を最適化\\n\";\n    std::cout << \"6. constな操作ではconst_iteratorを使用\\n\";\n    std::cout << \"7. 大きなオブジェクトはポインタで管理\\n\";\n    \n    return 0;\n}\n\n/*\n * まとめ：\n * - コンテナ選択はパフォーマンスに大きく影響する\n * - vector: 連続メモリで高速、後方挿入が効率的\n * - list: 任意位置での挿入/削除がO(1)\n * - deque: 両端での操作が効率的\n * - set/map: 自動ソート、O(log n)操作\n * - unordered_set/map: 平均O(1)、検索重視の場合に最適\n * - 用途に応じた適切な選択がゲームパフォーマンス向上の鍵\n * - メモリ使用量とキャッシュ効率も考慮に入れる\n */"}]