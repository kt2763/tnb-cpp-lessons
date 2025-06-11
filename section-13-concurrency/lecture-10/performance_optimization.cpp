// 並行プログラミングのパフォーマンス最適化
// 効率的な並行プログラムの設計と最適化技法

#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <future>
#include <queue>
#include <mutex>
#include <condition_variable>

// パフォーマンス測定ユーティリティ
class PerformanceProfiler
{
private:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = Clock::time_point;
    
    TimePoint startTime_;
    std::string operation_;
    
public:
    explicit PerformanceProfiler(const std::string& operation)
        : operation_(operation), startTime_(Clock::now()) {}
    
    ~PerformanceProfiler()
    {
        auto endTime = Clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime_);
        std::cout << operation_ << ": " << duration.count() << " μs" << std::endl;
    }
};

// キャッシュライン最適化
struct alignas(64) CacheLinePadded
{
    std::atomic<int> counter;
    char padding[64 - sizeof(std::atomic<int>)];  // キャッシュライン境界まで埋める
};

// False Sharingの比較
class FalseSharingDemo
{
private:
    struct UnoptimizedCounters
    {
        std::atomic<int> counter1;
        std::atomic<int> counter2;
        std::atomic<int> counter3;
        std::atomic<int> counter4;
    };
    
    struct OptimizedCounters
    {
        alignas(64) std::atomic<int> counter1;
        alignas(64) std::atomic<int> counter2;
        alignas(64) std::atomic<int> counter3;
        alignas(64) std::atomic<int> counter4;
    };
    
public:
    void demonstrateFalseSharing()
    {
        std::cout << "\n=== False Sharing の影響 ===" << std::endl;
        
        const int iterations = 1000000;
        
        // 最適化されていない版
        {
            UnoptimizedCounters counters{};
            std::vector<std::thread> threads;
            
            PerformanceProfiler profiler("最適化前（False Sharing）");
            
            threads.emplace_back([&]() {
                for (int i = 0; i < iterations; ++i)
                    counters.counter1.fetch_add(1, std::memory_order_relaxed);
            });
            
            threads.emplace_back([&]() {
                for (int i = 0; i < iterations; ++i)
                    counters.counter2.fetch_add(1, std::memory_order_relaxed);
            });
            
            for (auto& t : threads)
                t.join();
        }
        
        // 最適化された版
        {
            OptimizedCounters counters{};
            std::vector<std::thread> threads;
            
            PerformanceProfiler profiler("最適化後（Padding適用）");
            
            threads.emplace_back([&]() {
                for (int i = 0; i < iterations; ++i)
                    counters.counter1.fetch_add(1, std::memory_order_relaxed);
            });
            
            threads.emplace_back([&]() {
                for (int i = 0; i < iterations; ++i)
                    counters.counter2.fetch_add(1, std::memory_order_relaxed);
            });
            
            for (auto& t : threads)
                t.join();
        }
    }
};

// 作業分割の最適化
class WorkPartitioning
{
public:
    // 悪い例：粒度が細かすぎる
    static long long badParallelSum(const std::vector<int>& data)
    {
        std::atomic<long long> result{0};
        std::vector<std::thread> threads;
        
        // 各要素を別々のスレッドで処理（オーバーヘッドが大きい）
        for (size_t i = 0; i < data.size() && i < 8; ++i)
        {
            threads.emplace_back([&result, &data, i]() {
                result.fetch_add(data[i], std::memory_order_relaxed);
            });
        }
        
        for (auto& t : threads)
            t.join();
        
        return result.load();
    }
    
    // 良い例：適切な粒度
    static long long goodParallelSum(const std::vector<int>& data)
    {
        const size_t numThreads = std::thread::hardware_concurrency();
        const size_t chunkSize = data.size() / numThreads;
        
        std::vector<std::future<long long>> futures;
        
        for (size_t i = 0; i < numThreads; ++i)
        {
            size_t start = i * chunkSize;
            size_t end = (i == numThreads - 1) ? data.size() : start + chunkSize;
            
            futures.push_back(std::async(std::launch::async, [&data, start, end]() {
                long long localSum = 0;
                for (size_t j = start; j < end; ++j)
                {
                    localSum += data[j];
                }
                return localSum;
            }));
        }
        
        long long totalSum = 0;
        for (auto& future : futures)
        {
            totalSum += future.get();
        }
        
        return totalSum;
    }
    
