// co_await と Awaitable 型
// カスタム awaitable 型の実装と高度な待機パターン

#include <iostream>
#include <coroutine>
#include <thread>
#include <chrono>
#include <atomic>
#include <memory>
#include <future>
#include <random>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>

// Awaitable 概念を実装するための基本的なインターフェース
template<typename T>
concept Awaitable = requires(T&& t, std::coroutine_handle<> h) {
    { t.await_ready() } -> std::convertible_to<bool>;
    { t.await_suspend(h) };
    { t.await_resume() };
};

// 基本的なTask型（前回から改良）
template<typename T>
class Task
{
public:
    struct promise_type
    {
        T value;
        std::exception_ptr exception;
        std::coroutine_handle<> waitingCoroutine;
        
        Task get_return_object()
        {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
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
            value = val;
        }
        
        void unhandled_exception()
        {
            exception = std::current_exception();
        }
    };
    
    std::coroutine_handle<promise_type> handle;
    
    explicit Task(std::coroutine_handle<promise_type> h) : handle(h) {}
    
    ~Task()
    {
        if (handle)
            handle.destroy();
    }
    
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;
    
    Task(Task&& other) noexcept : handle(other.handle)
    {
        other.handle = {};
    }
    
    Task& operator=(Task&& other) noexcept
    {
        if (this != &other)
        {
            if (handle)
                handle.destroy();
            handle = other.handle;
            other.handle = {};
        }
        return *this;
    }
    
    // Awaitable インターフェース
    bool await_ready() const
    {
        return handle.done();
    }
    
    void await_suspend(std::coroutine_handle<> waitingCoroutine)
    {
        handle.promise().waitingCoroutine = waitingCoroutine;
        if (!handle.done())
        {
            handle.resume();
        }
    }
    
    T await_resume()
    {
        if (handle.promise().exception)
        {
            std::rethrow_exception(handle.promise().exception);
        }
        return handle.promise().value;
    }
    
    // 従来の同期取得方法
    T get()
    {
        while (!handle.done())
        {
            handle.resume();
        }
        
        if (handle.promise().exception)
        {
            std::rethrow_exception(handle.promise().exception);
        }
        
        return handle.promise().value;
    }
};

// タイマー awaitable
class Timer
{
private:
    std::chrono::milliseconds duration_;
    
public:
    explicit Timer(std::chrono::milliseconds duration) : duration_(duration) {}
    
    bool await_ready() const
    {
        return duration_.count() <= 0;
    }
    
    void await_suspend(std::coroutine_handle<> handle)
    {
        std::thread([handle, duration = duration_]() {
            std::this_thread::sleep_for(duration);
            handle.resume();
        }).detach();
    }
    
    void await_resume() {}
};

// イベント awaitable
class Event
{
private:
    std::atomic<bool> signaled_{false};
    std::vector<std::coroutine_handle<>> waitingCoroutines_;
    std::mutex mutex_;
    
public:
    bool await_ready()
    {
        return signaled_.load();
    }
    
    void await_suspend(std::coroutine_handle<> handle)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!signaled_.load())
        {
            waitingCoroutines_.push_back(handle);
        }
        else
        {
            handle.resume();
        }
    }
    
    void await_resume() {}
    
    void signal()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        signaled_.store(true);
        
        for (auto handle : waitingCoroutines_)
        {
            handle.resume();
        }
        waitingCoroutines_.clear();
    }
    
    void reset()
    {
        signaled_.store(false);
    }
};

// 条件待機 awaitable
template<typename Predicate>
class WaitUntil
{
private:
    Predicate predicate_;
    std::chrono::milliseconds checkInterval_;
    
public:
    WaitUntil(Predicate pred, std::chrono::milliseconds interval = std::chrono::milliseconds(10))
        : predicate_(pred), checkInterval_(interval) {}
    
    bool await_ready()
    {
        return predicate_();
    }
    
    void await_suspend(std::coroutine_handle<> handle)
    {
        std::thread([handle, pred = predicate_, interval = checkInterval_]() {
            while (!pred())
            {
                std::this_thread::sleep_for(interval);
            }
            handle.resume();
        }).detach();
    }
    
