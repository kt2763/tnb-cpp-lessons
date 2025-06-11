// 高度なコルーチンパターン
// 実用的なコルーチンアプリケーションと設計パターン

#include <iostream>
#include <coroutine>
#include <memory>
#include <vector>
#include <queue>
#include <unordered_map>
#include <string>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <random>
#include <functional>
#include <optional>

// 高度なTask型（キャンセレーション対応）
template<typename T>
class CancellableTask
{
public:
    struct promise_type
    {
        T value;
        std::exception_ptr exception;
        std::shared_ptr<std::atomic<bool>> cancellationToken;
        std::coroutine_handle<> waitingCoroutine;
        
        CancellableTask get_return_object()
        {
            cancellationToken = std::make_shared<std::atomic<bool>>(false);
            return CancellableTask{
                std::coroutine_handle<promise_type>::from_promise(*this),
                cancellationToken
            };
        }
        
        std::suspend_never initial_suspend() { return {}; }
        
        auto final_suspend() noexcept
        {
            struct FinalAwaiter
            {
                bool await_ready() noexcept { return false; }
                
                std::coroutine_handle<> await_suspend(std::coroutine_handle<promise_type> h) noexcept
                {
                    auto& promise = h.promise();
                    if (promise.waitingCoroutine)
                    {
                        return promise.waitingCoroutine;
                    }
                    return std::noop_coroutine();
                }
                
                void await_resume() noexcept {}
            };
            
            return FinalAwaiter{};
        }
        
        void return_value(T val)
        {
            if (!cancellationToken->load())
            {
                value = std::move(val);
            }
        }
        
        void unhandled_exception()
        {
            exception = std::current_exception();
        }
    };
    
    std::coroutine_handle<promise_type> handle;
    std::shared_ptr<std::atomic<bool>> cancellationToken;
    
    CancellableTask(std::coroutine_handle<promise_type> h, std::shared_ptr<std::atomic<bool>> token)
        : handle(h), cancellationToken(token) {}
    
    ~CancellableTask()
    {
        if (handle)
            handle.destroy();
    }
    
    CancellableTask(const CancellableTask&) = delete;
    CancellableTask& operator=(const CancellableTask&) = delete;
    
    CancellableTask(CancellableTask&& other) noexcept
        : handle(other.handle), cancellationToken(other.cancellationToken)
    {
        other.handle = {};
    }
    
    CancellableTask& operator=(CancellableTask&& other) noexcept
    {
        if (this != &other)
        {
            if (handle)
                handle.destroy();
            handle = other.handle;
            cancellationToken = other.cancellationToken;
            other.handle = {};
        }
        return *this;
    }
    
    void cancel()
    {
        cancellationToken->store(true);
    }
    
    bool is_cancelled() const
    {
        return cancellationToken->load();
    }
    
    bool await_ready() const
    {
        return handle.done() || is_cancelled();
    }
    
    void await_suspend(std::coroutine_handle<> waitingCoroutine)
    {
        handle.promise().waitingCoroutine = waitingCoroutine;
        if (!handle.done() && !is_cancelled())
        {
            handle.resume();
        }
    }
    
    T await_resume()
    {
        if (is_cancelled())
        {
            throw std::runtime_error("タスクがキャンセルされました");
        }
        
        if (handle.promise().exception)
        {
            std::rethrow_exception(handle.promise().exception);
        }
        
        return std::move(handle.promise().value);
    }
    
    T get()
    {
        while (!handle.done() && !is_cancelled())
        {
            handle.resume();
        }
        
        if (is_cancelled())
        {
            throw std::runtime_error("タスクがキャンセルされました");
        }
        
        if (handle.promise().exception)
        {
            std::rethrow_exception(handle.promise().exception);
        }
        
        return std::move(handle.promise().value);
    }
};

// コルーチンプール（軽量なスレッドプール）
class CoroutinePool
{
private:
    struct CoroutineEntry
    {
        std::coroutine_handle<> handle;
        bool active;
        std::chrono::steady_clock::time_point nextWakeup;
        
        CoroutineEntry(std::coroutine_handle<> h)
            : handle(h), active(true), nextWakeup(std::chrono::steady_clock::now()) {}
    };
    
