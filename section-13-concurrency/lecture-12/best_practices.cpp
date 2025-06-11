// 並行プログラミングのベストプラクティス
// 保守性、安全性、性能を考慮した実践的な指針

#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <future>
#include <vector>
#include <queue>
#include <condition_variable>
#include <chrono>
#include <functional>
#include <memory>
#include <exception>

// 設計原則1: 最小権限の原則
namespace minimal_privilege 
{
    // 悪い例：すべてmutableで広すぎるアクセス
    class BadGameState
    {
    public:
        mutable std::mutex mutex_;
        int playerScore_;
        int playerHealth_;
        std::string playerName_;
        
        // すべてのメンバーが変更可能
        void updateAnything(int score, int health, const std::string& name)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            playerScore_ = score;
            playerHealth_ = health;
            playerName_ = name;
        }
    };
    
    // 良い例：最小限のアクセス権限
    class GoodGameState
    {
    private:
        mutable std::shared_mutex stateMutex_;
        std::atomic<int> playerScore_;
        std::atomic<int> playerHealth_;
        std::string playerName_;
        mutable std::mutex nameMutex_;
        
    public:
        GoodGameState(const std::string& name) : playerScore_(0), playerHealth_(100), playerName_(name) {}
        
        // 読み取り専用操作
        int getScore() const { return playerScore_.load(std::memory_order_relaxed); }
        int getHealth() const { return playerHealth_.load(std::memory_order_relaxed); }
        
        std::string getName() const
        {
            std::shared_lock<std::shared_mutex> lock(nameMutex_);
            return playerName_;
        }
        
        // 特定の操作のみ
        void addScore(int points) { playerScore_.fetch_add(points, std::memory_order_relaxed); }
        void takeDamage(int damage) 
        { 
            int current = playerHealth_.load();
            int newHealth = std::max(0, current - damage);
            playerHealth_.store(newHealth, std::memory_order_relaxed);
        }
        
        void setName(const std::string& newName)
        {
            std::unique_lock<std::shared_mutex> lock(nameMutex_);
            playerName_ = newName;
        }
    };
}

// 設計原則2: RAIIによるリソース管理
namespace raii_principles
{
    // 良い例：RAIIベースのスレッドプール
    class ThreadPool
    {
    private:
        std::vector<std::thread> workers_;
        std::queue<std::function<void()>> tasks_;
        std::mutex queueMutex_;
        std::condition_variable condition_;
        bool stop_;
        
    public:
        explicit ThreadPool(size_t numThreads) : stop_(false)
        {
            for (size_t i = 0; i < numThreads; ++i)
            {
                workers_.emplace_back([this] {
                    while (true)
                    {
                        std::function<void()> task;
                        
                        {
                            std::unique_lock<std::mutex> lock(queueMutex_);
                            condition_.wait(lock, [this] { return stop_ || !tasks_.empty(); });
                            
                            if (stop_ && tasks_.empty())
                                return;
                            
                            task = std::move(tasks_.front());
                            tasks_.pop();
                        }
                        
                        task();
                    }
                });
            }
        }
        
        ~ThreadPool()
        {
            {
                std::unique_lock<std::mutex> lock(queueMutex_);
                stop_ = true;
            }
            
            condition_.notify_all();
            
            for (std::thread& worker : workers_)
            {
                worker.join();
            }
        }
        
        template<class F>
        void enqueue(F&& f)
        {
            {
                std::unique_lock<std::mutex> lock(queueMutex_);
                if (stop_)
                    throw std::runtime_error("ThreadPool停止済み");
                
                tasks_.emplace(std::forward<F>(f));
            }
            condition_.notify_one();
        }
        
        // コピー・ムーブ禁止
        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;
    };
    
    // RAII ロックガード
    class ScopedLockGuard
    {
    private:
        std::function<void()> unlockFunction_;
        
    public:
        template<typename LockFunc, typename UnlockFunc>
        ScopedLockGuard(LockFunc&& lock, UnlockFunc&& unlock)
            : unlockFunction_(std::forward<UnlockFunc>(unlock))
        {
            lock();
        }
        
        ~ScopedLockGuard()
        {
            if (unlockFunction_)
                unlockFunction_();
        }
        
        // コピー禁止、ムーブ可能
        ScopedLockGuard(const ScopedLockGuard&) = delete;
        ScopedLockGuard& operator=(const ScopedLockGuard&) = delete;
        
        ScopedLockGuard(ScopedLockGuard&& other) noexcept
            : unlockFunction_(std::move(other.unlockFunction_))
        {
            other.unlockFunction_ = nullptr;
        }
    };
}