    void await_resume() {}
};

// 値待機 awaitable
template<typename T>
class ValueAwaiter
{
private:
    std::shared_ptr<T> value_;
    std::shared_ptr<std::atomic<bool>> ready_;
    
public:
    ValueAwaiter() : ready_(std::make_shared<std::atomic<bool>>(false)) {}
    
    bool await_ready()
    {
        return ready_->load();
    }
    
    void await_suspend(std::coroutine_handle<> handle)
    {
        std::thread([handle, ready = ready_]() {
            while (!ready->load())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            handle.resume();
        }).detach();
    }
    
    T await_resume()
    {
        return *value_;
    }
    
    void setValue(T value)
    {
        value_ = std::make_shared<T>(std::move(value));
        ready_->store(true);
    }
};

// ゲーム用のリソース読み込み awaitable
class ResourceLoader
{
private:
    std::string filename_;
    int sizeKB_;
    std::vector<char> data_;
    
public:
    ResourceLoader(const std::string& filename, int sizeKB)
        : filename_(filename), sizeKB_(sizeKB) {}
    
    bool await_ready() const
    {
        return false;  // 常に非同期で読み込む
    }
    
    void await_suspend(std::coroutine_handle<> handle)
    {
        std::thread([this, handle]() {
            std::cout << "リソース読み込み開始: " << filename_ << std::endl;
            
            // 読み込み時間をシミュレート
            std::this_thread::sleep_for(std::chrono::milliseconds(sizeKB_ * 2));
            
            // データ生成
            data_.resize(sizeKB_ * 1024);
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, 255);
            
            for (auto& byte : data_)
            {
                byte = static_cast<char>(dis(gen));
            }
            
            std::cout << "リソース読み込み完了: " << filename_ << std::endl;
            handle.resume();
        }).detach();
    }
    
    std::vector<char> await_resume()
    {
        return std::move(data_);
    }
};

// ネットワーク要求 awaitable
class NetworkRequest
{
private:
    std::string url_;
    int latency_;
    std::string response_;
    
public:
    NetworkRequest(const std::string& url, int latency)
        : url_(url), latency_(latency) {}
    
    bool await_ready() const
    {
        return false;
    }
    
    void await_suspend(std::coroutine_handle<> handle)
    {
        std::thread([this, handle]() {
            std::cout << "ネットワーク要求送信: " << url_ << std::endl;
            
            // ネットワーク遅延をシミュレート
            std::this_thread::sleep_for(std::chrono::milliseconds(latency_));
            
            response_ = "Data from " + url_ + " (latency: " + std::to_string(latency_) + "ms)";
            
            std::cout << "ネットワーク応答受信: " << url_ << std::endl;
            handle.resume();
        }).detach();
    }
    
    std::string await_resume()
    {
        return response_;
    }
};

// ゲームの入力待機 awaitable
class InputAwaiter
{
private:
    std::string expectedInput_;
    std::string actualInput_;
    std::atomic<bool> inputReceived_{false};
    
public:
    explicit InputAwaiter(const std::string& expected) : expectedInput_(expected) {}
    
    bool await_ready()
    {
        return inputReceived_.load();
    }
    
    void await_suspend(std::coroutine_handle<> handle)
    {
        std::thread([this, handle]() {
            std::cout << "入力待機中: 「" << expectedInput_ << "」を入力してください" << std::endl;
            
            // 入力をシミュレート（実際はキーボード入力を待つ）
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            
            actualInput_ = expectedInput_;  // シミュレーション
            inputReceived_.store(true);
            
            std::cout << "入力受信: " << actualInput_ << std::endl;
            handle.resume();
        }).detach();
    }
    
    std::string await_resume()
    {
        return actualInput_;
    }
};

// 複数のタスクを並列実行するための awaitable
template<typename... Tasks>
class WhenAll
{
private:
    std::tuple<Tasks...> tasks_;
    
public:
    explicit WhenAll(Tasks... tasks) : tasks_(std::move(tasks)...) {}
    