    std::vector<CoroutineEntry> coroutines_;
    std::atomic<bool> running_{true};
    std::thread worker_;
    std::mutex mutex_;
    
public:
    CoroutinePool()
    {
        worker_ = std::thread([this]() {
            while (running_.load())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                processCoroutines();
            }
        });
    }
    
    ~CoroutinePool()
    {
        running_.store(false);
        if (worker_.joinable())
        {
            worker_.join();
        }
        
        // 残りのコルーチンを破棄
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& entry : coroutines_)
        {
            if (entry.handle)
            {
                entry.handle.destroy();
            }
        }
    }
    
    void schedule(std::coroutine_handle<> handle)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        coroutines_.emplace_back(handle);
    }
    
    void scheduleAfter(std::coroutine_handle<> handle, std::chrono::milliseconds delay)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        CoroutineEntry entry(handle);
        entry.nextWakeup = std::chrono::steady_clock::now() + delay;
        coroutines_.emplace_back(entry);
    }
    
private:
    void processCoroutines()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto now = std::chrono::steady_clock::now();
        
        for (auto it = coroutines_.begin(); it != coroutines_.end();)
        {
            if (!it->active || it->handle.done())
            {
                if (it->handle)
                {
                    it->handle.destroy();
                }
                it = coroutines_.erase(it);
            }
            else if (now >= it->nextWakeup)
            {
                it->handle.resume();
                if (it->handle.done())
                {
                    it->active = false;
                }
                ++it;
            }
            else
            {
                ++it;
            }
        }
    }
};

// スケジューラー（時間ベースの実行制御）
class CoroutineScheduler
{
private:
    CoroutinePool& pool_;
    
public:
    explicit CoroutineScheduler(CoroutinePool& pool) : pool_(pool) {}
    
    struct DelayAwaiter
    {
        std::chrono::milliseconds delay;
        CoroutinePool& pool;
        
        DelayAwaiter(std::chrono::milliseconds d, CoroutinePool& p) : delay(d), pool(p) {}
        
        bool await_ready() const { return delay.count() <= 0; }
        
        void await_suspend(std::coroutine_handle<> handle)
        {
            pool.scheduleAfter(handle, delay);
        }
        
        void await_resume() {}
    };
    
    DelayAwaiter sleep(std::chrono::milliseconds delay)
    {
        return DelayAwaiter(delay, pool_);
    }
};

// 非同期イベントシステム
template<typename EventType>
class AsyncEventSystem
{
private:
    struct EventSubscription
    {
        std::coroutine_handle<> handle;
        std::function<bool(const EventType&)> filter;
        EventType lastEvent;
        bool eventReceived = false;
    };
    
    std::vector<EventSubscription> subscriptions_;
    std::mutex mutex_;
    
public:
    void publishEvent(const EventType& event)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        
        for (auto& subscription : subscriptions_)
        {
            if (!subscription.filter || subscription.filter(event))
            {
                subscription.lastEvent = event;
                subscription.eventReceived = true;
                
                if (subscription.handle && !subscription.handle.done())
                {
                    subscription.handle.resume();
                }
            }
        }
    }
    
    struct EventAwaiter
    {
        AsyncEventSystem& system;
        std::function<bool(const EventType&)> filter;
        mutable std::optional<size_t> subscriptionIndex;
        
        EventAwaiter(AsyncEventSystem& sys, std::function<bool(const EventType&)> f = nullptr)
            : system(sys), filter(f) {}
        
        bool await_ready() const { return false; }
        
        void await_suspend(std::coroutine_handle<> handle)
        {
            std::lock_guard<std::mutex> lock(system.mutex_);
            
            EventSubscription subscription;
            subscription.handle = handle;
            subscription.filter = filter;
            
            subscriptionIndex = system.subscriptions_.size();
            system.subscriptions_.push_back(subscription);
        }
        
        EventType await_resume()
        {
            std::lock_guard<std::mutex> lock(system.mutex_);
            
            if (subscriptionIndex.has_value() && 
                subscriptionIndex.value() < system.subscriptions_.size())
            {
                auto& subscription = system.subscriptions_[subscriptionIndex.value()];
                EventType result = subscription.lastEvent;
                
                // 使用済み購読を削除
                system.subscriptions_.erase(system.subscriptions_.begin() + subscriptionIndex.value());
                
                return result;
            }
            
            throw std::runtime_error("イベント待機エラー");
        }
    };
    
    EventAwaiter waitForEvent(std::function<bool(const EventType&)> filter = nullptr)
    {
        return EventAwaiter(*this, filter);
    }
};

// ゲームイベントの定義
struct GameEvent
{
    enum class Type { PlayerAction, EnemyAction, SystemEvent, InputEvent };
    
    Type type;
    std::string description;
    std::unordered_map<std::string, std::string> data;
    
    GameEvent(Type t, const std::string& desc) : type(t), description(desc) {}
};

