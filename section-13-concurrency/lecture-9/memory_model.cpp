// C++メモリモデル
// メモリオーダリングと同期の詳細な制御

#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <chrono>
#include <cassert>

// 基本的なメモリオーダリングの例
class BasicMemoryOrdering
{
private:
    std::atomic<int> data_;
    std::atomic<bool> ready_;
    
public:
    BasicMemoryOrdering() : data_(0), ready_(false) {}
    
    // relaxed ordering - 最も弱い保証
    void relaxed_example()
    {
        std::cout << "\n=== Relaxed Ordering ===" << std::endl;
        
        std::thread producer([this]() {
            data_.store(42, std::memory_order_relaxed);
            ready_.store(true, std::memory_order_relaxed);
            std::cout << "Producer: データ設定完了" << std::endl;
        });
        
        std::thread consumer([this]() {
            while (!ready_.load(std::memory_order_relaxed))
            {
                std::this_thread::sleep_for(std::chrono::microseconds(1));
            }
            std::cout << "Consumer: データ = " << data_.load(std::memory_order_relaxed) << std::endl;
        });
        
        producer.join();
        consumer.join();
        
        // リセット
        data_.store(0, std::memory_order_relaxed);
        ready_.store(false, std::memory_order_relaxed);
    }
    
    // acquire-release ordering - より強い保証
    void acquire_release_example()
    {
        std::cout << "\n=== Acquire-Release Ordering ===" << std::endl;
        
        std::thread producer([this]() {
            data_.store(84, std::memory_order_relaxed);
            ready_.store(true, std::memory_order_release);  // release
            std::cout << "Producer: データ設定とリリース完了" << std::endl;
        });
        
        std::thread consumer([this]() {
            while (!ready_.load(std::memory_order_acquire))  // acquire
            {
                std::this_thread::sleep_for(std::chrono::microseconds(1));
            }
            // acquire-releaseにより、data_の値は確実に42が見える
            std::cout << "Consumer: データ = " << data_.load(std::memory_order_relaxed) << std::endl;
        });
        
        producer.join();
        consumer.join();
    }
    
    // sequential consistency - 最も強い保証（デフォルト）
    void sequential_consistency_example()
    {
        std::cout << "\n=== Sequential Consistency ===" << std::endl;
        
        std::thread producer([this]() {
            data_.store(126);  // デフォルトでmemory_order_seq_cst
            ready_.store(true);
            std::cout << "Producer: シーケンシャル一貫性で設定完了" << std::endl;
        });
        
        std::thread consumer([this]() {
            while (!ready_.load())
            {
                std::this_thread::sleep_for(std::chrono::microseconds(1));
            }
            std::cout << "Consumer: データ = " << data_.load() << std::endl;
        });
        
        producer.join();
        consumer.join();
    }
};

// より複雑な例：複数の変数を使った同期
class ComplexSynchronization
{
private:
    std::atomic<int> x_, y_;
    std::atomic<bool> flag1_, flag2_;
    
public:
    ComplexSynchronization() : x_(0), y_(0), flag1_(false), flag2_(false) {}
    
    void dekker_like_algorithm()
    {
        std::cout << "\n=== デッカーライクアルゴリズム ===" << std::endl;
        
        std::thread thread1([this]() {
            x_.store(1, std::memory_order_relaxed);
            flag1_.store(true, std::memory_order_release);
            
            // thread2のフラグを待つ
            while (!flag2_.load(std::memory_order_acquire))
            {
                std::this_thread::yield();
            }
            
            int y_val = y_.load(std::memory_order_acquire);
            std::cout << "Thread1: y = " << y_val << std::endl;
        });
        
        std::thread thread2([this]() {
            y_.store(2, std::memory_order_relaxed);
            flag2_.store(true, std::memory_order_release);
            
            // thread1のフラグを待つ
            while (!flag1_.load(std::memory_order_acquire))
            {
                std::this_thread::yield();
            }
            
            int x_val = x_.load(std::memory_order_acquire);
            std::cout << "Thread2: x = " << x_val << std::endl;
        });
        
        thread1.join();
        thread2.join();
    }
};

// ゲームでの実用例：ロックフリーカウンター
class GameStatistics
{
private:
    std::atomic<uint64_t> frameCount_;
    std::atomic<uint64_t> totalFrameTime_;
    std::atomic<uint32_t> currentFPS_;
    std::atomic<bool> statsReady_;
    
public:
    GameStatistics() : frameCount_(0), totalFrameTime_(0), currentFPS_(0), statsReady_(false) {}
    