    bool await_ready()
    {
        return std::apply([](auto&... tasks) {
            return (tasks.await_ready() && ...);
        }, tasks_);
    }
    
    void await_suspend(std::coroutine_handle<> handle)
    {
        // シンプルな実装：各タスクを順次実行
        std::apply([handle](auto&... tasks) {
            ((tasks.await_ready() || (tasks.await_suspend(handle), false)), ...);
        }, tasks_);
    }
    
    auto await_resume()
    {
        return std::apply([](auto&... tasks) {
            return std::make_tuple(tasks.await_resume()...);
        }, tasks_);
    }
};

// タイムアウト付き awaitable
template<typename Awaitable>
class WithTimeout
{
private:
    Awaitable awaitable_;
    std::chrono::milliseconds timeout_;
    bool timedOut_{false};
    
public:
    WithTimeout(Awaitable awaitable, std::chrono::milliseconds timeout)
        : awaitable_(std::move(awaitable)), timeout_(timeout) {}
    
    bool await_ready()
    {
        return awaitable_.await_ready();
    }
    
    void await_suspend(std::coroutine_handle<> handle)
    {
        // タイムアウトタイマーを開始
        std::thread([this, handle]() {
            std::this_thread::sleep_for(timeout_);
            if (!awaitable_.await_ready())
            {
                timedOut_ = true;
                handle.resume();
            }
        }).detach();
        
        // 元のawaitableを実行
        if (!awaitable_.await_ready())
        {
            awaitable_.await_suspend(handle);
        }
    }
    
    auto await_resume()
    {
        if (timedOut_)
        {
            throw std::runtime_error("操作がタイムアウトしました");
        }
        return awaitable_.await_resume();
    }
};

// ヘルパー関数
Timer sleep(std::chrono::milliseconds duration)
{
    return Timer(duration);
}

template<typename Predicate>
WaitUntil<Predicate> wait_until(Predicate pred, std::chrono::milliseconds interval = std::chrono::milliseconds(10))
{
    return WaitUntil<Predicate>(pred, interval);
}

template<typename Awaitable>
WithTimeout<Awaitable> with_timeout(Awaitable awaitable, std::chrono::milliseconds timeout)
{
    return WithTimeout<Awaitable>(std::move(awaitable), timeout);
}

// 実践例：ゲームのロード画面
Task<void> gameLoadingSequence()
{
    std::cout << "\n=== ゲームロード画面 ===" << std::endl;
    
    // 複数のリソースを並列読み込み
    auto textureLoader = ResourceLoader("textures.pak", 1024);
    auto soundLoader = ResourceLoader("sounds.pak", 2048);
    auto modelLoader = ResourceLoader("models.pak", 512);
    
    std::cout << "並列リソース読み込み開始..." << std::endl;
    
    // 各リソースを個別に待機
    auto textureData = co_await textureLoader;
    std::cout << "テクスチャ読み込み完了: " << textureData.size() << " bytes" << std::endl;
    
    auto soundData = co_await soundLoader;
    std::cout << "サウンド読み込み完了: " << soundData.size() << " bytes" << std::endl;
    
    auto modelData = co_await modelLoader;
    std::cout << "モデル読み込み完了: " << modelData.size() << " bytes" << std::endl;
    
    std::cout << "全リソース読み込み完了！" << std::endl;
}

// 実践例：プレイヤーとの対話
Task<void> playerInteractionSequence()
{
    std::cout << "\n=== プレイヤー対話シーケンス ===" << std::endl;
    
    // プレイヤーの入力を待機
    auto input1 = co_await InputAwaiter("攻撃");
    std::cout << "プレイヤーが「" << input1 << "」を選択" << std::endl;
    
    // ゲームの応答
    co_await sleep(std::chrono::milliseconds(500));
    std::cout << "敵にダメージを与えた！" << std::endl;
    
    // 次の入力を待機
    auto input2 = co_await InputAwaiter("防御");
    std::cout << "プレイヤーが「" << input2 << "」を選択" << std::endl;
    
    co_await sleep(std::chrono::milliseconds(300));
    std::cout << "敵の攻撃を防いだ！" << std::endl;
}

