// 例外のパフォーマンスと最適化
// 例外処理のコストを理解し、適切に使用する方法

#include <iostream>
#include <chrono>
#include <vector>
#include <optional>
#include <functional>
#include <numeric>
#include <memory>

// パフォーマンス測定ユーティリティ
class PerformanceTimer
{
private:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = Clock::time_point;
    
    std::string name;
    TimePoint start;
    
public:
    explicit PerformanceTimer(const std::string& n) 
        : name(n), start(Clock::now()) {}
        
    ~PerformanceTimer()
    {
        auto end = Clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << name << ": " << duration.count() << " μs" << std::endl;
    }
};

// テスト用の例外クラス
class GameException : public std::exception
{
private:
    std::string message;
    
public:
    explicit GameException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override { return message.c_str(); }
};

// パフォーマンステストクラス
class ExceptionPerformanceTest
{
private:
    static constexpr int ITERATIONS = 100000;
    
public:
    // 例外を使った実装
    int validateHealthWithException(int health)
    {
        if (health < 0)
        {
            throw GameException("体力が負の値です");
        }
        if (health > 9999)
        {
            throw GameException("体力が最大値を超えています");
        }
        return health;
    }
    
    // エラーコードを使った実装
    enum class HealthError
    {
        None,
        Negative,
        Overflow
    };
    
    std::pair<int, HealthError> validateHealthWithErrorCode(int health)
    {
        if (health < 0)
        {
            return {0, HealthError::Negative};
        }
        if (health > 9999)
        {
            return {9999, HealthError::Overflow};
        }
        return {health, HealthError::None};
    }
    
    // std::optionalを使った実装
    std::optional<int> validateHealthWithOptional(int health)
    {
        if (health < 0 || health > 9999)
        {
            return std::nullopt;
        }
        return health;
    }
    
    // 正常系のパフォーマンステスト
    void testNormalCase()
    {
        std::cout << "\n=== 正常系のパフォーマンス (エラーなし) ===" << std::endl;
        
        // 例外版
        {
            PerformanceTimer timer("例外版");
            int sum = 0;
            for (int i = 0; i < ITERATIONS; ++i)
            {
                try
                {
                    sum += validateHealthWithException(100);
                }
                catch (const GameException&)
                {
                    // エラー処理
                }
            }
        }
        
        // エラーコード版
        {
            PerformanceTimer timer("エラーコード版");
            int sum = 0;
            for (int i = 0; i < ITERATIONS; ++i)
            {
                auto [value, error] = validateHealthWithErrorCode(100);
                if (error == HealthError::None)
                {
                    sum += value;
                }
            }
        }
        
        // optional版
        {
            PerformanceTimer timer("optional版");
            int sum = 0;
            for (int i = 0; i < ITERATIONS; ++i)
            {
                auto result = validateHealthWithOptional(100);
                if (result)
                {
                    sum += *result;
                }
            }
        }
    }
    
    // 異常系のパフォーマンステスト
    void testErrorCase()
    {
        std::cout << "\n=== 異常系のパフォーマンス (1%エラー) ===" << std::endl;
        
        // 例外版
        {
            PerformanceTimer timer("例外版");
            int sum = 0;
            int errors = 0;
            for (int i = 0; i < ITERATIONS; ++i)
            {
                try
                {
                    // 1%の確率でエラー
                    int health = (i % 100 == 0) ? -1 : 100;
                    sum += validateHealthWithException(health);
                }
                catch (const GameException&)
                {
                    errors++;
                }
            }
            std::cout << "  エラー数: " << errors << std::endl;
        }
        
        // エラーコード版
        {
            PerformanceTimer timer("エラーコード版");
            int sum = 0;
            int errors = 0;
            for (int i = 0; i < ITERATIONS; ++i)
            {
                int health = (i % 100 == 0) ? -1 : 100;
                auto [value, error] = validateHealthWithErrorCode(health);
                if (error == HealthError::None)
                {
                    sum += value;
                }
                else
                {
                    errors++;
                }
            }
            std::cout << "  エラー数: " << errors << std::endl;
        }
        
        // optional版
        {
            PerformanceTimer timer("optional版");
            int sum = 0;
            int errors = 0;
            for (int i = 0; i < ITERATIONS; ++i)
            {
                int health = (i % 100 == 0) ? -1 : 100;
                auto result = validateHealthWithOptional(health);
                if (result)
                {
                    sum += *result;
                }
                else
                {
                    errors++;
                }
            }
            std::cout << "  エラー数: " << errors << std::endl;
        }
    }
};

// スタック巻き戻しのコスト
class StackUnwindingTest
{
private:
    struct Resource
    {
        std::string name;
        Resource(const std::string& n) : name(n) 
        {
            // std::cout << "リソース作成: " << name << std::endl;
        }
        ~Resource() 
        {
            // std::cout << "リソース破棄: " << name << std::endl;
        }
    };
    
public:
    // 深いコールスタックでの例外
    void deepFunction(int depth)
    {
        Resource res("深さ" + std::to_string(depth));
        
        if (depth <= 0)
        {
            throw GameException("最深部に到達");
        }
        
        deepFunction(depth - 1);
    }
    
    void testStackUnwinding()
    {
        std::cout << "\n=== スタック巻き戻しのコスト ===" << std::endl;
        
        const int DEPTH = 100;
        const int RUNS = 1000;
        
        // 浅い例外
        {
            PerformanceTimer timer("浅い例外 (深さ1)");
            for (int i = 0; i < RUNS; ++i)
            {
                try
                {
                    deepFunction(1);
                }
                catch (const GameException&)
                {
                    // 処理済み
                }
            }
        }
        
        // 深い例外
        {
            PerformanceTimer timer("深い例外 (深さ100)");
            for (int i = 0; i < RUNS; ++i)
            {
                try
                {
                    deepFunction(DEPTH);
                }
                catch (const GameException&)
                {
                    // 処理済み
                }
            }
        }
    }
};

