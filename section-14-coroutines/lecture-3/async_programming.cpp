// 非同期プログラミング（Asynchronous Programming）
// コルーチンによる非同期処理の基礎と応用

#include <iostream>
#include <coroutine>
#include <future>
#include <thread>
#include <chrono>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <random>

// 基本的なTask型
template<typename T>
class Task
{
public:
    struct promise_type
    {
        T value;
        std::exception_ptr exception;
        
        Task get_return_object()
        {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        
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
    
    // ムーブのみ可能
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
    
    bool done() const
    {
        return handle.done();
    }
    
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
    
    // co_await サポート
    bool await_ready() const { return handle.done(); }
    
    void await_suspend(std::coroutine_handle<> waiting)
    {
        // 単純な実装：すぐに再開
        if (!handle.done())
        {
            handle.resume();
        }
        waiting.resume();
    }
    
    T await_resume()
    {
        if (handle.promise().exception)
        {
            std::rethrow_exception(handle.promise().exception);
        }
        return handle.promise().value;
    }
};

// 非同期タイマー
class AsyncTimer
{
public:
    struct promise_type
    {
        AsyncTimer get_return_object()
        {
            return AsyncTimer{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() {}
    };
    
    std::coroutine_handle<promise_type> handle;
    
    explicit AsyncTimer(std::coroutine_handle<promise_type> h) : handle(h) {}
    
    ~AsyncTimer()
    {
        if (handle)
            handle.destroy();
    }
    
    AsyncTimer(const AsyncTimer&) = delete;
    AsyncTimer& operator=(const AsyncTimer&) = delete;
    
    AsyncTimer(AsyncTimer&& other) noexcept : handle(other.handle)
    {
        other.handle = {};
    }
    
    AsyncTimer& operator=(AsyncTimer&& other) noexcept
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
    
    void wait()
    {
        while (!handle.done())
        {
            handle.resume();
        }
    }
};

// 待機可能な型
struct WaitableTimer
{
    std::chrono::milliseconds duration;
    
    explicit WaitableTimer(std::chrono::milliseconds d) : duration(d) {}
    
    bool await_ready() const { return false; }
    
    void await_suspend(std::coroutine_handle<> handle)
    {
        std::thread([handle, duration = this->duration]() {
            std::this_thread::sleep_for(duration);
            handle.resume();
        }).detach();
    }
    
    void await_resume() {}
};

// 待機関数
WaitableTimer wait_for(std::chrono::milliseconds duration)
{
    return WaitableTimer(duration);
}

// ゲームのリソース読み込み
struct GameResource
{
    std::string name;
    std::vector<char> data;
    bool loaded;
    
    GameResource(const std::string& n) : name(n), loaded(false) {}
};

// 非同期リソース読み込み
Task<std::shared_ptr<GameResource>> loadResourceAsync(const std::string& filename, int sizeKB)
{
    std::cout << "リソース読み込み開始: " << filename << " (" << sizeKB << "KB)" << std::endl;
    
    auto resource = std::make_shared<GameResource>(filename);
    
    // ファイルサイズに応じた読み込み時間をシミュレート
    co_await wait_for(std::chrono::milliseconds(sizeKB * 2));
    
    // データ生成（実際の読み込みをシミュレート）
    resource->data.resize(sizeKB * 1024);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    
    for (auto& byte : resource->data)
    {
        byte = static_cast<char>(dis(gen));
    }
    
    resource->loaded = true;
    
    std::cout << "リソース読み込み完了: " << filename << std::endl;
    co_return resource;
}

// 非同期計算処理
Task<int> performHeavyCalculation(int input)
{
    std::cout << "重い計算開始: " << input << std::endl;
    
    // 計算時間をシミュレート
    co_await wait_for(std::chrono::milliseconds(100 + (input % 100)));
    
    int result = 0;
    for (int i = 0; i < input; ++i)
    {
        result += i * i;
    }
    
    std::cout << "重い計算完了: " << input << " -> " << result << std::endl;
    co_return result;
}

// ゲームのAI思考ルーチン
Task<std::string> aiThinkingRoutine(const std::string& playerAction)
{
    std::cout << "AI思考開始: プレイヤーアクション「" << playerAction << "」に対応中..." << std::endl;
    
    // AI思考時間をシミュレート
    co_await wait_for(std::chrono::milliseconds(200));
    
    std::vector<std::string> responses = {
        "攻撃で対抗",
        "防御態勢",
        "回避行動",
        "特殊技発動",
        "アイテム使用"
    };
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, responses.size() - 1);
    