// ゲームステートマシン（コルーチンベース）
class GameStateMachine
{
private:
    AsyncEventSystem<GameEvent> eventSystem_;
    CoroutinePool pool_;
    CoroutineScheduler scheduler_;
    std::atomic<bool> running_{true};
    
public:
    GameStateMachine() : scheduler_(pool_) {}
    
    ~GameStateMachine()
    {
        running_.store(false);
    }
    
    void publishEvent(const GameEvent& event)
    {
        eventSystem_.publishEvent(event);
    }
    
    // メニューステート
    CancellableTask<void> menuState()
    {
        std::cout << "\n=== メニューステート ===" << std::endl;
        
        while (running_.load())
        {
            std::cout << "メニュー表示中..." << std::endl;
            
            // プレイヤーの入力を待機
            auto event = co_await eventSystem_.waitForEvent([](const GameEvent& e) {
                return e.type == GameEvent::Type::PlayerAction;
            });
            
            std::cout << "プレイヤーアクション: " << event.description << std::endl;
            
            if (event.description == "start_game")
            {
                std::cout << "ゲーム開始！" << std::endl;
                co_return;
            }
            else if (event.description == "quit")
            {
                std::cout << "ゲーム終了" << std::endl;
                running_.store(false);
                co_return;
            }
            
            co_await scheduler_.sleep(std::chrono::milliseconds(100));
        }
    }
    
    // ゲームプレイステート
    CancellableTask<void> gameplayState()
    {
        std::cout << "\n=== ゲームプレイステート ===" << std::endl;
        
        int playerHealth = 100;
        int enemyHealth = 80;
        
        while (running_.load() && playerHealth > 0 && enemyHealth > 0)
        {
            std::cout << "戦闘中... プレイヤー:" << playerHealth << " 敵:" << enemyHealth << std::endl;
            
            // プレイヤーまたは敵のアクションを待機
            auto event = co_await eventSystem_.waitForEvent([](const GameEvent& e) {
                return e.type == GameEvent::Type::PlayerAction || 
                       e.type == GameEvent::Type::EnemyAction;
            });
            
            if (event.type == GameEvent::Type::PlayerAction)
            {
                std::cout << "プレイヤーの" << event.description << std::endl;
                if (event.description == "attack")
                {
                    enemyHealth -= 20;
                    std::cout << "敵にダメージ！残り体力: " << enemyHealth << std::endl;
                }
            }
            else if (event.type == GameEvent::Type::EnemyAction)
            {
                std::cout << "敵の" << event.description << std::endl;
                if (event.description == "attack")
                {
                    playerHealth -= 15;
                    std::cout << "プレイヤーにダメージ！残り体力: " << playerHealth << std::endl;
                }
            }
            
            co_await scheduler_.sleep(std::chrono::milliseconds(200));
        }
        
        if (playerHealth <= 0)
        {
            std::cout << "ゲームオーバー" << std::endl;
        }
        else if (enemyHealth <= 0)
        {
            std::cout << "勝利！" << std::endl;
        }
    }
    
    // AIエージェント（敵の思考ルーチン）
    CancellableTask<void> enemyAI()
    {
        std::cout << "\n=== 敵AIステート ===" << std::endl;
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> actionDist(0, 2);
        std::uniform_int_distribution<> timingDist(500, 1500);
        
        while (running_.load())
        {
            // ランダムな時間待機
            int waitTime = timingDist(gen);
            co_await scheduler_.sleep(std::chrono::milliseconds(waitTime));
            
            // ランダムなアクションを選択
            std::vector<std::string> actions = {"attack", "defend", "special"};
            int actionIndex = actionDist(gen);
            
            GameEvent aiEvent(GameEvent::Type::EnemyAction, actions[actionIndex]);
            publishEvent(aiEvent);
            
            std::cout << "AI: " << actions[actionIndex] << " を実行" << std::endl;
        }
    }
    
