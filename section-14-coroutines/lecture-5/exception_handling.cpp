// コルーチンでの例外処理
// 非同期処理における例外の安全な伝播と処理

#include <iostream>
#include <coroutine>
#include <exception>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <memory>
#include <atomic>
#include <vector>
#include <string>
#include <future>

// 例外安全なTask型
template<typename T>
class SafeTask
{
public:
    struct promise_type
    {
        T value;
        std::exception_ptr exception;
        std::coroutine_handle<> waitingCoroutine;
        
        SafeTask get_return_object()
        {
            return SafeTask{std::coroutine_handle<promise_type>::from_promise(*this)};
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
            try
            {
                value = std::move(val);
            }
            catch (...)
            {
                exception = std::current_exception();
            }
        }
        
        void unhandled_exception()
        {
            exception = std::current_exception();
        }
    };
    
    std::coroutine_handle<promise_type> handle;
    
    explicit SafeTask(std::coroutine_handle<promise_type> h) : handle(h) {}
    
    ~SafeTask()
    {
        if (handle)
            handle.destroy();
    }
    
    SafeTask(const SafeTask&) = delete;
    SafeTask& operator=(const SafeTask&) = delete;
    
    SafeTask(SafeTask&& other) noexcept : handle(other.handle)
    {
        other.handle = {};
    }
    
    SafeTask& operator=(SafeTask&& other) noexcept
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
        return std::move(handle.promise().value);
    }
    
    // 同期取得（例外安全）
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
        
        return std::move(handle.promise().value);
    }
    
    // 例外チェック
    bool has_exception() const
    {
        return handle.done() && handle.promise().exception != nullptr;
    }
    
    std::exception_ptr get_exception() const
    {
        return handle.promise().exception;
    }
};

// void用の特殊化
template<>
class SafeTask<void>
{
public:
    struct promise_type
    {
        std::exception_ptr exception;
        std::coroutine_handle<> waitingCoroutine;
        
        SafeTask get_return_object()
        {
            return SafeTask{std::coroutine_handle<promise_type>::from_promise(*this)};
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
        
        void return_void() {}
        
        void unhandled_exception()
        {
            exception = std::current_exception();
        }
    };
    
    std::coroutine_handle<promise_type> handle;
    
    explicit SafeTask(std::coroutine_handle<promise_type> h) : handle(h) {}
    
    ~SafeTask()
    {
        if (handle)
            handle.destroy();
    }
    
    SafeTask(const SafeTask&) = delete;
    SafeTask& operator=(const SafeTask&) = delete;
    
    SafeTask(SafeTask&& other) noexcept : handle(other.handle)
    {
        other.handle = {};
    }
    
    SafeTask& operator=(SafeTask&& other) noexcept
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
    
    void await_resume()
    {
        if (handle.promise().exception)
        {
            std::rethrow_exception(handle.promise().exception);
        }
    }
    
    void get()
    {
        while (!handle.done())
        {
            handle.resume();
        }
        
        if (handle.promise().exception)
        {
            std::rethrow_exception(handle.promise().exception);
        }
    }
    
    bool has_exception() const
    {
        return handle.done() && handle.promise().exception != nullptr;
    }
    
    std::exception_ptr get_exception() const
    {
        return handle.promise().exception;
    }
};

// ゲーム専用の例外クラス
class GameException : public std::exception
{
private:
    std::string message_;
    
public:
    explicit GameException(const std::string& message) : message_(message) {}
    