    void comparePartitioning()
    {
        std::cout << "\n=== 作業分割の比較 ===" << std::endl;
        
        std::vector<int> data(1000000, 1);  // 1の配列
        
        {
            PerformanceProfiler profiler("悪い分割（細かすぎる）");
            volatile auto result = badParallelSum(data);
        }
        
        {
            PerformanceProfiler profiler("良い分割（適切な粒度）");
            volatile auto result = goodParallelSum(data);
        }
    }
};

// メモリオーダリングの最適化
class MemoryOrderingOptimization
{
private:
    std::atomic<int> sharedCounter_{0};
    std::atomic<bool> flag_{false};
    
public:
    // 悪い例：強すぎる同期
    void badMemoryOrdering()
    {
        std::vector<std::thread> threads;
        
        for (int i = 0; i < 4; ++i)
        {
            threads.emplace_back([this]() {
                for (int j = 0; j < 100000; ++j)
                {
                    // sequential consistencyは不必要に強い
                    sharedCounter_.fetch_add(1);
                }
            });
        }
        
        for (auto& t : threads)
            t.join();
    }
    
    // 良い例：適切なメモリオーダリング
    void goodMemoryOrdering()
    {
        sharedCounter_.store(0);
        std::vector<std::thread> threads;
        
        for (int i = 0; i < 4; ++i)
        {
            threads.emplace_back([this]() {
                for (int j = 0; j < 100000; ++j)
                {
                    // relaxedで十分（単純なカウンタ）
                    sharedCounter_.fetch_add(1, std::memory_order_relaxed);
                }
            });
        }
        
        for (auto& t : threads)
            t.join();
    }
    
    void compareMemoryOrdering()
    {
        std::cout << "\n=== メモリオーダリングの比較 ===" << std::endl;
        
        {
            PerformanceProfiler profiler("強い同期（Sequential Consistency）");
            badMemoryOrdering();
        }
        
        {
            PerformanceProfiler profiler("適切な同期（Relaxed Ordering）");
            goodMemoryOrdering();
        }
    }
};

// ロック競合の最適化
class LockContentionOptimization
{
private:
    // 悪い例：単一ミューテックス
    class BadCounter
    {
    private:
        int count_;
        std::mutex mutex_;
        
    public:
        BadCounter() : count_(0) {}
        
        void increment()
        {
            std::lock_guard<std::mutex> lock(mutex_);
            ++count_;
        }
        
        int getValue() const
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return count_;
        }
    };
    
    // 良い例：スレッドローカルカウンタ
    class GoodCounter
    {
    private:
        static thread_local int localCount_;
        static std::atomic<int> globalCount_;
        
    public:
        static void increment()
        {
            ++localCount_;
            
            // 100回に1回だけグローバルカウンタに反映
            if (localCount_ % 100 == 0)
            {
                globalCount_.fetch_add(100, std::memory_order_relaxed);
                localCount_ = 0;
            }
        }
        
        static int getValue()
        {
            return globalCount_.load(std::memory_order_relaxed);
        }
        
        static void flush()
        {
            if (localCount_ > 0)
            {
                globalCount_.fetch_add(localCount_, std::memory_order_relaxed);
                localCount_ = 0;
            }
        }
    };
    