    void updateFrame(uint32_t frameTimeMs)
    {
        // relaxed orderingで高頻度な更新を効率化
        frameCount_.fetch_add(1, std::memory_order_relaxed);
        totalFrameTime_.fetch_add(frameTimeMs, std::memory_order_relaxed);
        
        // 60フレームごとにFPSを計算
        if (frameCount_.load(std::memory_order_relaxed) % 60 == 0)
        {
            uint64_t avgFrameTime = totalFrameTime_.load(std::memory_order_relaxed) / 60;
            uint32_t fps = (avgFrameTime > 0) ? (1000 / avgFrameTime) : 0;
            
            currentFPS_.store(fps, std::memory_order_relaxed);
            statsReady_.store(true, std::memory_order_release);  // 統計準備完了を通知
            
            // カウンターをリセット
            totalFrameTime_.store(0, std::memory_order_relaxed);
        }
    }
    
    bool getStatistics(uint64_t& frames, uint32_t& fps)
    {
        if (statsReady_.load(std::memory_order_acquire))
        {
            frames = frameCount_.load(std::memory_order_relaxed);
            fps = currentFPS_.load(std::memory_order_relaxed);
            return true;
        }
        return false;
    }
};

// フェンスを使った同期
class FenceExample
{
private:
    int data1_, data2_;
    std::atomic<bool> ready_;
    
public:
    FenceExample() : data1_(0), data2_(0), ready_(false) {}
    
    void fence_synchronization()
    {
        std::cout << "\n=== フェンスによる同期 ===" << std::endl;
        
        std::thread writer([this]() {
            data1_ = 100;
            data2_ = 200;
            
            // acquire-releaseフェンス
            std::atomic_thread_fence(std::memory_order_release);
            ready_.store(true, std::memory_order_relaxed);
            
            std::cout << "Writer: データ設定とフェンス完了" << std::endl;
        });
        
        std::thread reader([this]() {
            while (!ready_.load(std::memory_order_relaxed))
            {
                std::this_thread::yield();
            }
            
            // acquire フェンス
            std::atomic_thread_fence(std::memory_order_acquire);
            
            std::cout << "Reader: data1 = " << data1_ << ", data2 = " << data2_ << std::endl;
        });
        
        writer.join();
        reader.join();
    }
};

// 弱いメモリモデルでの問題例
class WeakMemoryModelExample
{
private:
    std::atomic<int> x_, y_;
    std::atomic<int> r1_, r2_;
    
public:
    WeakMemoryModelExample() : x_(0), y_(0), r1_(0), r2_(0) {}
    
    void demonstrate_reordering()
    {
        std::cout << "\n=== メモリ並び替えの例 ===" << std::endl;
        
        for (int iteration = 0; iteration < 1000; ++iteration)
        {
            x_.store(0, std::memory_order_relaxed);
            y_.store(0, std::memory_order_relaxed);
            r1_.store(0, std::memory_order_relaxed);
            r2_.store(0, std::memory_order_relaxed);
            
            std::thread t1([this]() {
                x_.store(1, std::memory_order_relaxed);
                r1_.store(y_.load(std::memory_order_relaxed), std::memory_order_relaxed);
            });
            
            std::thread t2([this]() {
                y_.store(1, std::memory_order_relaxed);
                r2_.store(x_.load(std::memory_order_relaxed), std::memory_order_relaxed);
            });
            
            t1.join();
            t2.join();
            
            int r1_val = r1_.load(std::memory_order_relaxed);
            int r2_val = r2_.load(std::memory_order_relaxed);
            
            // 理論的には r1 == 0 && r2 == 0 になる可能性がある
            if (r1_val == 0 && r2_val == 0)
            {
                std::cout << "反復 " << iteration << ": r1 = " << r1_val 
                          << ", r2 = " << r2_val << " (並び替え発生？)" << std::endl;
                break;
            }
        }
    }
};

// 実用的な例：ロックフリーな単純キャッシュ
template<typename K, typename V>
class SimpleLockFreeCache
{
private:
    struct Entry
    {
        std::atomic<K> key;
        std::atomic<V> value;
        std::atomic<bool> valid;
        
        Entry() : valid(false) {}
    };
    
    static constexpr size_t CACHE_SIZE = 16;
    Entry cache_[CACHE_SIZE];
    std::atomic<size_t> nextIndex_;
    
