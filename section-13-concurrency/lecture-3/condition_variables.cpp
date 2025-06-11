// 条件変数（Condition Variables）
// スレッド間の協調と効率的な待機を実現

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <string>
#include <chrono>
#include <functional>

// 基本的な条件変数の使用例
class SimpleBuffer
{
private:
    std::queue<int> buffer_;
    std::mutex mutex_;
    std::condition_variable condition_;
    bool finished_;
    
public:
    SimpleBuffer() : finished_(false) {}
    
    void produce(int value)
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            buffer_.push(value);
            std::cout << "生産: " << value << " (バッファサイズ: " << buffer_.size() << ")" << std::endl;
        }
        condition_.notify_one();  // 待機中のスレッドに通知
    }
    
    bool consume(int& value)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        
        // バッファが空でない、または処理完了まで待機
        condition_.wait(lock, [this] { return !buffer_.empty() || finished_; });
        
        if (buffer_.empty() && finished_)
        {
            return false;  // 処理完了
        }
        
        value = buffer_.front();
        buffer_.pop();
        std::cout << "消費: " << value << " (バッファサイズ: " << buffer_.size() << ")" << std::endl;
        return true;
    }
    
    void finish()
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            finished_ = true;
        }
        condition_.notify_all();  // すべての待機スレッドに通知
    }
};

// ゲームでの応用：タスクキュー
struct GameTask
{
    std::string taskName;
    std::function<void()> action;
    int priority;
    
    GameTask(const std::string& name, std::function<void()> act, int prio = 0)
        : taskName(name), action(act), priority(prio) {}
};

class TaskQueue
{
private:
    std::queue<GameTask> tasks_;
    std::mutex mutex_;
    std::condition_variable condition_;
    bool shutdown_;
    
public:
    TaskQueue() : shutdown_(false) {}
    
    void addTask(const GameTask& task)
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!shutdown_)
            {
                tasks_.push(task);
                std::cout << "タスク追加: " << task.taskName << std::endl;
            }
        }
        condition_.notify_one();
    }
    
    bool getTask(GameTask& task)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        
        // タスクがあるかシャットダウンまで待機
        condition_.wait(lock, [this] { return !tasks_.empty() || shutdown_; });
        
        if (tasks_.empty() && shutdown_)
        {
            return false;
        }
        
        task = tasks_.front();
        tasks_.pop();
        std::cout << "タスク取得: " << task.taskName << std::endl;
        return true;
    }
    
    void shutdown()
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            shutdown_ = true;
        }
        condition_.notify_all();
    }
    
    size_t size() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return tasks_.size();
    }
};

// ワーカースレッドプール
class ThreadPool
{
private:
    std::vector<std::thread> workers_;
    TaskQueue taskQueue_;
    
public:
    explicit ThreadPool(size_t numThreads)
    {
        for (size_t i = 0; i < numThreads; ++i)
        {
            workers_.emplace_back([this, i]() {
                std::cout << "ワーカースレッド " << i << " 開始" << std::endl;
                
                GameTask task("", nullptr);
                while (taskQueue_.getTask(task))
                {
                    std::cout << "ワーカー " << i << " がタスク実行: " << task.taskName << std::endl;
                    task.action();
                }
                
                std::cout << "ワーカースレッド " << i << " 終了" << std::endl;
            });
        }
    }
    
    ~ThreadPool()
    {
        shutdown();
    }
    
    void addTask(const GameTask& task)
    {
        taskQueue_.addTask(task);
    }
    
    void shutdown()
    {
        taskQueue_.shutdown();
        for (auto& worker : workers_)
        {
            if (worker.joinable())
            {
                worker.join();
            }
        }
    }
};

// 同期バリア（全スレッドの完了を待つ）
class Barrier
{
private:
    std::mutex mutex_;
    std::condition_variable condition_;
    size_t count_;
    size_t waiting_;
    size_t generation_;
    
public:
    explicit Barrier(size_t count) : count_(count), waiting_(0), generation_(0) {}
    
    void wait()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        size_t gen = generation_;
        
        if (++waiting_ == count_)
        {
            // 最後のスレッドが到着
            waiting_ = 0;
            ++generation_;
            condition_.notify_all();
        }
        else
        {
            // 他のスレッドを待機
            condition_.wait(lock, [this, gen] { return gen != generation_; });
        }
    }
};

// イベント待機システム
class GameEvent
{
private:
    std::mutex mutex_;
    std::condition_variable condition_;
    bool triggered_;
    
public:
    GameEvent() : triggered_(false) {}
    
    void trigger()
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            triggered_ = true;
        }
        condition_.notify_all();
        std::cout << "イベント発火！" << std::endl;
    }
    
    void wait()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [this] { return triggered_; });
        std::cout << "イベント受信" << std::endl;
    }
    
    bool waitFor(std::chrono::milliseconds timeout)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        return condition_.wait_for(lock, timeout, [this] { return triggered_; });
    }
    
    void reset()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        triggered_ = false;
    }
};