public:
    void compareLockContention()
    {
        std::cout << "\n=== ロック競合の比較 ===" << std::endl;
        
        const int iterations = 100000;
        
        // 悪い例
        {
            BadCounter counter;
            std::vector<std::thread> threads;
            
            PerformanceProfiler profiler("ロック競合あり（Mutex）");
            
            for (int i = 0; i < 4; ++i)
            {
                threads.emplace_back([&counter, iterations]() {
                    for (int j = 0; j < iterations; ++j)
                        counter.increment();
                });
            }
            
            for (auto& t : threads)
                t.join();
        }
        
        // 良い例
        {
            GoodCounter::globalCount_.store(0);
            std::vector<std::thread> threads;
            
            PerformanceProfiler profiler("ロック競合軽減（Thread-local）");
            
            for (int i = 0; i < 4; ++i)
            {
                threads.emplace_back([iterations]() {
                    for (int j = 0; j < iterations; ++j)
                        GoodCounter::increment();
                    GoodCounter::flush();
                });
            }
            
            for (auto& t : threads)
                t.join();
        }
    }
};

// スレッドローカルストレージの定義
thread_local int LockContentionOptimization::GoodCounter::localCount_ = 0;
std::atomic<int> LockContentionOptimization::GoodCounter::globalCount_{0};

// CPU親和性の最適化
class CPUAffinityDemo
{
public:
    void demonstrateCPUAffinity()
    {
        std::cout << "\n=== CPU親和性の重要性 ===" << std::endl;
        std::cout << "利用可能なCPUコア数: " << std::thread::hardware_concurrency() << std::endl;
        
        // Note: 実際のCPU親和性設定はプラットフォーム依存
        // LinuxならCPU_SET、WindowsならSetThreadAffinityMaskを使用
        
        std::vector<std::thread> threads;
        
        for (int i = 0; i < 4; ++i)
        {
            threads.emplace_back([i]() {
                // 高CPU使用率のタスクをシミュレート
                volatile long long sum = 0;
                for (long long j = 0; j < 10000000; ++j)
                {
                    sum += j;
                }
                
                std::cout << "スレッド " << i << " 完了（ID: " 
                          << std::this_thread::get_id() << "）" << std::endl;
            });
        }
        
        for (auto& t : threads)
            t.join();
    }
};

// データローカリティの最適化
class DataLocalityOptimization
{
private:
    struct Point3D
    {
        float x, y, z;
        
        Point3D(float px, float py, float pz) : x(px), y(py), z(pz) {}
    };
    
public:
    // 悪い例：Array of Structures (AoS)
    void badDataLayout(std::vector<Point3D>& points)
    {
        std::for_each(points.begin(), points.end(), [](Point3D& p) {
            p.x += 1.0f;  // キャッシュミスが多発
            p.y += 1.0f;
            p.z += 1.0f;
        });
    }
    
    // 良い例：Structure of Arrays (SoA)
    void goodDataLayout(std::vector<float>& x, std::vector<float>& y, std::vector<float>& z)
    {
        std::for_each(x.begin(), x.end(), [](float& val) { val += 1.0f; });
        std::for_each(y.begin(), y.end(), [](float& val) { val += 1.0f; });
        std::for_each(z.begin(), z.end(), [](float& val) { val += 1.0f; });
    }
    
    void compareDataLayout()
    {
        std::cout << "\n=== データレイアウトの比較 ===" << std::endl;
        
        const size_t numPoints = 1000000;
        
        // AoS版
        {
            std::vector<Point3D> points;
            points.reserve(numPoints);
            for (size_t i = 0; i < numPoints; ++i)
            {
                points.emplace_back(i * 0.1f, i * 0.2f, i * 0.3f);
            }
            
            PerformanceProfiler profiler("Array of Structures (AoS)");
            badDataLayout(points);
        }
        
        // SoA版
        {
            std::vector<float> x(numPoints), y(numPoints), z(numPoints);
            for (size_t i = 0; i < numPoints; ++i)
            {
                x[i] = i * 0.1f;
                y[i] = i * 0.2f;
                z[i] = i * 0.3f;
            }
            
            PerformanceProfiler profiler("Structure of Arrays (SoA)");
            goodDataLayout(x, y, z);
        }
    }
};