// 設計原則3: 例外安全性
namespace exception_safety
{
    class SafeGameInventory
    {
    private:
        std::vector<std::string> items_;
        std::atomic<int> totalValue_;
        mutable std::mutex itemsMutex_;
        
    public:
        SafeGameInventory() : totalValue_(0) {}
        
        // 強い例外安全性保証
        void addItem(const std::string& item, int value)
        {
            // 一時的なコピーを作成
            std::vector<std::string> tempItems;
            
            {
                std::lock_guard<std::mutex> lock(itemsMutex_);
                tempItems = items_;  // 例外が発生する可能性
            }
            
            // ローカル操作（例外安全）
            tempItems.push_back(item);
            int newTotalValue = totalValue_.load() + value;
            
            // アトミックに状態を更新
            {
                std::lock_guard<std::mutex> lock(itemsMutex_);
                items_ = std::move(tempItems);
            }
            totalValue_.store(newTotalValue);
            
            std::cout << "アイテム追加: " << item << " (価値: " << value << ")" << std::endl;
        }
        
        // nothrow保証
        size_t getItemCount() const noexcept
        {
            std::lock_guard<std::mutex> lock(itemsMutex_);
            return items_.size();
        }
        
        int getTotalValue() const noexcept
        {
            return totalValue_.load();
        }
        
        // 基本保証
        std::vector<std::string> getAllItems() const
        {
            std::lock_guard<std::mutex> lock(itemsMutex_);
            return items_;  // コピーを返す
        }
    };
}

// 設計原則4: 適切な同期プリミティブの選択
namespace synchronization_choice
{
    // 高頻度読み取り、低頻度書き込み
    class GameConfiguration
    {
    private:
        mutable std::shared_mutex configMutex_;
        std::map<std::string, std::string> settings_;
        
    public:
        // 読み取り（共有ロック）
        std::string getSetting(const std::string& key) const
        {
            std::shared_lock<std::shared_mutex> lock(configMutex_);
            auto it = settings_.find(key);
            return (it != settings_.end()) ? it->second : "";
        }
        
        // 書き込み（排他ロック）
        void setSetting(const std::string& key, const std::string& value)
        {
            std::unique_lock<std::shared_mutex> lock(configMutex_);
            settings_[key] = value;
        }
    };
    
    // 単純なカウンター（アトミック）
    class PerformanceMetrics
    {
    private:
        std::atomic<uint64_t> frameCount_{0};
        std::atomic<uint64_t> drawCalls_{0};
        std::atomic<double> frameTime_{0.0};
        
    public:
        void recordFrame(double time, uint32_t drawCallCount)
        {
            frameCount_.fetch_add(1, std::memory_order_relaxed);
            drawCalls_.fetch_add(drawCallCount, std::memory_order_relaxed);
            frameTime_.store(time, std::memory_order_relaxed);
        }
        
        uint64_t getFrameCount() const { return frameCount_.load(std::memory_order_relaxed); }
        uint64_t getTotalDrawCalls() const { return drawCalls_.load(std::memory_order_relaxed); }
        double getLastFrameTime() const { return frameTime_.load(std::memory_order_relaxed); }
    };
}

// 設計原則5: デッドロックの回避
namespace deadlock_avoidance
{
    class ResourceManager
    {
    private:
        struct Resource
        {
            std::string name;
            bool inUse;
            std::mutex mutex;
            
            Resource(const std::string& n) : name(n), inUse(false) {}
        };
        
        std::array<std::unique_ptr<Resource>, 4> resources_;
        
    public:
        ResourceManager()
        {
            for (int i = 0; i < 4; ++i)
            {
                resources_[i] = std::make_unique<Resource>("Resource" + std::to_string(i));
            }
        }
        
        // 複数リソースの安全な取得（デッドロック回避）
        template<typename... ResourceIds>
        void useResources(ResourceIds... ids)
        {
            // リソースIDを配列に収集
            std::array<int, sizeof...(ids)> resourceIds = {ids...};
            
            // ソートしてロック順序を統一
            std::sort(resourceIds.begin(), resourceIds.end());
            
            // 順序通りにロックを取得
            std::vector<std::unique_lock<std::mutex>> locks;
            for (int id : resourceIds)
            {
                if (id >= 0 && id < 4)
                {
                    locks.emplace_back(resources_[id]->mutex);
                    resources_[id]->inUse = true;
                    std::cout << "リソース " << id << " をロック" << std::endl;
                }
            }
            
            // 作業をシミュレート
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            // RAIIによりスコープを抜ける時に自動的にアンロック
            for (int id : resourceIds)
            {
                if (id >= 0 && id < 4)
                {
                    resources_[id]->inUse = false;
                    std::cout << "リソース " << id << " をアンロック" << std::endl;
                }
            }
        }
    };
}