    size_t hash(const K& key) const
    {
        return std::hash<K>{}(key) % CACHE_SIZE;
    }
    
public:
    SimpleLockFreeCache() : nextIndex_(0) {}
    
    void put(const K& key, const V& value)
    {
        size_t index = hash(key);
        
        // キーが既に存在するかチェック
        for (size_t i = 0; i < CACHE_SIZE; ++i)
        {
            size_t idx = (index + i) % CACHE_SIZE;
            if (cache_[idx].valid.load(std::memory_order_acquire) &&
                cache_[idx].key.load(std::memory_order_relaxed) == key)
            {
                // 既存エントリを更新
                cache_[idx].value.store(value, std::memory_order_relaxed);
                return;
            }
        }
        
        // 新しいエントリを追加
        size_t newIndex = nextIndex_.fetch_add(1, std::memory_order_relaxed) % CACHE_SIZE;
        
        cache_[newIndex].key.store(key, std::memory_order_relaxed);
        cache_[newIndex].value.store(value, std::memory_order_relaxed);
        cache_[newIndex].valid.store(true, std::memory_order_release);
        
        std::cout << "キャッシュ追加: [" << key << "] = " << value 
                  << " (インデックス: " << newIndex << ")" << std::endl;
    }
    
    bool get(const K& key, V& value) const
    {
        size_t index = hash(key);
        
        for (size_t i = 0; i < CACHE_SIZE; ++i)
        {
            size_t idx = (index + i) % CACHE_SIZE;
            if (cache_[idx].valid.load(std::memory_order_acquire) &&
                cache_[idx].key.load(std::memory_order_relaxed) == key)
            {
                value = cache_[idx].value.load(std::memory_order_relaxed);
                return true;
            }
        }
        
        return false;
    }
};

int main()
{
    std::cout << "=== C++メモリモデル ===" << std::endl;
    
    // 1. 基本的なメモリオーダリング
    BasicMemoryOrdering basic;
    basic.relaxed_example();
    basic.acquire_release_example();
    basic.sequential_consistency_example();
    
    // 2. 複雑な同期
    ComplexSynchronization complex;
    complex.dekker_like_algorithm();
    
    // 3. ゲーム統計の例
    std::cout << "\n=== ゲーム統計システム ===" << std::endl;
    GameStatistics stats;
    
    std::thread gameLoop([&stats]() {
        for (int frame = 1; frame <= 200; ++frame)
        {
            // フレーム時間をシミュレート（16-20ms）
            uint32_t frameTime = 16 + (frame % 5);
            stats.updateFrame(frameTime);
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });
    
    std::thread monitor([&stats]() {
        uint64_t frames;
        uint32_t fps;
        
        for (int i = 0; i < 5; ++i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(800));
            
            if (stats.getStatistics(frames, fps))
            {
                std::cout << "統計: フレーム数 = " << frames << ", FPS = " << fps << std::endl;
            }
        }
    });
    
    gameLoop.join();
    monitor.join();
    
    // 4. フェンスの例
    FenceExample fence;
    fence.fence_synchronization();
    
    // 5. 弱いメモリモデルの例
    WeakMemoryModelExample weak;
    weak.demonstrate_reordering();
    
    // 6. ロックフリーキャッシュ
    std::cout << "\n=== ロックフリーキャッシュ ===" << std::endl;
    SimpleLockFreeCache<std::string, int> cache;
    
    std::vector<std::thread> threads;
    
    // 複数スレッドでキャッシュを操作
    for (int i = 0; i < 3; ++i)
    {
        threads.emplace_back([&cache, i]() {
            for (int j = 0; j < 3; ++j)
            {
                std::string key = "key" + std::to_string(i * 3 + j);
                cache.put(key, i * 100 + j);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        });
    }
    
    threads.emplace_back([&cache]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        
        int value;
        if (cache.get("key5", value))
        {
            std::cout << "キャッシュヒット: key5 = " << value << std::endl;
        }
        else
        {
            std::cout << "キャッシュミス: key5" << std::endl;
        }
    });
    
    for (auto& t : threads)
    {
        t.join();
    }
    
    return 0;
}

// まとめ：
// - C++メモリモデルで細かい同期制御が可能
// - relaxed < acquire/release < sequential consistency
// - acquire-releaseでデータ依存関係を保証
// - フェンスで明示的なメモリバリアを設置
// - 高性能な並行プログラムでは適切な選択が重要