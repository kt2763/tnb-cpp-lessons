// アトミック操作
// ロックフリーな並行プログラミングを実現

#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <chrono>
#include <memory>

// 基本的なアトミック変数
class AtomicCounter
{
private:
    std::atomic<int> count_;
    
public:
    AtomicCounter() : count_(0) {}
    
    void increment()
    {
        ++count_;  // アトミックなインクリメント
        std::cout << "カウント: " << count_.load() << " (スレッド: " << std::this_thread::get_id() << ")" << std::endl;
    }
    
    void decrement()
    {
        --count_;  // アトミックなデクリメント
        std::cout << "カウント: " << count_.load() << " (スレッド: " << std::this_thread::get_id() << ")" << std::endl;
    }
    
    int getValue() const
    {
        return count_.load();
    }
    
    // Compare and Swap操作
    bool compareAndSet(int expected, int desired)
    {
        return count_.compare_exchange_weak(expected, desired);
    }
};

// ゲームでの使用例：プレイヤー統計
class PlayerStats
{
private:
    std::atomic<int> totalScore_;
    std::atomic<int> totalKills_;
    std::atomic<int> totalDeaths_;
    std::atomic<bool> isOnline_;
    
public:
    PlayerStats() : totalScore_(0), totalKills_(0), totalDeaths_(0), isOnline_(false) {}
    
    void addScore(int points)
    {
        totalScore_.fetch_add(points);
        std::cout << "スコア追加: +" << points << " (総スコア: " << totalScore_.load() << ")" << std::endl;
    }
    
    void addKill()
    {
        totalKills_.fetch_add(1);
        std::cout << "キル数: " << totalKills_.load() << std::endl;
    }
    
    void addDeath()
    {
        totalDeaths_.fetch_add(1);
        std::cout << "デス数: " << totalDeaths_.load() << std::endl;
    }
    
    void setOnlineStatus(bool online)
    {
        isOnline_.store(online);
        std::cout << "オンライン状態: " << (online ? "ON" : "OFF") << std::endl;
    }
    
    bool isOnline() const
    {
        return isOnline_.load();
    }
    
    void printStats() const
    {
        std::cout << "=== プレイヤー統計 ===" << std::endl;
        std::cout << "総スコア: " << totalScore_.load() << std::endl;
        std::cout << "キル数: " << totalKills_.load() << std::endl;
        std::cout << "デス数: " << totalDeaths_.load() << std::endl;
        std::cout << "K/D比: " << (totalDeaths_.load() > 0 ? 
                    static_cast<float>(totalKills_.load()) / totalDeaths_.load() : 
                    totalKills_.load()) << std::endl;
    }
};

// メモリオーダリングの例
class OrderingExample
{
private:
    std::atomic<int> data_;
    std::atomic<bool> ready_;
    
public:
    OrderingExample() : data_(0), ready_(false) {}
    
    // プロデューサー（書き込み側）
    void produce(int value)
    {
        data_.store(value, std::memory_order_relaxed);  // データを書き込み
        ready_.store(true, std::memory_order_release);  // 完了フラグを設定
        std::cout << "データ生成: " << value << std::endl;
    }
    
    // コンシューマー（読み込み側）
    bool consume(int& value)
    {
        if (ready_.load(std::memory_order_acquire))  // 完了フラグを確認
        {
            value = data_.load(std::memory_order_relaxed);  // データを読み込み
            std::cout << "データ消費: " << value << std::endl;
            return true;
        }
        return false;
    }
    
    void reset()
    {
        ready_.store(false, std::memory_order_relaxed);
    }
};

// ロックフリーなスタック
template<typename T>
class LockFreeStack
{
private:
    struct Node
    {
        T data;
        std::atomic<Node*> next;
        
        Node(const T& item) : data(item), next(nullptr) {}
    };
    
    std::atomic<Node*> head_;
    
public:
    LockFreeStack() : head_(nullptr) {}
    