// 設計原則6: スケーラブルな設計
namespace scalable_design
{
    // ワークスティーリング対応のタスクキュー
    class ScalableTaskQueue
    {
    private:
        struct alignas(64) LocalQueue  // False Sharing回避
        {
            std::queue<std::function<void()>> tasks;
            std::mutex mutex;
        };
        
        std::vector<std::unique_ptr<LocalQueue>> localQueues_;
        std::atomic<size_t> nextQueue_{0};
        
    public:
        explicit ScalableTaskQueue(size_t numQueues = std::thread::hardware_concurrency())
        {
            localQueues_.resize(numQueues);
            for (auto& queue : localQueues_)
            {
                queue = std::make_unique<LocalQueue>();
            }
        }
        
        void enqueue(std::function<void()> task)
        {
            // ラウンドロビンで負荷分散
            size_t queueIndex = nextQueue_.fetch_add(1) % localQueues_.size();
            
            std::lock_guard<std::mutex> lock(localQueues_[queueIndex]->mutex);
            localQueues_[queueIndex]->tasks.push(std::move(task));
        }
        
        bool tryDequeue(std::function<void()>& task, size_t preferredQueue)
        {
            // まず優先キューから試行
            if (preferredQueue < localQueues_.size())
            {
                std::lock_guard<std::mutex> lock(localQueues_[preferredQueue]->mutex);
                if (!localQueues_[preferredQueue]->tasks.empty())
                {
                    task = std::move(localQueues_[preferredQueue]->tasks.front());
                    localQueues_[preferredQueue]->tasks.pop();
                    return true;
                }
            }
            
            // 他のキューからワークスティーリング
            for (size_t i = 0; i < localQueues_.size(); ++i)
            {
                if (i == preferredQueue) continue;
                
                std::lock_guard<std::mutex> lock(localQueues_[i]->mutex);
                if (!localQueues_[i]->tasks.empty())
                {
                    task = std::move(localQueues_[i]->tasks.front());
                    localQueues_[i]->tasks.pop();
                    return true;
                }
            }
            
            return false;
        }
    };
}

// 実践的なゲームシステムの例
class GameSystemExample
{
private:
    using namespace raii_principles;
    using namespace exception_safety;
    using namespace synchronization_choice;
    
    ThreadPool workerPool_;
    SafeGameInventory inventory_;
    GameConfiguration config_;
    PerformanceMetrics metrics_;
    
public:
    GameSystemExample() : workerPool_(std::thread::hardware_concurrency()) 
    {
        // 初期設定
        config_.setSetting("difficulty", "normal");
        config_.setSetting("resolution", "1920x1080");
    }
    
    void simulateGameFrame()
    {
        auto frameStart = std::chrono::high_resolution_clock::now();
        
        // 並列タスクを投入
        std::vector<std::future<void>> tasks;
        
        // AI処理
        tasks.push_back(std::async(std::launch::async, [this]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            // AI処理をシミュレート
        }));
        
        // 物理演算
        tasks.push_back(std::async(std::launch::async, [this]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(3));
            // 物理演算をシミュレート
        }));
        
        // レンダリング
        tasks.push_back(std::async(std::launch::async, [this]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(8));
            // レンダリングをシミュレート
        }));
        
        // すべてのタスクの完了を待つ
        for (auto& task : tasks)
        {
            task.wait();
        }
        
        auto frameEnd = std::chrono::high_resolution_clock::now();
        double frameTime = std::chrono::duration<double, std::milli>(frameEnd - frameStart).count();
        
        metrics_.recordFrame(frameTime, 150);  // 150ドローコール
    }
    
    void printStatistics() const
    {
        std::cout << "\n=== ゲーム統計 ===" << std::endl;
        std::cout << "フレーム数: " << metrics_.getFrameCount() << std::endl;
        std::cout << "総ドローコール: " << metrics_.getTotalDrawCalls() << std::endl;
        std::cout << "最終フレーム時間: " << metrics_.getLastFrameTime() << "ms" << std::endl;
        std::cout << "インベントリアイテム数: " << inventory_.getItemCount() << std::endl;
        std::cout << "総アイテム価値: " << inventory_.getTotalValue() << std::endl;
        std::cout << "解像度設定: " << config_.getSetting("resolution") << std::endl;
    }
    
    void addRandomItem()
    {
        static int itemCounter = 0;
        std::string itemName = "アイテム" + std::to_string(++itemCounter);
        int value = 50 + (itemCounter % 100);
        
        try
        {
            inventory_.addItem(itemName, value);
        }
        catch (const std::exception& e)
        {
            std::cerr << "アイテム追加エラー: " << e.what() << std::endl;
        }
    }
};