    void run()
    {
        std::cout << "=== ゲームステートマシン開始 ===" << std::endl;
        
        // 各ステートを並列実行
        auto menuTask = menuState();
        auto aiTask = enemyAI();
        
        // メニューからスタート
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        publishEvent(GameEvent(GameEvent::Type::PlayerAction, "start_game"));
        
        try
        {
            menuTask.get();
            
            // ゲームプレイステートに移行
            auto gameplayTask = gameplayState();
            
            // プレイヤーアクションをシミュレート
            std::thread playerSimulation([this]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
                publishEvent(GameEvent(GameEvent::Type::PlayerAction, "attack"));
                
                std::this_thread::sleep_for(std::chrono::milliseconds(400));
                publishEvent(GameEvent(GameEvent::Type::PlayerAction, "attack"));
                
                std::this_thread::sleep_for(std::chrono::milliseconds(600));
                publishEvent(GameEvent(GameEvent::Type::PlayerAction, "attack"));
                
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                publishEvent(GameEvent(GameEvent::Type::PlayerAction, "attack"));
            });
            
            gameplayTask.get();
            
            if (playerSimulation.joinable())
            {
                playerSimulation.join();
            }
        }
        catch (const std::exception& e)
        {
            std::cout << "エラー: " << e.what() << std::endl;
        }
        
        // AIタスクをキャンセル
        aiTask.cancel();
        
        std::cout << "ゲームステートマシン終了" << std::endl;
    }
};

// パイプライン処理パターン
template<typename Input, typename Output>
class Pipeline
{
private:
    std::function<CancellableTask<Output>(Input)> processor_;
    
public:
    explicit Pipeline(std::function<CancellableTask<Output>(Input)> proc)
        : processor_(proc) {}
    
    template<typename NextOutput>
    auto then(std::function<CancellableTask<NextOutput>(Output)> nextProcessor)
    {
        return Pipeline<Input, NextOutput>([processor = processor_, nextProcessor](Input input) -> CancellableTask<NextOutput> {
            auto intermediate = co_await processor(input);
            co_return co_await nextProcessor(intermediate);
        });
    }
    
    CancellableTask<Output> process(Input input)
    {
        return processor_(input);
    }
};

// パイプライン使用例
CancellableTask<std::string> loadResource(const std::string& filename)
{
    std::cout << "リソース読み込み: " << filename << std::endl;
    co_await [](std::chrono::milliseconds duration) -> CancellableTask<void> {
        std::this_thread::sleep_for(duration);
        co_return;
    }(std::chrono::milliseconds(100));
    co_return "リソースデータ: " + filename;
}

CancellableTask<std::string> processResource(const std::string& data)
{
    std::cout << "リソース処理: " << data << std::endl;
    co_await [](std::chrono::milliseconds duration) -> CancellableTask<void> {
        std::this_thread::sleep_for(duration);
        co_return;
    }(std::chrono::milliseconds(50));
    co_return "処理済み: " + data;
}

CancellableTask<std::string> cacheResource(const std::string& processedData)
{
    std::cout << "リソースキャッシュ: " << processedData << std::endl;
    co_await [](std::chrono::milliseconds duration) -> CancellableTask<void> {
        std::this_thread::sleep_for(duration);
        co_return;
    }(std::chrono::milliseconds(25));
    co_return "キャッシュ済み: " + processedData;
}

void demonstratePipeline()
{
    std::cout << "\n=== パイプライン処理パターン ===" << std::endl;
    
    // パイプラインを構築
    auto pipeline = Pipeline<std::string, std::string>(loadResource)
        .then<std::string>(processResource)
        .then<std::string>(cacheResource);
    
    // パイプラインを実行
    auto result = pipeline.process("texture.png");
    auto finalResult = result.get();
    
    std::cout << "最終結果: " << finalResult << std::endl;
}

int main()
{
    std::cout << "=== 高度なコルーチンパターン ===" << std::endl;
    
    // 1. パイプライン処理パターン
    demonstratePipeline();
    
    // 2. ゲームステートマシン
    {
        GameStateMachine gameSystem;
        gameSystem.run();
    }
    
    std::cout << "\n=== 高度なパターンの利点 ===" << std::endl;
    std::cout << "- キャンセレーション：長時間実行タスクの制御" << std::endl;
    std::cout << "- コルーチンプール：効率的な軽量スレッド管理" << std::endl;
    std::cout << "- イベントシステム：非同期イベント駆動プログラミング" << std::endl;
    std::cout << "- ステートマシン：ゲーム状態の管理と遷移" << std::endl;
    std::cout << "- パイプライン：データ処理の段階的実行" << std::endl;
    std::cout << "- スケジューラー：時間ベースの実行制御" << std::endl;
    
    return 0;
}

// まとめ：
// - キャンセレーション機能で長時間タスクを制御
// - コルーチンプールで効率的な並行実行を実現
// - 非同期イベントシステムでイベント駆動プログラミング
// - ステートマシンでゲーム状態を管理
// - パイプラインパターンでデータ処理を段階化
// - これらのパターンでゲーム開発における複雑な非同期処理を簡潔に記述