// 例外最適化のテクニック
class OptimizationTechniques
{
public:
    // テクニック1: 例外を投げない関数をnoexceptでマーク
    int fastCalculation(int a, int b) noexcept
    {
        return a * b + a / (b + 1);  // b == -1でもクラッシュしない
    }
    
    // テクニック2: ホットパスから例外を除外
    class GameLoop
    {
    private:
        std::vector<int> entityHealths;
        
    public:
        GameLoop() : entityHealths(1000, 100) {}
        
        // 悪い例：毎フレーム例外をチェック
        void updateBad()
        {
            for (auto& health : entityHealths)
            {
                try
                {
                    if (health <= 0)
                    {
                        throw GameException("エンティティが死亡");
                    }
                    health--;
                }
                catch (const GameException&)
                {
                    health = 0;
                }
            }
        }
        
        // 良い例：例外を使わない
        void updateGood() noexcept
        {
            for (auto& health : entityHealths)
            {
                if (health > 0)
                {
                    health--;
                }
            }
        }
        
        void comparePerformance()
        {
            std::cout << "\n=== ホットパスの最適化 ===" << std::endl;
            
            const int FRAMES = 1000;
            
            {
                PerformanceTimer timer("例外を使う更新");
                for (int i = 0; i < FRAMES; ++i)
                {
                    updateBad();
                }
            }
            
            // リセット
            std::fill(entityHealths.begin(), entityHealths.end(), 100);
            
            {
                PerformanceTimer timer("例外を使わない更新");
                for (int i = 0; i < FRAMES; ++i)
                {
                    updateGood();
                }
            }
        }
    };
    
    // テクニック3: 例外オブジェクトの最適化
    class LightweightException : public std::exception
    {
    private:
        const char* staticMessage;
        
    public:
        explicit LightweightException(const char* msg) noexcept
            : staticMessage(msg) {}
            
        const char* what() const noexcept override
        {
            return staticMessage;
        }
    };
    
    // テクニック4: 例外の事前割り当て
    class PreallocatedException
    {
    private:
        static GameException commonErrors[3];
        
    public:
        static const GameException& getInvalidParameterError()
        {
            return commonErrors[0];
        }
        
        static const GameException& getResourceNotFoundError()
        {
            return commonErrors[1];
        }
        
        static const GameException& getOperationFailedError()
        {
            return commonErrors[2];
        }
    };
};

GameException OptimizationTechniques::PreallocatedException::commonErrors[3] = {
    GameException("無効なパラメータ"),
    GameException("リソースが見つかりません"),
    GameException("操作に失敗しました")
};

// ベストプラクティスのデモ
class BestPractices
{
public:
    // 初期化時のエラーは例外を使う
    class GameConfig
    {
    private:
        int screenWidth;
        int screenHeight;
        
    public:
        GameConfig(int w, int h)
        {
            if (w <= 0 || h <= 0)
            {
                throw std::invalid_argument("画面サイズが無効です");
            }
            screenWidth = w;
            screenHeight = h;
        }
    };
    
    // 頻繁に呼ばれる関数は例外を避ける
    class CollisionDetector
    {
    public:
        struct CollisionResult
        {
            bool hasCollision;
            float penetrationDepth;
        };
        
        // 良い設計：結果を返す
        CollisionResult checkCollision(const Vector2D& pos1, const Vector2D& pos2) noexcept
        {
            float distance = std::sqrt(
                std::pow(pos1.x - pos2.x, 2) + 
                std::pow(pos1.y - pos2.y, 2)
            );
            
            const float COLLISION_RADIUS = 1.0f;
            if (distance < COLLISION_RADIUS * 2)
            {
                return {true, COLLISION_RADIUS * 2 - distance};
            }
            
            return {false, 0.0f};
        }
    };
};

// Vector2D定義（前のレクチャーから）
struct Vector2D
{
    float x, y;
    Vector2D(float px, float py) : x(px), y(py) {}
};

int main()
{
    std::cout << "=== 例外のパフォーマンスと最適化 ===" << std::endl;
    
    // パフォーマンステスト
    ExceptionPerformanceTest perfTest;
    perfTest.testNormalCase();
    perfTest.testErrorCase();
    
    // スタック巻き戻しテスト
    StackUnwindingTest stackTest;
    stackTest.testStackUnwinding();
    
    // 最適化テクニック
    OptimizationTechniques::GameLoop gameLoop;
    gameLoop.comparePerformance();
    
    // ガイドライン
    std::cout << "\n=== パフォーマンスガイドライン ===" << std::endl;
    std::cout << "1. 正常系では例外のオーバーヘッドは小さい" << std::endl;
    std::cout << "2. 例外が投げられるとコストが大きい" << std::endl;
    std::cout << "3. スタックの深さに比例してコストが増加" << std::endl;
    std::cout << "4. ホットパス（ゲームループ等）では例外を避ける" << std::endl;
    std::cout << "5. 初期化やエラーが稀な処理では例外が適切" << std::endl;
    
    return 0;
}

// まとめ：
// - 例外の正常系オーバーヘッドは最小限
// - 例外が投げられると大きなコストが発生
// - パフォーマンスクリティカルな部分では例外を避ける
// - noexceptで最適化の機会を提供
// - 適切な場面で適切なエラーハンドリングを選択