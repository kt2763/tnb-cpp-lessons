/**
 * @file best_practices.cpp
 * @brief ラムダ式のベストプラクティスとパフォーマンス
 * 
 * ラムダ式を効率的かつ保守しやすいコードで使用するための
 * ベストプラクティスとパフォーマンス最適化テクニックを学びます。
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <string>
#include <memory>
#include <chrono>
#include <numeric>
#include <execution>
#include <map>
#include <unordered_map>

namespace GameBestPractices
{
    // 1. ラムダの適切なキャプチャ方法
    void demonstrateCaptureStrategies()
    {
        std::cout << "=== キャプチャ戦略 ===\n";
        
        int playerScore = 1000;
        std::string playerName = "タロウ";
        std::vector<int> inventory = {1, 2, 3, 4, 5};
        
        // 悪い例: 不必要な値キャプチャ
        auto badLambda = [=]() // 全てをコピー（無駄）
        {
            return playerScore * 2; // playerScoreのみ使用
        };
        
        // 良い例: 必要なものだけキャプチャ
        auto goodLambda = [playerScore]() // 必要なものだけ
        {
            return playerScore * 2;
        };
        
        // 大きなオブジェクトは参照でキャプチャ
        auto processInventory = [&inventory]()
        {
            return std::accumulate(inventory.begin(), inventory.end(), 0);
        };
        
        // const参照で読み取り専用
        auto readOnlyAccess = [&playerName = std::as_const(playerName)]()
        {
            return playerName.length();
        };
        
        std::cout << "スコア倍加: " << goodLambda() << "\n";
        std::cout << "インベントリ合計: " << processInventory() << "\n";
        std::cout << "名前の長さ: " << readOnlyAccess() << "\n";
    }
    
    // 2. ラムダのサイズとパフォーマンス
    void demonstratePerformanceConsiderations()
    {
        std::cout << "\n=== パフォーマンス最適化 ===\n";
        
        std::vector<int> largeData(1000000);
        std::iota(largeData.begin(), largeData.end(), 1);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // 良い例: シンプルなラムダ（インライン化しやすい）
        auto result1 = std::count_if(largeData.begin(), largeData.end(),
            [](int x) { return x % 2 == 0; });
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        start = std::chrono::high_resolution_clock::now();
        
        // 悪い例: 複雑なラムダ（インライン化しにくい）
        std::function<bool(int)> complexLambda = [](int x)
        {
            // 不必要に複雑な処理
            int result = 0;
            for (int i = 0; i < 10; ++i)
            {
                result += x * i;
            }
            return result % 2 == 0;
        };
        
        auto result2 = std::count_if(largeData.begin(), largeData.end(), complexLambda);
        
        end = std::chrono::high_resolution_clock::now();
        auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "シンプルなラムダ: " << duration1.count() << "μs\n";
        std::cout << "複雑なラムダ: " << duration2.count() << "μs\n";
        std::cout << "結果1: " << result1 << ", 結果2: " << result2 << "\n";
    }
    
    // 3. ラムダの合成と再利用
    void demonstrateLambdaComposition()
    {
        std::cout << "\n=== ラムダの合成 ===\n";
        
        // 基本的なフィルター関数
        auto isEven = [](int x) { return x % 2 == 0; };
        auto isPositive = [](int x) { return x > 0; };
        auto isLarge = [](int x) { return x > 100; };
        
        // ラムダを合成する高階関数
        auto combineFilters = [](auto... filters)
        {
            return [filters...](auto value)
            {
                return (filters(value) && ...);
            };
        };
        
        auto combinedFilter = combineFilters(isEven, isPositive, isLarge);
        
        std::vector<int> testData = {-50, 50, 102, 103, 200, -200};
        
        std::cout << "元のデータ: ";
        for (int x : testData) std::cout << x << " ";
        std::cout << "\n";
        
        std::cout << "フィルター後（偶数 & 正数 & 大きい）: ";
        for (int x : testData)
        {
            if (combinedFilter(x))
            {
                std::cout << x << " ";
            }
        }
        std::cout << "\n";
    }
    
    // 4. ゲーム特化のラムダパターン
    class GameEventManager
    {
    private:
        std::unordered_map<std::string, std::vector<std::function<void()>>> eventHandlers;
        
    public:
        // ラムツェムプレートで型安全なイベント登録
        template<typename EventType>
        void subscribe(const std::string& eventName, std::function<void(const EventType&)> handler)
        {
            // 型消去で汎用的なコールバックに変換
            eventHandlers[eventName].push_back([handler]()
            {
                // 実際の実装では、イベントデータを渡す必要があります
                // ここでは簡略化しています
            });
        }
        
        // ラムダで簡単なイベント登録
        void subscribe(const std::string& eventName, std::function<void()> handler)
        {
            eventHandlers[eventName].push_back(std::move(handler));
        }
        
        void trigger(const std::string& eventName)
        {
            auto it = eventHandlers.find(eventName);
            if (it != eventHandlers.end())
            {
                for (const auto& handler : it->second)
                {
                    handler();
                }
            }
        }
    };
    
    void demonstrateGameEventSystem()
    {
        std::cout << "\n=== ゲームイベントシステム ===\n";
        
        GameEventManager eventManager;
        
        // ゲーム状態
        int playerScore = 0;
        int playerLevel = 1;
        
        // イベントハンドラーをラムダで登録
        eventManager.subscribe("score_increased", [&playerScore, &playerLevel]()
        {
            playerScore += 100;
            std::cout << "スコアが増加しました: " << playerScore << "\n";
            
            // レベルアップチェック
            if (playerScore >= playerLevel * 500)
            {
                playerLevel++;
                std::cout << "レベルアップ！新しいレベル: " << playerLevel << "\n";
            }
        });
        
        eventManager.subscribe("enemy_defeated", [&]()
        {
            std::cout << "敵を撃破しました！\n";
            // スコア増加イベントを発生
        });
        
        // イベント発生シミュレーション
        for (int i = 0; i < 3; ++i)
        {
            eventManager.trigger("enemy_defeated");
            eventManager.trigger("score_increased");
        }
    }
    
    // 5. ラムダのデバッグとテスト
    void demonstrateDebuggingTechniques()
    {
        std::cout << "\n=== デバッグ技法 ===\n";
        
        std::vector<int> gameData = {1, 5, 3, 8, 2, 9, 4};
        
        // デバッグ情報付きのラムダ
        auto debugFilter = [](const std::string& name)
        {
            return [name](int value)
            {
                bool result = value > 5;
                std::cout << "[DEBUG] " << name << ": " << value 
                         << " -> " << (result ? "true" : "false") << "\n";
                return result;
            };
        };
        
        std::cout << "値 > 5 のフィルタリング:\n";
        auto filteredData = std::vector<int>{};
        std::copy_if(gameData.begin(), gameData.end(), 
                     std::back_inserter(filteredData),
                     debugFilter("value_filter"));
        
        std::cout << "フィルター結果: ";
        for (int x : filteredData) std::cout << x << " ";
        std::cout << "\n";
    }
    
    // 6. メモリ管理のベストプラクティス
    void demonstrateMemoryManagement()
    {
        std::cout << "\n=== メモリ管理 ===\n";
        
        // 悪い例: 大きなオブジェクトを値キャプチャ
        std::vector<int> largeVector(10000, 42);
        
        // こうしない
        // auto badLambda = [largeVector]() { return largeVector.size(); };
        
        // 良い例: 参照キャプチャまたは必要な情報のみキャプチャ
        auto goodLambda = [&largeVector]() { return largeVector.size(); };
        
        // さらに良い例: 必要な情報のみキャプチャ
        auto size = largeVector.size();
        auto bestLambda = [size]() { return size; };
        
        std::cout << "ベクターサイズ: " << bestLambda() << "\n";
        
        // スマートポインタの扱い
        auto gameObject = std::make_shared<std::string>("GameEntity");
        
        // weak_ptrで循環参照を防ぐ
        std::weak_ptr<std::string> weakRef = gameObject;
        auto safeLambda = [weakRef]()
        {
            if (auto locked = weakRef.lock())
            {
                return *locked + " is alive";
            }
            return std::string("Object has been destroyed");
        };
        
        std::cout << "オブジェクト状態: " << safeLambda() << "\n";
        
        gameObject.reset(); // オブジェクトを破棄
        std::cout << "破棄後: " << safeLambda() << "\n";
    }
    
    // 7. ラムダのコンパイル最適化
    void demonstrateCompilerOptimizations()
    {
        std::cout << "\n=== コンパイラ最適化 ===\n";
        
        std::vector<int> data = {1, 2, 3, 4, 5};
        
        // インライン化されやすいラムダ
        auto simpleTransform = [](int x) -> int { return x * 2; };
        
        // constexprラムダ（C++17）
        auto constexprLambda = [](int x) constexpr -> int { return x * x; };
        
        // コンパイル時計算
        constexpr int compiletimeResult = constexprLambda(5);
        
        std::cout << "コンパイル時計算結果: " << compiletimeResult << "\n";
        
        // ランタイム変換
        std::vector<int> transformed;
        std::transform(data.begin(), data.end(), 
                       std::back_inserter(transformed), 
                       simpleTransform);
        
        std::cout << "変換結果: ";
        for (int x : transformed) std::cout << x << " ";
        std::cout << "\n";
    }
}

int main()
{
    using namespace GameBestPractices;
    
    demonstrateCaptureStrategies();
    demonstratePerformanceConsiderations();
    demonstrateLambdaComposition();
    demonstrateGameEventSystem();
    demonstrateDebuggingTechniques();
    demonstrateMemoryManagement();
    demonstrateCompilerOptimizations();
    
    std::cout << "\n=== ラムダベストプラクティスのまとめ ===\n";
    std::cout << "1. 必要な変数のみキャプチャし、大きなオブジェクトは参照で\n";
    std::cout << "2. シンプルなラムダを使用してインライン化を促進\n";
    std::cout << "3. ラムダを合成して再利用性を向上\n";
    std::cout << "4. イベントシステムで型安全なラムダを活用\n";
    std::cout << "5. デバッグ情報を含むラムダでトラブルシューティングを容易に\n";
    std::cout << "6. weak_ptrでメモリリークを防止\n";
    std::cout << "7. constexprラムダでコンパイル時最適化を活用\n";
    
    return 0;
}

/**
 * ラムダ式のベストプラクティスまとめ：
 * 
 * パフォーマンス:
 * - 必要な変数のみキャプチャし、不必要なコピーを避ける
 * - 大きなオブジェクトは参照キャプチャを使用
 * - シンプルなラムダでインライン化を促進
 * 
 * 保守性:
 * - ラムダを小さく、単一責任に保つ
 * - デバッグ情報を含むラムダでトラブルシューティングを容易に
 * - ラムダの合成で複雑なロジックを管理しやすく
 * 
 * メモリ管理:
 * - weak_ptrで循環参照やメモリリークを防止
 * - スマートポインタと組み合わせて安全に使用
 * 
 * 最適化:
 * - constexprラムダでコンパイル時計算を活用
 * - std::functionの使用は必要な時のみに限定
 */