    ~LockFreeStack()
    {
        while (Node* const oldHead = head_.load())
        {
            head_.store(oldHead->next);
            delete oldHead;
        }
    }
    
    void push(const T& data)
    {
        Node* const newNode = new Node(data);
        newNode->next = head_.load();
        
        // Compare-and-swapでアトミックに更新
        while (!head_.compare_exchange_weak(newNode->next, newNode));
        
        std::cout << "スタックにプッシュ: " << data << std::endl;
    }
    
    bool pop(T& result)
    {
        Node* oldHead = head_.load();
        
        while (oldHead && !head_.compare_exchange_weak(oldHead, oldHead->next));
        
        if (oldHead)
        {
            result = oldHead->data;
            std::cout << "スタックからポップ: " << result << std::endl;
            delete oldHead;
            return true;
        }
        
        return false;
    }
    
    bool empty() const
    {
        return head_.load() == nullptr;
    }
};

// ゲームのフレームレート管理
class FrameRateManager
{
private:
    std::atomic<double> currentFPS_;
    std::atomic<int> frameCount_;
    std::atomic<std::chrono::steady_clock::time_point> lastTime_;
    
public:
    FrameRateManager() : currentFPS_(0.0), frameCount_(0)
    {
        lastTime_.store(std::chrono::steady_clock::now());
    }
    
    void updateFrame()
    {
        frameCount_.fetch_add(1);
        
        auto currentTime = std::chrono::steady_clock::now();
        auto oldTime = lastTime_.load();
        
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - oldTime);
        
        if (elapsed.count() >= 1000)  // 1秒ごとに更新
        {
            if (lastTime_.compare_exchange_weak(oldTime, currentTime))
            {
                double fps = frameCount_.exchange(0) * 1000.0 / elapsed.count();
                currentFPS_.store(fps);
                std::cout << "FPS: " << fps << std::endl;
            }
        }
    }
    
    double getCurrentFPS() const
    {
        return currentFPS_.load();
    }
};

// アトミックポインタの使用例
class ResourceCache
{
private:
    struct Resource
    {
        std::string name;
        int loadTime;
        
        Resource(const std::string& n, int t) : name(n), loadTime(t) {}
    };
    
    std::atomic<Resource*> cachedResource_;
    
public:
    ResourceCache() : cachedResource_(nullptr) {}
    
    ~ResourceCache()
    {
        Resource* res = cachedResource_.load();
        delete res;
    }
    
    void loadResource(const std::string& name)
    {
        // 重い処理をシミュレート
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        Resource* newResource = new Resource(name, 100);
        Resource* oldResource = cachedResource_.exchange(newResource);
        
        std::cout << "リソース読み込み: " << name << std::endl;
        
        delete oldResource;  // 古いリソースを削除
    }
    
    std::string getCurrentResource() const
    {
        Resource* current = cachedResource_.load();
        return current ? current->name : "なし";
    }
};

// スピンロックの実装
class SpinLock
{
private:
    std::atomic<bool> locked_;
    
public:
    SpinLock() : locked_(false) {}
    
    void lock()
    {
        while (locked_.exchange(true, std::memory_order_acquire))
        {
            // スピン待機（CPUを消費）
            while (locked_.load(std::memory_order_relaxed))
            {
                std::this_thread::yield();  // 他のスレッドに実行権を譲る
            }
        }
    }
    
    void unlock()
    {
        locked_.store(false, std::memory_order_release);
    }
    
    bool tryLock()
    {
        return !locked_.exchange(true, std::memory_order_acquire);
    }
};

// RAII スピンロックガード
class SpinLockGuard
{
private:
    SpinLock& lock_;
    
public:
    explicit SpinLockGuard(SpinLock& lock) : lock_(lock)
    {
        lock_.lock();
    }
    
    ~SpinLockGuard()
    {
        lock_.unlock();
    }
    