// 実践例：ネットワーク通信
Task<void> networkCommunicationExample()
{
    std::cout << "\n=== ネットワーク通信例 ===" << std::endl;
    
    try
    {
        // タイムアウト付きネットワーク要求
        auto request = with_timeout(
            NetworkRequest("api.game.com/player_data", 150),
            std::chrono::milliseconds(200)
        );
        
        auto response = co_await request;
        std::cout << "ネットワーク応答: " << response << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "ネットワークエラー: " << e.what() << std::endl;
    }
}

// 実践例：条件待機
Task<void> conditionalWaitingExample()
{
    std::cout << "\n=== 条件待機例 ===" << std::endl;
    
    std::atomic<int> playerHealth{100};
    std::atomic<bool> enemyDefeated{false};
    
    // 別スレッドで戦闘をシミュレート
    std::thread combatSimulation([&]() {
        for (int i = 0; i < 10; ++i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            playerHealth -= 10;
            std::cout << "プレイヤー体力: " << playerHealth.load() << std::endl;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        enemyDefeated.store(true);
        std::cout << "敵を倒した！" << std::endl;
    });
    
    // 敵が倒されるまで待機
    co_await wait_until([&]() { return enemyDefeated.load(); });
    
    std::cout << "戦闘終了！最終体力: " << playerHealth.load() << std::endl;
    
    combatSimulation.join();
}

// 実践例：イベント駆動処理
Task<void> eventDrivenExample()
{
    std::cout << "\n=== イベント駆動処理例 ===" << std::endl;
    
    Event gameEvent;
    
    // 別スレッドでイベントを発火
    std::thread eventTrigger([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "イベント発火！" << std::endl;
        gameEvent.signal();
    });
    
    std::cout << "イベント待機中..." << std::endl;
    co_await gameEvent;
    std::cout << "イベントを受信しました！" << std::endl;
    
    eventTrigger.join();
}

int main()
{
    std::cout << "=== co_await と Awaitable 型 ===" << std::endl;
    
    // 1. 基本的なタイマー
    std::cout << "\n--- 基本的なタイマー ---" << std::endl;
    
    auto timerTask = []() -> Task<void> {
        std::cout << "タイマー開始" << std::endl;
        co_await sleep(std::chrono::milliseconds(100));
        std::cout << "タイマー完了" << std::endl;
    }();
    
    timerTask.get();
    
    // 2. ゲームロード画面
    auto loadingTask = gameLoadingSequence();
    loadingTask.get();
    
    // 3. プレイヤー対話
    auto interactionTask = playerInteractionSequence();
    interactionTask.get();
    
    // 4. ネットワーク通信
    auto networkTask = networkCommunicationExample();
    networkTask.get();
    
    // 5. 条件待機
    auto conditionalTask = conditionalWaitingExample();
    conditionalTask.get();
    
    // 6. イベント駆動処理
    auto eventTask = eventDrivenExample();
    eventTask.get();
    
    std::cout << "\n=== Awaitable 型の利点 ===" << std::endl;
    std::cout << "- カスタム待機条件を簡単に実装" << std::endl;
    std::cout << "- 統一されたco_awaitインターフェース" << std::endl;
    std::cout << "- 型安全な非同期プログラミング" << std::endl;
    std::cout << "- 組み合わせ可能な非同期操作" << std::endl;
    std::cout << "- ゲーム開発特有の待機パターンに最適" << std::endl;
    
    return 0;
}

// まとめ：
// - Awaitable型でカスタム待機動作を実装
// - await_ready, await_suspend, await_resumeの3つのメソッドが重要
// - co_awaitで統一されたインターフェースを提供
// - タイムアウト、条件待機、イベント待機などの高度なパターンが可能
// - ゲーム開発では入力待機、リソース読み込み、ネットワーク通信で活用