// 並行プログラムのデバッグとテスト
// マルチスレッドプログラムの検証と問題発見の技法

#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <chrono>
#include <random>
#include <cassert>
#include <condition_variable>

// デバッグ用のログ出力
class ThreadSafeLogger
{
private:
    std::mutex logMutex_;
    
public:
    template<typename... Args>
    void log(Args&&... args)
    {
        std::lock_guard<std::mutex> lock(logMutex_);
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        std::cout << "[" << std::this_thread::get_id() << "] ";
        (std::cout << ... << args);
        std::cout << std::endl;
    }
};

ThreadSafeLogger logger;

// レースコンディションの例とデバッグ
class RaceConditionExample
{
private:
    int sharedCounter_;
    std::mutex counterMutex_;
    std::atomic<int> atomicCounter_;
    
public:
    RaceConditionExample() : sharedCounter_(0), atomicCounter_(0) {}
    
    // 悪い例：レースコンディションあり
    void unsafeIncrement()
    {
        sharedCounter_++;  // 危険：アトミックでない
        logger.log("Unsafe increment: ", sharedCounter_);
    }
    
    // 良い例：ミューテックスで保護
    void safeIncrement()
    {
        std::lock_guard<std::mutex> lock(counterMutex_);
        sharedCounter_++;
        logger.log("Safe increment: ", sharedCounter_);
    }
    
    // 良い例：アトミック操作
    void atomicIncrement()
    {
        int newValue = atomicCounter_.fetch_add(1) + 1;
        logger.log("Atomic increment: ", newValue);
    }
    
    void demonstrateRaceCondition()
    {
        std::cout << "\n=== レースコンディションのデモ ===" << std::endl;
        
        // レースコンディションを意図的に発生させる
        std::vector<std::thread> threads;
        
        for (int i = 0; i < 3; ++i)
        {
            threads.emplace_back([this, i]() {
                for (int j = 0; j < 5; ++j)
                {
                    if (i == 0)
                        unsafeIncrement();  // 危険
                    else if (i == 1)
                        safeIncrement();    // 安全
                    else
                        atomicIncrement();  // 安全
                    
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            });
        }
        
        for (auto& t : threads)
            t.join();
    }
    
    int getCounterValue() const
    {
        std::lock_guard<std::mutex> lock(counterMutex_);
        return sharedCounter_;
    }
    
    int getAtomicCounterValue() const
    {
        return atomicCounter_.load();
    }
};

// デッドロックの検出と回避
class DeadlockExample
{
private:
    std::mutex mutex1_, mutex2_;
    int resource1_, resource2_;
    
public:
    DeadlockExample() : resource1_(0), resource2_(0) {}
    
    // 悪い例：デッドロックの可能性
    void potentialDeadlock1()
    {
        logger.log("Thread1: ミューテックス1を取得試行");
        std::lock_guard<std::mutex> lock1(mutex1_);
        logger.log("Thread1: ミューテックス1を取得");
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        logger.log("Thread1: ミューテックス2を取得試行");
        std::lock_guard<std::mutex> lock2(mutex2_);
        logger.log("Thread1: ミューテックス2を取得");
        
        resource1_++;
        resource2_++;
        logger.log("Thread1: リソース更新完了");
    }
    
    void potentialDeadlock2()
    {
        logger.log("Thread2: ミューテックス2を取得試行");
        std::lock_guard<std::mutex> lock2(mutex2_);
        logger.log("Thread2: ミューテックス2を取得");
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        logger.log("Thread2: ミューテックス1を取得試行");
        std::lock_guard<std::mutex> lock1(mutex1_);
        logger.log("Thread2: ミューテックス1を取得");
        
        resource1_++;
        resource2_++;
        logger.log("Thread2: リソース更新完了");
    }
    
    // 良い例：デッドロック回避
    void avoidDeadlock1()
    {
        logger.log("Thread1: std::lockでデッドロック回避");
        std::lock(mutex1_, mutex2_);
        std::lock_guard<std::mutex> lock1(mutex1_, std::adopt_lock);
        std::lock_guard<std::mutex> lock2(mutex2_, std::adopt_lock);
        
        resource1_++;
        resource2_++;
        logger.log("Thread1: 安全にリソース更新完了");
    }
    
    void avoidDeadlock2()
    {
        logger.log("Thread2: std::lockでデッドロック回避");
        std::lock(mutex1_, mutex2_);
        std::lock_guard<std::mutex> lock1(mutex1_, std::adopt_lock);
        std::lock_guard<std::mutex> lock2(mutex2_, std::adopt_lock);
        
        resource1_++;
        resource2_++;
        logger.log("Thread2: 安全にリソース更新完了");
    }
    