    const char* what() const noexcept override
    {
        return message_.c_str();
    }
};

class ResourceLoadException : public GameException
{
public:
    explicit ResourceLoadException(const std::string& resource)
        : GameException("リソース読み込みエラー: " + resource) {}
};

class NetworkException : public GameException
{
public:
    explicit NetworkException(const std::string& url)
        : GameException("ネットワークエラー: " + url) {}
};

class GameStateException : public GameException
{
public:
    explicit GameStateException(const std::string& state)
        : GameException("ゲーム状態エラー: " + state) {}
};

// 例外が発生する可能性のある非同期関数
SafeTask<std::string> riskyResourceLoad(const std::string& filename, bool shouldFail = false)
{
    std::cout << "リソース読み込み開始: " << filename << std::endl;
    
    // 読み込み処理をシミュレート
    co_await [](std::chrono::milliseconds duration) -> SafeTask<void> {
        std::this_thread::sleep_for(duration);
        co_return;
    }(std::chrono::milliseconds(100));
    
    if (shouldFail)
    {
        throw ResourceLoadException(filename);
    }
    
    std::cout << "リソース読み込み成功: " << filename << std::endl;
    co_return "リソースデータ: " + filename;
}

SafeTask<int> riskyCalculation(int input, bool shouldFail = false)
{
    std::cout << "計算開始: " << input << std::endl;
    
    co_await [](std::chrono::milliseconds duration) -> SafeTask<void> {
        std::this_thread::sleep_for(duration);
        co_return;
    }(std::chrono::milliseconds(50));
    
    if (shouldFail)
    {
        throw std::invalid_argument("無効な入力値: " + std::to_string(input));
    }
    
    if (input < 0)
    {
        throw std::domain_error("負の値は処理できません");
    }
    
    int result = input * input + 42;
    std::cout << "計算完了: " << input << " -> " << result << std::endl;
    co_return result;
}

SafeTask<void> riskyNetworkOperation(const std::string& url, bool shouldFail = false)
{
    std::cout << "ネットワーク操作開始: " << url << std::endl;
    
    co_await [](std::chrono::milliseconds duration) -> SafeTask<void> {
        std::this_thread::sleep_for(duration);
        co_return;
    }(std::chrono::milliseconds(200));
    
    if (shouldFail)
    {
        throw NetworkException(url);
    }
    
    std::cout << "ネットワーク操作成功: " << url << std::endl;
}

// 例外処理のパターン1: try-catch
SafeTask<void> basicExceptionHandling()
{
    std::cout << "\n=== 基本的な例外処理 ===" << std::endl;
    
    try
    {
        // 成功するケース
        auto result1 = co_await riskyCalculation(10, false);
        std::cout << "結果1: " << result1 << std::endl;
        
        // 失敗するケース
        auto result2 = co_await riskyCalculation(20, true);
        std::cout << "この行は実行されない: " << result2 << std::endl;
    }
    catch (const std::invalid_argument& e)
    {
        std::cout << "引数エラーをキャッチ: " << e.what() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "その他の例外をキャッチ: " << e.what() << std::endl;
    }
    
    std::cout << "例外処理後も継続" << std::endl;
}

// 例外処理のパターン2: 例外チェーン
SafeTask<void> exceptionChaining()
{
    std::cout << "\n=== 例外チェーン ===" << std::endl;
    
    try
    {
        // 最初の操作
        auto resource = co_await riskyResourceLoad("texture.png", false);
        std::cout << "取得したリソース: " << resource << std::endl;
        
        // 2番目の操作（失敗）
        co_await riskyNetworkOperation("api.game.com", true);
        
        // 3番目の操作（実行されない）
        auto calc = co_await riskyCalculation(100, false);
        std::cout << "この計算は実行されない: " << calc << std::endl;
    }
    catch (const NetworkException& e)
    {
        std::cout << "ネットワーク例外をキャッチ: " << e.what() << std::endl;
        
        // 例外発生時の代替処理
        try
        {
            std::cout << "代替処理を実行中..." << std::endl;
            auto fallback = co_await riskyCalculation(50, false);
            std::cout << "代替処理結果: " << fallback << std::endl;
        }
        catch (const std::exception& fallbackE)
        {
            std::cout << "代替処理も失敗: " << fallbackE.what() << std::endl;
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "予期しない例外: " << e.what() << std::endl;
    }
}

// 例外処理のパターン3: 並列処理での例外
SafeTask<void> parallelExceptionHandling()
{
    std::cout << "\n=== 並列処理での例外処理 ===" << std::endl;
    
    std::vector<SafeTask<int>> tasks;
    
    // 複数のタスクを作成（一部は失敗する）
    tasks.push_back(riskyCalculation(10, false));
    tasks.push_back(riskyCalculation(20, true));   // 失敗
    tasks.push_back(riskyCalculation(30, false));
    tasks.push_back(riskyCalculation(-5, false));  // 失敗（負の値）
    
    std::vector<int> results;
    std::vector<std::string> errors;
    
    // 各タスクを個別に処理
    for (auto& task : tasks)
    {
        try
        {
            auto result = co_await task;
            results.push_back(result);
            std::cout << "成功: " << result << std::endl;
        }
        catch (const std::exception& e)
        {
            errors.push_back(e.what());
            std::cout << "失敗: " << e.what() << std::endl;
        }
    }
    
    std::cout << "並列処理結果:" << std::endl;
    std::cout << "  成功したタスク数: " << results.size() << std::endl;
    std::cout << "  失敗したタスク数: " << errors.size() << std::endl;
}

// 例外処理のパターン4: リソース管理との組み合わせ
class GameResourceManager
{
private:
    std::vector<std::string> loadedResources_;
    
public:
    ~GameResourceManager()
    {
        for (const auto& resource : loadedResources_)
        {
            std::cout << "リソース解放: " << resource << std::endl;
        }
    }
    
    SafeTask<void> loadAllResources()
    {
        std::cout << "\n=== リソース管理での例外処理 ===" << std::endl;
        
        std::vector<std::string> resourceList = {
            "player.png",
            "enemy.png",
            "background.jpg",
            "sound.wav"
        };
        
        try
        {
            for (size_t i = 0; i < resourceList.size(); ++i)
            {
                bool shouldFail = (i == 2);  // 3番目のリソースで失敗
                
                auto resource = co_await riskyResourceLoad(resourceList[i], shouldFail);
                loadedResources_.push_back(resourceList[i]);
                std::cout << "読み込み成功: " << resourceList[i] << std::endl;
            }
            
            std::cout << "全リソース読み込み完了" << std::endl;
        }
        catch (const ResourceLoadException& e)
        {
            std::cout << "リソース読み込み失敗: " << e.what() << std::endl;
            std::cout << "既に読み込まれたリソース数: " << loadedResources_.size() << std::endl;
            
            // 部分的な成功でも処理を継続
            if (!loadedResources_.empty())
            {
                std::cout << "既存のリソースで処理を継続" << std::endl;
            }
        }
    }
};

// 例外処理のパターン5: カスタム例外ハンドラ
template<typename T>
class TaskWithCustomHandler
{
private:
    SafeTask<T> task_;
    std::function<void(const std::exception&)> errorHandler_;
    
public:
    TaskWithCustomHandler(SafeTask<T> task, std::function<void(const std::exception&)> handler)
        : task_(std::move(task)), errorHandler_(handler) {}
    
    SafeTask<std::optional<T>> execute()
    {
        try
        {
            auto result = co_await task_;
            co_return std::make_optional(result);
        }
        catch (const std::exception& e)
        {
            if (errorHandler_)
            {
                errorHandler_(e);
            }
            co_return std::nullopt;
        }
    }
};

SafeTask<void> customExceptionHandling()
{
    std::cout << "\n=== カスタム例外ハンドラ ===" << std::endl;
    
    auto errorHandler = [](const std::exception& e) {
        std::cout << "カスタムハンドラ: " << e.what() << std::endl;
        std::cout << "エラーログに記録しました" << std::endl;
    };
    
    // 成功ケース
    {
        TaskWithCustomHandler taskWrapper(
            riskyCalculation(25, false),
            errorHandler
        );
        
        auto result = co_await taskWrapper.execute();
        if (result.has_value())
        {
            std::cout << "成功結果: " << result.value() << std::endl;
        }
    }
    
    // 失敗ケース
    {
        TaskWithCustomHandler taskWrapper(
            riskyCalculation(25, true),
            errorHandler
        );
        
        auto result = co_await taskWrapper.execute();
        if (!result.has_value())
        {
            std::cout << "タスクが失敗しましたが、プログラムは継続" << std::endl;
        }
    }
}

// 例外処理のパターン6: finally ブロックのシミュレーション
template<typename CleanupFunc>
class ScopeGuard
{
private:
    CleanupFunc cleanup_;
    bool dismissed_;
    
public:
    explicit ScopeGuard(CleanupFunc cleanup)
        : cleanup_(cleanup), dismissed_(false) {}
    
    ~ScopeGuard()
    {
        if (!dismissed_)
        {
            cleanup_();
        }
    }
    
    void dismiss()
    {
        dismissed_ = true;
    }
};

template<typename CleanupFunc>
ScopeGuard<CleanupFunc> make_scope_guard(CleanupFunc cleanup)
{
    return ScopeGuard<CleanupFunc>(cleanup);
}

SafeTask<void> finallyBlockSimulation()
{
    std::cout << "\n=== Finally ブロックのシミュレーション ===" << std::endl;
    
    auto cleanup = make_scope_guard([]() {
        std::cout << "クリーンアップ処理実行（finallyブロック相当）" << std::endl;
    });
    
    try
    {
        std::cout << "重要な処理開始" << std::endl;
        
        auto result = co_await riskyCalculation(15, true);  // 失敗する
        std::cout << "この行は実行されない" << std::endl;
        
        cleanup.dismiss();  // 正常完了時はクリーンアップを無効化
    }
    catch (const std::exception& e)
    {
        std::cout << "例外発生: " << e.what() << std::endl;
        // cleanupは自動的に実行される
    }
    
    std::cout << "処理終了" << std::endl;
}

int main()
{
    std::cout << "=== コルーチンでの例外処理 ===" << std::endl;
    
    // 1. 基本的な例外処理
    auto basicTask = basicExceptionHandling();
    basicTask.get();
    
    // 2. 例外チェーン
    auto chainTask = exceptionChaining();
    chainTask.get();
    
    // 3. 並列処理での例外
    auto parallelTask = parallelExceptionHandling();
    parallelTask.get();
    
    // 4. リソース管理
    {
        GameResourceManager manager;
        auto resourceTask = manager.loadAllResources();
        resourceTask.get();
    }
    
    // 5. カスタム例外ハンドラ
    auto customTask = customExceptionHandling();
    customTask.get();
    
    // 6. Finally ブロック
    auto finallyTask = finallyBlockSimulation();
    finallyTask.get();
    
    std::cout << "\n=== 例外処理のベストプラクティス ===" << std::endl;
    std::cout << "1. unhandled_exception()で例外を確実にキャッチ" << std::endl;
    std::cout << "2. exception_ptrで例外を安全に伝播" << std::endl;
    std::cout << "3. RAIIパターンでリソースを自動管理" << std::endl;
    std::cout << "4. 型固有の例外ハンドリングで適切な対処" << std::endl;
    std::cout << "5. 並列処理では個別の例外処理を実装" << std::endl;
    std::cout << "6. スコープガードでクリーンアップを保証" << std::endl;
    
    return 0;
}

// まとめ：
// - promise_type::unhandled_exception()で例外を安全にキャッチ
// - std::exception_ptrで例外情報を保存・伝播
// - co_awaitした時点で例外が再スロー
// - 通常のtry-catch構文が使用可能
// - RAIIパターンとの組み合わせで確実なリソース管理
// - ゲーム開発では特に非同期読み込みとネットワーク処理で重要