// セマフォの実装
class Semaphore
{
private:
    std::mutex mutex_;
    std::condition_variable condition_;
    int count_;
    
public:
    explicit Semaphore(int count) : count_(count) {}
    
    void acquire()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [this] { return count_ > 0; });
        --count_;
    }
    
    void release()
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            ++count_;
        }
        condition_.notify_one();
    }
    
    bool tryAcquire()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (count_ > 0)
        {
            --count_;
            return true;
        }
        return false;
    }
    
    bool acquireFor(std::chrono::milliseconds timeout)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (condition_.wait_for(lock, timeout, [this] { return count_ > 0; }))
        {
            --count_;
            return true;
        }
        return false;
    }
};

// 実演関数
void demonstrateBasicConditionVariable()
{
    std::cout << "\n=== 基本的な条件変数 ===" << std::endl;
    
    SimpleBuffer buffer;
    
    // 生産者スレッド
    std::thread producer([&buffer]() {
        for (int i = 1; i <= 10; ++i)
        {
            buffer.produce(i);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        buffer.finish();
    });
    
    // 消費者スレッド
    std::thread consumer([&buffer]() {
        int value;
        while (buffer.consume(value))
        {
            std::cout << "処理中: " << value << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
        }
    });
    
    producer.join();
    consumer.join();
}

void demonstrateThreadPool()
{
    std::cout << "\n=== スレッドプール ===" << std::endl;
    
    ThreadPool pool(3);
    
    // タスクを追加
    for (int i = 1; i <= 8; ++i)
    {
        pool.addTask(GameTask(
            "計算タスク" + std::to_string(i),
            [i]() {
                std::cout << "  計算実行: " << i << " x " << i << " = " << (i * i) << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }
        ));
    }
    
    // 少し待ってからシャットダウン
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

void demonstrateBarrier()
{
    std::cout << "\n=== 同期バリア ===" << std::endl;
    
    Barrier barrier(4);
    std::vector<std::thread> threads;
    
    for (int i = 0; i < 4; ++i)
    {
        threads.emplace_back([&barrier, i]() {
            // フェーズ1の処理
            std::cout << "スレッド " << i << " フェーズ1開始" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100 * (i + 1)));
            std::cout << "スレッド " << i << " フェーズ1完了" << std::endl;
            
            // 他のスレッドを待機
            barrier.wait();
            
            // フェーズ2の処理
            std::cout << "スレッド " << i << " フェーズ2開始" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::cout << "スレッド " << i << " フェーズ2完了" << std::endl;
        });
    }
    
    for (auto& t : threads)
    {
        t.join();
    }
}

void demonstrateGameEvent()
{
    std::cout << "\n=== ゲームイベント ===" << std::endl;
    
    GameEvent levelCompleteEvent;
    
    // イベント待機スレッド
    std::vector<std::thread> waiters;
    for (int i = 1; i <= 3; ++i)
    {
        waiters.emplace_back([&levelCompleteEvent, i]() {
            std::cout << "プレイヤー " << i << " がレベルクリアイベントを待機中..." << std::endl;
            levelCompleteEvent.wait();
            std::cout << "プレイヤー " << i << " がレベルクリアを確認！" << std::endl;
        });
    }
    
    // イベント発火
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    levelCompleteEvent.trigger();
    
    for (auto& t : waiters)
    {
        t.join();
    }
}

void demonstrateSemaphore()
{
    std::cout << "\n=== セマフォ ===" << std::endl;
    
    Semaphore resourceSemaphore(2);  // 最大2つのリソース
    
    std::vector<std::thread> threads;
    for (int i = 1; i <= 5; ++i)
    {
        threads.emplace_back([&resourceSemaphore, i]() {
            std::cout << "スレッド " << i << " がリソース取得を試行..." << std::endl;
            resourceSemaphore.acquire();
            
            std::cout << "スレッド " << i << " がリソース取得成功！" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            
            std::cout << "スレッド " << i << " がリソースを解放" << std::endl;
            resourceSemaphore.release();
        });
    }
    
    for (auto& t : threads)
    {
        t.join();
    }
}

int main()
{
    std::cout << "=== 条件変数（Condition Variables）===" << std::endl;
    
    // 1. 基本的な条件変数
    demonstrateBasicConditionVariable();
    
    // 2. スレッドプール
    demonstrateThreadPool();
    
    // 3. 同期バリア
    demonstrateBarrier();
    
    // 4. ゲームイベント
    demonstrateGameEvent();
    
    // 5. セマフォ
    demonstrateSemaphore();
    
    return 0;
}

// まとめ：
// - condition_variableでスレッド間の協調を実現
// - wait/notify_oneで効率的な待機と通知
// - バリア、セマフォなどの同期プリミティブを実装
// - ゲーム開発ではタスクキューやイベントシステムに活用
// - タイムアウト付き待機で応答性を確保