    void demonstrateDeadlock()
    {
        std::cout << "\n=== デッドロック回避のデモ ===" << std::endl;
        
        std::thread t1(&DeadlockExample::avoidDeadlock1, this);
        std::thread t2(&DeadlockExample::avoidDeadlock2, this);
        
        t1.join();
        t2.join();
        
        logger.log("両方のスレッドが正常完了");
    }
};

// 並行プログラムのテストフレームワーク
class ConcurrencyTester
{
public:
    // ストレステスト
    template<typename Func>
    static bool stressTest(Func testFunction, int iterations, int numThreads)
    {
        std::vector<std::thread> threads;
        std::atomic<bool> testPassed{true};
        std::atomic<int> completedThreads{0};
        
        logger.log("ストレステスト開始: ", iterations, " 回 x ", numThreads, " スレッド");
        
        for (int i = 0; i < numThreads; ++i)
        {
            threads.emplace_back([&, i]() {
                try
                {
                    for (int j = 0; j < iterations; ++j)
                    {
                        if (!testFunction(i, j))
                        {
                            testPassed.store(false);
                            logger.log("テスト失敗: スレッド ", i, ", 反復 ", j);
                            return;
                        }
                    }
                    
                    completedThreads.fetch_add(1);
                    logger.log("スレッド ", i, " 完了");
                }
                catch (const std::exception& e)
                {
                    testPassed.store(false);
                    logger.log("例外発生: スレッド ", i, " - ", e.what());
                }
            });
        }
        
        for (auto& t : threads)
            t.join();
        
        bool passed = testPassed.load() && (completedThreads.load() == numThreads);
        logger.log("ストレステスト結果: ", passed ? "成功" : "失敗");
        return passed;
    }
    
    // 一意性テスト（重複なし）
    template<typename Container>
    static bool uniquenessTest(Container& container)
    {
        std::sort(container.begin(), container.end());
        auto it = std::unique(container.begin(), container.end());
        
        bool isUnique = (it == container.end());
        logger.log("一意性テスト: ", isUnique ? "成功" : "失敗", 
                  " (重複検出数: ", std::distance(it, container.end()), ")");
        
        return isUnique;
    }
    
    // 順序性テスト
    template<typename Container>
    static bool orderingTest(const Container& container)
    {
        bool isOrdered = std::is_sorted(container.begin(), container.end());
        logger.log("順序性テスト: ", isOrdered ? "成功" : "失敗");
        return isOrdered;
    }
};

// テスト用のスレッドセーフなデータ構造
class ThreadSafeCounter
{
private:
    std::atomic<int> counter_;
    std::vector<int> incrementHistory_;
    std::mutex historyMutex_;
    
public:
    ThreadSafeCounter() : counter_(0) {}
    
    int increment()
    {
        int newValue = counter_.fetch_add(1) + 1;
        
        {
            std::lock_guard<std::mutex> lock(historyMutex_);
            incrementHistory_.push_back(newValue);
        }
        
        return newValue;
    }
    
    int getValue() const
    {
        return counter_.load();
    }
    
    std::vector<int> getHistory() const
    {
        std::lock_guard<std::mutex> lock(historyMutex_);
        return incrementHistory_;
    }
    
    void reset()
    {
        counter_.store(0);
        std::lock_guard<std::mutex> lock(historyMutex_);
        incrementHistory_.clear();
    }
};

// パフォーマンス回帰テスト
class PerformanceRegression
{
public:
    template<typename Func>
    static std::chrono::microseconds measureExecutionTime(Func&& func)
    {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        
        return std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    }
    