    // コピー禁止
    SpinLockGuard(const SpinLockGuard&) = delete;
    SpinLockGuard& operator=(const SpinLockGuard&) = delete;
};

// 実演関数
void demonstrateBasicAtomic()
{
    std::cout << "\n=== 基本的なアトミック操作 ===" << std::endl;
    
    AtomicCounter counter;
    std::vector<std::thread> threads;
    
    // 複数スレッドでカウンターを操作
    for (int i = 0; i < 4; ++i)
    {
        threads.emplace_back([&counter, i]() {
            for (int j = 0; j < 3; ++j)
            {
                if (j % 2 == 0)
                    counter.increment();
                else
                    counter.decrement();
                
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        });
    }
    
    for (auto& t : threads)
    {
        t.join();
    }
    
    std::cout << "最終カウント: " << counter.getValue() << std::endl;
}

void demonstratePlayerStats()
{
    std::cout << "\n=== プレイヤー統計 ===" << std::endl;
    
    PlayerStats stats;
    std::vector<std::thread> gameThreads;
    
    // ゲームプレイをシミュレート
    for (int i = 0; i < 3; ++i)
    {
        gameThreads.emplace_back([&stats, i]() {
            stats.setOnlineStatus(true);
            
            for (int j = 0; j < 5; ++j)
            {
                stats.addScore(100 + i * 10);
                
                if (j % 3 == 0)
                    stats.addKill();
                if (j % 4 == 0)
                    stats.addDeath();
                
                std::this_thread::sleep_for(std::chrono::milliseconds(150));
            }
        });
    }
    
    for (auto& t : gameThreads)
    {
        t.join();
    }
    
    stats.printStats();
}

void demonstrateLockFreeStack()
{
    std::cout << "\n=== ロックフリースタック ===" << std::endl;
    
    LockFreeStack<int> stack;
    std::vector<std::thread> threads;
    
    // プッシュスレッド
    for (int i = 0; i < 2; ++i)
    {
        threads.emplace_back([&stack, i]() {
            for (int j = 0; j < 5; ++j)
            {
                stack.push(i * 10 + j);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        });
    }
    
    // ポップスレッド
    threads.emplace_back([&stack]() {
        int value;
        int count = 0;
        
        while (count < 10)
        {
            if (stack.pop(value))
            {
                count++;
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }
    });
    
    for (auto& t : threads)
    {
        t.join();
    }
}

int main()
{
    std::cout << "=== アトミック操作 ===" << std::endl;
    
    // 1. 基本的なアトミック操作
    demonstrateBasicAtomic();
    
    // 2. プレイヤー統計
    demonstratePlayerStats();
    
    // 3. メモリオーダリング
    std::cout << "\n=== メモリオーダリング ===" << std::endl;
    OrderingExample ordering;
    
    std::thread producer([&ordering]() {
        for (int i = 1; i <= 5; ++i)
        {
            ordering.produce(i * 100);
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            ordering.reset();
        }
    });
    
    std::thread consumer([&ordering]() {
        int value;
        for (int i = 0; i < 5; ++i)
        {
            while (!ordering.consume(value))
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    });
    
    producer.join();
    consumer.join();
    
    // 4. ロックフリースタック
    demonstrateLockFreeStack();
    
    // 5. フレームレート管理
    std::cout << "\n=== フレームレート管理 ===" << std::endl;
    FrameRateManager framerateManager;
    
    std::thread gameLoop([&framerateManager]() {
        for (int i = 0; i < 200; ++i)  // 200フレーム
        {
            framerateManager.updateFrame();
            std::this_thread::sleep_for(std::chrono::milliseconds(16));  // 約60FPS
        }
    });
    
    gameLoop.join();
    
    return 0;
}

// まとめ：
// - std::atomicでロックフリーな操作を実現
// - メモリオーダリングで細かい制御が可能
// - Compare-and-Swapでアトミックな更新
// - ロックフリーデータ構造で高性能を実現
// - ゲーム開発では統計情報やリソース管理に活用