int main()
{
    std::cout << "=== 並行プログラミングのベストプラクティス ===" << std::endl;
    
    // 1. 最小権限の原則のデモ
    std::cout << "\n--- 最小権限の原則 ---" << std::endl;
    minimal_privilege::GoodGameState gameState("プレイヤー1");
    
    // 複数スレッドで安全に操作
    std::vector<std::thread> threads;
    
    threads.emplace_back([&gameState]() {
        for (int i = 0; i < 5; ++i)
        {
            gameState.addScore(100);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    });
    
    threads.emplace_back([&gameState]() {
        for (int i = 0; i < 3; ++i)
        {
            gameState.takeDamage(10);
            std::this_thread::sleep_for(std::chrono::milliseconds(80));
        }
    });
    
    for (auto& t : threads)
        t.join();
    
    std::cout << "プレイヤー: " << gameState.getName() 
              << ", スコア: " << gameState.getScore()
              << ", 体力: " << gameState.getHealth() << std::endl;
    
    // 2. デッドロック回避のデモ
    std::cout << "\n--- デッドロック回避 ---" << std::endl;
    deadlock_avoidance::ResourceManager resourceMgr;
    
    std::thread t1([&resourceMgr]() {
        resourceMgr.useResources(0, 1, 2);
    });
    
    std::thread t2([&resourceMgr]() {
        resourceMgr.useResources(2, 1, 3);  // 異なる順序でも安全
    });
    
    t1.join();
    t2.join();
    
    // 3. スケーラブル設計のデモ
    std::cout << "\n--- スケーラブルタスクキュー ---" << std::endl;
    scalable_design::ScalableTaskQueue taskQueue;
    
    // タスクを投入
    for (int i = 0; i < 10; ++i)
    {
        taskQueue.enqueue([i]() {
            std::cout << "タスク " << i << " 実行" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        });
    }
    
    // ワーカースレッドで処理
    std::vector<std::thread> workers;
    for (size_t i = 0; i < 3; ++i)
    {
        workers.emplace_back([&taskQueue, i]() {
            std::function<void()> task;
            int processed = 0;
            
            while (processed < 4)  // 各ワーカーが4つのタスクを処理
            {
                if (taskQueue.tryDequeue(task, i))
                {
                    task();
                    processed++;
                }
                else
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
        });
    }
    
    for (auto& worker : workers)
        worker.join();
    
    // 4. 実践的なゲームシステム
    std::cout << "\n--- 実践的なゲームシステム ---" << std::endl;
    GameSystemExample gameSystem;
    
    // ゲームループをシミュレート
    for (int frame = 0; frame < 10; ++frame)
    {
        gameSystem.simulateGameFrame();
        
        // ランダムでアイテム追加
        if (frame % 3 == 0)
        {
            gameSystem.addRandomItem();
        }
    }
    
    gameSystem.printStatistics();
    
    // 5. ベストプラクティスのまとめ
    std::cout << "\n=== ベストプラクティスまとめ ===" << std::endl;
    std::cout << "1. 最小権限の原則：必要最小限のアクセス権限" << std::endl;
    std::cout << "2. RAIIによるリソース管理：自動的なクリーンアップ" << std::endl;
    std::cout << "3. 例外安全性：強い保証と基本保証の適切な使い分け" << std::endl;
    std::cout << "4. 適切な同期プリミティブ：用途に応じた選択" << std::endl;
    std::cout << "5. デッドロック回避：一貫したロック順序" << std::endl;
    std::cout << "6. スケーラブル設計：コア数に応じた負荷分散" << std::endl;
    std::cout << "7. パフォーマンス考慮：False Sharing回避とキャッシュ効率" << std::endl;
    
    return 0;
}

// まとめ：
// - 最小権限の原則で安全性を確保
// - RAIIで確実なリソース管理
// - 例外安全性で堅牢なコード
// - 適切な同期プリミティブで効率化
// - デッドロック回避で安定性確保
// - スケーラブル設計で性能向上