    static void benchmarkComparison()
    {
        std::cout << "\n=== パフォーマンス回帰テスト ===" << std::endl;
        
        const int iterations = 100000;
        std::atomic<int> atomicCounter{0};
        int normalCounter = 0;
        std::mutex counterMutex;
        
        // アトミック操作のベンチマーク
        auto atomicTime = measureExecutionTime([&]() {
            std::vector<std::thread> threads;
            
            for (int i = 0; i < 4; ++i)
            {
                threads.emplace_back([&]() {
                    for (int j = 0; j < iterations; ++j)
                    {
                        atomicCounter.fetch_add(1, std::memory_order_relaxed);
                    }
                });
            }
            
            for (auto& t : threads)
                t.join();
        });
        
        // ミューテックス操作のベンチマーク
        auto mutexTime = measureExecutionTime([&]() {
            std::vector<std::thread> threads;
            
            for (int i = 0; i < 4; ++i)
            {
                threads.emplace_back([&]() {
                    for (int j = 0; j < iterations; ++j)
                    {
                        std::lock_guard<std::mutex> lock(counterMutex);
                        ++normalCounter;
                    }
                });
            }
            
            for (auto& t : threads)
                t.join();
        });
        
        logger.log("アトミック操作: ", atomicTime.count(), " μs");
        logger.log("ミューテックス操作: ", mutexTime.count(), " μs");
        logger.log("性能比: ", static_cast<double>(mutexTime.count()) / atomicTime.count(), ":1");
    }
};

// メモリリークの検出
class MemoryLeakDetection
{
private:
    static std::atomic<int> allocationCount_;
    static std::atomic<int> deallocationCount_;
    
public:
    struct TrackingDeleter
    {
        template<typename T>
        void operator()(T* ptr)
        {
            deallocationCount_.fetch_add(1);
            delete ptr;
        }
    };
    
    template<typename T, typename... Args>
    static std::unique_ptr<T, TrackingDeleter> make_tracked(Args&&... args)
    {
        allocationCount_.fetch_add(1);
        return std::unique_ptr<T, TrackingDeleter>(new T(std::forward<Args>(args)...));
    }
    
    static void printStatistics()
    {
        int allocations = allocationCount_.load();
        int deallocations = deallocationCount_.load();
        
        logger.log("メモリ統計:");
        logger.log("  割り当て: ", allocations);
        logger.log("  解放: ", deallocations);
        logger.log("  リーク疑い: ", allocations - deallocations);
    }
    
    static void resetCounters()
    {
        allocationCount_.store(0);
        deallocationCount_.store(0);
    }
};

std::atomic<int> MemoryLeakDetection::allocationCount_{0};
std::atomic<int> MemoryLeakDetection::deallocationCount_{0};

int main()
{
    std::cout << "=== 並行プログラムのデバッグとテスト ===" << std::endl;
    
    // 1. レースコンディションの検出
    RaceConditionExample raceExample;
    raceExample.demonstrateRaceCondition();
    
    std::cout << "最終カウンタ値（非同期）: " << raceExample.getCounterValue() << std::endl;
    std::cout << "最終カウンタ値（アトミック）: " << raceExample.getAtomicCounterValue() << std::endl;
    
    // 2. デッドロックの回避
    DeadlockExample deadlockExample;
    deadlockExample.demonstrateDeadlock();
    
    // 3. 並行テストの実行
    std::cout << "\n=== 並行テスト実行 ===" << std::endl;
    ThreadSafeCounter testCounter;
    
    // カウンターのテスト
    bool testResult = ConcurrencyTester::stressTest(
        [&testCounter](int threadId, int iteration) -> bool {
            int value = testCounter.increment();
            return value > 0;  // カウンターは常に正の値
        }, 
        100, 4
    );
    
    logger.log("カウンターテスト結果: ", testResult ? "成功" : "失敗");
    logger.log("最終カウンター値: ", testCounter.getValue());
    
    // 履歴の一意性テスト
    auto history = testCounter.getHistory();
    bool uniqueTest = ConcurrencyTester::uniquenessTest(history);
    
    // 4. パフォーマンス回帰テスト
    PerformanceRegression::benchmarkComparison();
    
    // 5. メモリリーク検出
    std::cout << "\n=== メモリリーク検出 ===" << std::endl;
    MemoryLeakDetection::resetCounters();
    
    {
        auto ptr1 = MemoryLeakDetection::make_tracked<int>(42);
        auto ptr2 = MemoryLeakDetection::make_tracked<std::string>("test");
        // スコープを抜ける時に自動的に解放される
    }
    
    MemoryLeakDetection::printStatistics();
    
    // 6. デバッグのベストプラクティス
    std::cout << "\n=== デバッグのベストプラクティス ===" << std::endl;
    std::cout << "1. 段階的な検証：単体テスト → 統合テスト → ストレステスト" << std::endl;
    std::cout << "2. ログ出力でスレッドの動作を追跡" << std::endl;
    std::cout << "3. アサーションで不変条件をチェック" << std::endl;
    std::cout << "4. メモリ使用量とリーク検出を監視" << std::endl;
    std::cout << "5. パフォーマンス回帰をベンチマークで検出" << std::endl;
    
    return 0;
}

// まとめ：
// - スレッドセーフなログ出力でデバッグ情報を収集
// - レースコンディションとデッドロックの検出と回避
// - ストレステストで並行性の問題を発見
// - パフォーマンス回帰テストで性能低下を監視
// - メモリリーク検出で資源管理を確認