    std::string aiAction = responses[dis(gen)];
    
    std::cout << "AI思考完了: 「" << aiAction << "」を選択" << std::endl;
    co_return aiAction;
}

// 並列タスクの実行
Task<std::vector<std::string>> parallelAIProcessing()
{
    std::cout << "\n=== 並列AI処理開始 ===\n" << std::endl;
    
    // 複数のAIエージェントが同時に思考
    auto ai1 = aiThinkingRoutine("プレイヤーの攻撃");
    auto ai2 = aiThinkingRoutine("プレイヤーの魔法");
    auto ai3 = aiThinkingRoutine("プレイヤーの移動");
    
    std::vector<std::string> results;
    
    // 各AIの結果を待機
    results.push_back(co_await ai1);
    results.push_back(co_await ai2);
    results.push_back(co_await ai3);
    
    std::cout << "\n=== 並列AI処理完了 ===\n" << std::endl;
    co_return results;
}

// ゲームの初期化シーケンス
Task<bool> gameInitializationSequence()
{
    std::cout << "\n=== ゲーム初期化シーケンス開始 ===\n" << std::endl;
    
    try
    {
        // テクスチャファイルの読み込み
        auto textureTask = loadResourceAsync("player_texture.png", 512);
        auto soundTask = loadResourceAsync("background_music.wav", 2048);
        auto modelTask = loadResourceAsync("enemy_model.obj", 1024);
        
        // 設定ファイルの処理
        auto configTask = performHeavyCalculation(1000);
        
        // 順次待機（依存関係がある場合）
        auto texture = co_await textureTask;
        std::cout << "テクスチャ読み込み済み: " << texture->name << std::endl;
        
        // 並列待機（依存関係がない場合）
        auto sound = co_await soundTask;
        auto model = co_await modelTask;
        auto config = co_await configTask;
        
        std::cout << "サウンド読み込み済み: " << sound->name << std::endl;
        std::cout << "モデル読み込み済み: " << model->name << std::endl;
        std::cout << "設定処理完了: " << config << std::endl;
        
        // 最終初期化
        co_await wait_for(std::chrono::milliseconds(100));
        
        std::cout << "\n=== ゲーム初期化完了 ===\n" << std::endl;
        co_return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "初期化エラー: " << e.what() << std::endl;
        co_return false;
    }
}

// アニメーション更新の非同期処理
Task<void> animationUpdateLoop()
{
    std::cout << "\n=== アニメーション更新ループ開始 ===\n" << std::endl;
    
    for (int frame = 0; frame < 10; ++frame)
    {
        std::cout << "フレーム " << frame << " 更新中..." << std::endl;
        
        // 16ms待機（60FPS相当）
        co_await wait_for(std::chrono::milliseconds(16));
        
        // アニメーション計算をシミュレート
        float progress = static_cast<float>(frame) / 9.0f;
        std::cout << "  アニメーション進行度: " << (progress * 100) << "%" << std::endl;
    }
    
    std::cout << "アニメーション完了\n" << std::endl;
}

// ネットワーク通信のシミュレーション
Task<std::string> networkRequest(const std::string& url, int latencyMs)
{
    std::cout << "ネットワーク要求開始: " << url << std::endl;
    
    // ネットワーク遅延をシミュレート
    co_await wait_for(std::chrono::milliseconds(latencyMs));
    
    // レスポンスデータ生成
    std::string response = "Response from " + url + " (latency: " + std::to_string(latencyMs) + "ms)";
    
    std::cout << "ネットワーク要求完了: " << url << std::endl;
    co_return response;
}

// 複数ネットワーク要求の処理
Task<std::vector<std::string>> multipleNetworkRequests()
{
    std::cout << "\n=== 複数ネットワーク要求処理 ===\n" << std::endl;
    
    // 異なる遅延でネットワーク要求を実行
    auto request1 = networkRequest("api.game.com/player", 50);
    auto request2 = networkRequest("api.game.com/inventory", 100);
    auto request3 = networkRequest("api.game.com/friends", 75);
    
    std::vector<std::string> responses;
    
    // 並列実行で効率化
    responses.push_back(co_await request1);
    responses.push_back(co_await request2);
    responses.push_back(co_await request3);
    
    std::cout << "\n=== 全ネットワーク要求完了 ===\n" << std::endl;
    co_return responses;
}

// エラーハンドリングを含む非同期処理
Task<int> riskyAsyncOperation(bool shouldFail)
{
    std::cout << "リスクのある非同期操作開始" << std::endl;
    
    co_await wait_for(std::chrono::milliseconds(50));
    
    if (shouldFail)
    {
        throw std::runtime_error("操作が失敗しました");
    }
    
    std::cout << "リスクのある非同期操作成功" << std::endl;
    co_return 42;
}

// エラーハンドリングのデモ
Task<void> errorHandlingDemo()
{
    std::cout << "\n=== エラーハンドリングデモ ===\n" << std::endl;
    
    try
    {
        // 成功するケース
        auto result1 = co_await riskyAsyncOperation(false);
        std::cout << "成功結果: " << result1 << std::endl;
        
        // 失敗するケース
        auto result2 = co_await riskyAsyncOperation(true);
        std::cout << "この行は実行されない" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "例外をキャッチ: " << e.what() << std::endl;
    }
    
    std::cout << "エラーハンドリング完了\n" << std::endl;
}

// タイムアウト機能
template<typename T>
Task<T> withTimeout(Task<T> task, std::chrono::milliseconds timeout)
{
    // 注意：これは簡単な例で、実際のタイムアウト実装はより複雑
    std::cout << "タイムアウト付きタスク開始 (" << timeout.count() << "ms)" << std::endl;
    
    // シンプルな実装：タスクを実行して結果を返す
    // 実際の実装では競合状態やキャンセレーションを考慮する必要がある
    co_return co_await task;
}

int main()
{
    std::cout << "=== 非同期プログラミング（Asynchronous Programming） ===" << std::endl;
    
    // 1. 基本的な非同期タスク
    std::cout << "\n--- 基本的な非同期タスク ---" << std::endl;
    
    auto resourceTask = loadResourceAsync("game_data.bin", 256);
    auto calculationTask = performHeavyCalculation(500);
    
    // 結果を取得
    auto resource = resourceTask.get();
    auto calcResult = calculationTask.get();
    
    std::cout << "リソースサイズ: " << resource->data.size() << " bytes" << std::endl;
    std::cout << "計算結果: " << calcResult << std::endl;
    
    // 2. ゲーム初期化シーケンス
    auto initTask = gameInitializationSequence();
    bool initSuccess = initTask.get();
    std::cout << "初期化結果: " << (initSuccess ? "成功" : "失敗") << std::endl;
    
    // 3. 並列AI処理
    auto aiTask = parallelAIProcessing();
    auto aiResults = aiTask.get();
    
    std::cout << "AI処理結果:" << std::endl;
    for (size_t i = 0; i < aiResults.size(); ++i)
    {
        std::cout << "  AI" << (i + 1) << ": " << aiResults[i] << std::endl;
    }
    
    // 4. アニメーション更新
    auto animTask = animationUpdateLoop();
    animTask.wait();
    
    // 5. ネットワーク処理
    auto networkTask = multipleNetworkRequests();
    auto networkResults = networkTask.get();
    
    std::cout << "ネットワーク結果:" << std::endl;
    for (const auto& response : networkResults)
    {
        std::cout << "  " << response << std::endl;
    }
    
    // 6. エラーハンドリング
    auto errorTask = errorHandlingDemo();
    errorTask.wait();
    
    // 7. 実践例：ゲームフレーム処理
    std::cout << "\n--- ゲームフレーム処理例 ---" << std::endl;
    
    for (int frame = 0; frame < 3; ++frame)
    {
        std::cout << "\nフレーム " << frame << " 開始:" << std::endl;
        
        // フレーム毎の非同期処理
        auto frameAI = aiThinkingRoutine("フレーム" + std::to_string(frame) + "のアクション");
        auto frameAnim = animationUpdateLoop();
        
        // AI結果を取得
        auto aiAction = frameAI.get();
        std::cout << "フレーム " << frame << " AI行動: " << aiAction << std::endl;
        
        // 次のフレームまで待機
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    std::cout << "\n=== 非同期プログラミングの利点 ===" << std::endl;
    std::cout << "- ブロッキングを避けてUI応答性を向上" << std::endl;
    std::cout << "- I/O集約的なタスクの並列化" << std::endl;
    std::cout << "- リソース効率的な待機処理" << std::endl;
    std::cout << "- 複雑な非同期フローの簡潔な記述" << std::endl;
    std::cout << "- ゲーム開発：読み込み、AI、ネットワーク処理に最適" << std::endl;
    
    return 0;
}

// まとめ：
// - コルーチンで非同期処理を直感的に記述
// - co_await で非同期操作の完了を待機
// - Task型で非同期結果を管理
// - エラーハンドリングも同期処理と同様に記述可能
// - ゲーム開発では読み込み、AI、ネットワーク処理で威力を発揮