// ゲーム特化の最適化例
class GameOptimization
{
private:
    struct Entity
    {
        alignas(64) std::atomic<float> x, y, z;
        alignas(64) std::atomic<bool> active;
        std::atomic<int> health;
    };
    
public:
    void optimizedEntityUpdate()
    {
        std::cout << "\n=== ゲームエンティティ最適化 ===" << std::endl;
        
        const size_t numEntities = 100000;
        std::vector<Entity> entities(numEntities);
        
        // 初期化
        for (size_t i = 0; i < numEntities; ++i)
        {
            entities[i].x.store(i * 0.1f, std::memory_order_relaxed);
            entities[i].y.store(i * 0.2f, std::memory_order_relaxed);
            entities[i].z.store(i * 0.3f, std::memory_order_relaxed);
            entities[i].active.store(i % 10 != 0, std::memory_order_relaxed);
            entities[i].health.store(100, std::memory_order_relaxed);
        }
        
        const float deltaTime = 0.016f;
        const size_t numThreads = std::thread::hardware_concurrency();
        const size_t chunkSize = numEntities / numThreads;
        
        PerformanceProfiler profiler("並列エンティティ更新");
        
        std::vector<std::thread> threads;
        
        for (size_t t = 0; t < numThreads; ++t)
        {
            size_t start = t * chunkSize;
            size_t end = (t == numThreads - 1) ? numEntities : start + chunkSize;
            
            threads.emplace_back([&entities, start, end, deltaTime]() {
                for (size_t i = start; i < end; ++i)
                {
                    if (entities[i].active.load(std::memory_order_relaxed))
                    {
                        // relaxed orderingで高速化
                        float currentX = entities[i].x.load(std::memory_order_relaxed);
                        float currentY = entities[i].y.load(std::memory_order_relaxed);
                        
                        entities[i].x.store(currentX + deltaTime, std::memory_order_relaxed);
                        entities[i].y.store(currentY + deltaTime, std::memory_order_relaxed);
                    }
                }
            });
        }
        
        for (auto& thread : threads)
            thread.join();
        
        std::cout << "エンティティ更新完了: " << numEntities << " 個" << std::endl;
    }
};

int main()
{
    std::cout << "=== 並行プログラミングのパフォーマンス最適化 ===" << std::endl;
    
    // 1. False Sharing の影響
    FalseSharingDemo falseSharingDemo;
    falseSharingDemo.demonstrateFalseSharing();
    
    // 2. 作業分割の最適化
    WorkPartitioning partitioning;
    partitioning.comparePartitioning();
    
    // 3. メモリオーダリング最適化
    MemoryOrderingOptimization memoryOrdering;
    memoryOrdering.compareMemoryOrdering();
    
    // 4. ロック競合の軽減
    LockContentionOptimization lockContention;
    lockContention.compareLockContention();
    
    // 5. CPU親和性
    CPUAffinityDemo cpuAffinity;
    cpuAffinity.demonstrateCPUAffinity();
    
    // 6. データローカリティ
    DataLocalityOptimization dataLocality;
    dataLocality.compareDataLayout();
    
    // 7. ゲーム特化の最適化
    GameOptimization gameOpt;
    gameOpt.optimizedEntityUpdate();
    
    std::cout << "\n=== 最適化のまとめ ===" << std::endl;
    std::cout << "1. False Sharingを避けるためにpadding/alignmentを使用" << std::endl;
    std::cout << "2. 適切な粒度で作業を分割" << std::endl;
    std::cout << "3. 必要最小限のメモリオーダリングを選択" << std::endl;
    std::cout << "4. ロック競合を減らすためthread-localを活用" << std::endl;
    std::cout << "5. CPU親和性でコンテキストスイッチを削減" << std::endl;
    std::cout << "6. データレイアウトでキャッシュ効率を向上" << std::endl;
    
    return 0;
}

// まとめ：
// - False Sharingを避けるためキャッシュラインを意識
// - 適切な粒度で作業を分割してオーバーヘッドを最小化
// - メモリオーダリングは必要最小限に
// - ロック競合を減らすためthread-localストレージを活用
// - データレイアウトでキャッシュ効率を最大化